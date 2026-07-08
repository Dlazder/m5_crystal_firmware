// PCAP file format utilities: structures and helpers for writing .pcap files
// with IEEE 802.11 + Radiotap headers (LINKTYPE_IEEE802_11_RADIOTAP = 127).

// --- PCAP file format structures -------------------------------------------
// __attribute__((packed)) ensures exact on-disk layout (no compiler padding).

struct __attribute__((packed)) pcap_hdr_s {
	uint32_t magic_number;   // 0xa1b2c3d4
	uint16_t version_major;  // 2
	uint16_t version_minor;  // 4
	int32_t  thiszone;       // GMT to local offset (0)
	uint32_t sigfigs;        // timestamp accuracy (0)
	uint32_t snaplen;        // max packet length
	uint32_t network;        // link type: 127 = IEEE 802.11 + radiotap
};

struct __attribute__((packed)) pcaprec_hdr_s {
	uint32_t ts_sec;
	uint32_t ts_usec;
	uint32_t incl_len;
	uint32_t orig_len;
};

// --- Radiotap header (16 bytes, packed) ------------------------------------
// Present bits: 1 (Flags) | 3 (Channel) | 5 (dBm Signal)
// Fields must appear in increasing present-bit order: 1, 3, 5.
// flags = 0x10 (FCS at end) — ESP32 sig_len includes FCS (4 bytes).

struct __attribute__((packed)) radiotap_hdr {
	uint8_t  it_version;     // 0
	uint8_t  it_pad;         // 1
	uint16_t it_len;         // 2-3
	uint32_t it_present;     // 4-7: bitmask of present fields
	uint8_t  flags;          // 8:  bit 1 — Flags (0x10 = FCS at end)
	uint8_t  pad1;           // 9:  align Channel to 2 bytes
	uint16_t chan_freq;      // 10-11: bit 3 — Channel freq (MHz)
	uint16_t chan_flags;     // 12-13: bit 3 — Channel flags
	int8_t   ant_signal;     // 14: bit 5 — dBm Antenna Signal
	uint8_t  pad2;           // 15: pad to 16 bytes
};

// --- Channel helpers --------------------------------------------------------

static uint16_t channelToFreq(int ch) {
	if (ch >= 1 && ch <= 13) return 2412 + (ch - 1) * 5;
	if (ch == 14)            return 2484;
	if (ch >= 36 && ch <= 196) return 5000 + ch * 5;
	return 2412; // fallback
}

static uint16_t channelFlagsFor(int ch) {
	return (ch >= 1 && ch <= 14) ? 0x000a   // CCK + 2 GHz spectrum
	                             : 0x0014;  // OFDM + 5 GHz spectrum
}

// --- PCAP write helpers -----------------------------------------------------

static void writePcapGlobalHeader(File& f) {
	pcap_hdr_s hdr;
	hdr.magic_number  = 0xa1b2c3d4;
	hdr.version_major = 2;
	hdr.version_minor = 4;
	hdr.thiszone      = 0;
	hdr.sigfigs       = 0;
	hdr.snaplen       = 65535;
	hdr.network       = 127; // LINKTYPE_IEEE802_11_RADIOTAP
	f.write((uint8_t*)&hdr, sizeof(hdr));
}

static void writePcapPacket(File& f, const uint8_t* data, uint16_t len,
                            int8_t rssi, uint32_t timestamp, int channel) {
	// PCAP record header
	pcaprec_hdr_s rec;
	rec.ts_sec   = timestamp / 1000000;
	rec.ts_usec  = timestamp % 1000000;
	rec.incl_len = sizeof(radiotap_hdr) + len;
	rec.orig_len = sizeof(radiotap_hdr) + len;
	f.write((uint8_t*)&rec, sizeof(rec));

	// Radiotap header
	radiotap_hdr rt;
	memset(&rt, 0, sizeof(rt));
	rt.it_version = 0;
	rt.it_len     = sizeof(radiotap_hdr);
	rt.it_present = (1 << 1) | (1 << 3) | (1 << 5); // Flags + Channel + Signal
	rt.flags      = 0x00; // No FCS flag (see radiotap_hdr comment)
	rt.chan_freq  = channelToFreq(channel);
	rt.chan_flags = channelFlagsFor(channel);
	rt.ant_signal = rssi;
	f.write((uint8_t*)&rt, sizeof(rt));

	// Raw 802.11 frame (FCS included in len, but flags=0x00 so parsers ignore last 4 bytes)
	f.write(data, len);
}

// --- PCAP to FT Hash conversion -------------------------------------------
// Reads a .pcap file from SD, extracts EAPOL handshake data, writes a
// .hash file for FT-Crack / hashcat mode 22000 (WPA*04*...).
// Returns true if a handshake was found and the hash file was written.

static bool pcapToFTHash(const String& pcapPath) {
	if (!sdBegin()) { Serial.println("pcapToFTHash: SD init failed"); return false; }
	File f = SD.open(pcapPath.c_str());
	if (!f) { Serial.printf("pcapToFTHash: cannot open %s\n", pcapPath.c_str()); return false; }
	Serial.printf("pcapToFTHash: opened %s (%u bytes)\n", pcapPath.c_str(), f.size());

	// Read and verify pcap global header (24 bytes)
	uint8_t gh[24];
	if (f.read(gh, 24) != 24) { Serial.println("pcapToFTHash: short global hdr"); f.close(); return false; }

	uint32_t magic = gh[0] | ((uint32_t)gh[1]<<8) | ((uint32_t)gh[2]<<16) | ((uint32_t)gh[3]<<24);
	if (magic != 0xa1b2c3d4) { f.close(); return false; } // not a valid pcap
	uint32_t network = gh[20] | ((uint32_t)gh[21]<<8) | ((uint32_t)gh[22]<<16) | ((uint32_t)gh[23]<<24);
	if (network != 127) { f.close(); return false; } // must be LINKTYPE_IEEE802_11_RADIOTAP

	// --- Storage for extracted handshake data ---
	uint8_t  apMac[6];     bool hasApMac   = false;
	uint8_t  staMac[6];    bool hasStaMac  = false;
	char     ssid[33];     uint8_t ssidLen = 0;
	uint8_t  mdid[2];      bool hasMdid    = false;
	uint8_t  anonce[32];   bool hasAnonce  = false;
	uint8_t  m2pdu[512];   uint16_t m2pduLen = 0;
	uint8_t  m2mic[16];
	uint8_t  mp = 0; // message pair bitmask
	uint8_t  r0khid[48];   uint8_t r0khidLen = 0;

	uint8_t  pktBuf[512];
	uint8_t  recHdr[16];

	while (f.read(recHdr, 16) == 16) {
		// PCAP record header (little-endian)
		uint32_t incl_len = recHdr[8] | ((uint32_t)recHdr[9]<<8)
		                  | ((uint32_t)recHdr[10]<<16) | ((uint32_t)recHdr[11]<<24);
		if (incl_len > sizeof(pktBuf) || incl_len < 16) break;
		if (f.read(pktBuf, incl_len) != (int)incl_len) break;

		// Radiotap header
		uint16_t rtLen = pktBuf[2] | (pktBuf[3] << 8);
		if (rtLen < 8 || rtLen > incl_len) continue;
		uint8_t* dot11 = pktBuf + rtLen;
		int dot11Len = incl_len - rtLen;
		if (dot11Len < 24) continue;

		// 802.11 Frame Control
		uint16_t fc = dot11[0] | (dot11[1] << 8);
		uint8_t  frameType = (fc >> 2) & 0x3;
		uint8_t  subtype   = (fc >> 4) & 0xF;
		bool     toDS      = (fc >> 8) & 1;
		bool     fromDS    = (fc >> 9) & 1;

		// --- Beacon (MGMT, subtype 8) ---
		if (frameType == 0 && subtype == 8 && !hasApMac) {
			memcpy(apMac, dot11 + 16, 6);
			hasApMac = true;
			Serial.printf("pcapToFTHash: beacon BSSID=%02X%02X%02X%02X%02X%02X\n",
				apMac[0],apMac[1],apMac[2],apMac[3],apMac[4],apMac[5]);

			// Walk tagged parameters (start at offset 36)
			int pos = 36;
			while (pos + 2 <= dot11Len - 4) { // -4 for possible FCS
				uint8_t tag = dot11[pos];
				uint8_t tlen = dot11[pos + 1];
				if (pos + 2 + tlen > dot11Len - 4) break;

				if (tag == 0 && ssidLen == 0 && tlen <= (int)sizeof(ssid)-1) {
					memcpy(ssid, dot11 + pos + 2, tlen);
					ssid[tlen] = '\0';
					ssidLen = tlen;
				}
				if (tag == 0x36 && !hasMdid && tlen >= 2) {
					memcpy(mdid, dot11 + pos + 2, 2);
					hasMdid = true;
				}
				// RSN IE: look for R0KH-ID sub-element (type 0x01)
				if (tag == 0x30 && r0khidLen == 0 && tlen >= 20) {
					const uint8_t* rsn = dot11 + pos + 2;
					int off = 2;                               // version
					off += 4;                                  // group cipher
					uint16_t pwCnt = rsn[off] | (rsn[off+1]<<8);
					off += 2 + pwCnt * 4;                      // pairwise list
					if (off + 2 > tlen) goto beacon_next_ie;
					uint16_t akmCnt = rsn[off] | (rsn[off+1]<<8);
					off += 2 + akmCnt * 4;                     // AKM list
					if (off + 2 > tlen) goto beacon_next_ie;
					off += 2;                                  // RSN capabilities
					if (off + 2 <= tlen) {
						uint16_t pmkidCnt = rsn[off] | (rsn[off+1]<<8);
						off += 2 + pmkidCnt * 16;
						if (off > tlen) goto beacon_next_ie;
					}
					while (off + 2 <= tlen) {
						uint8_t st = rsn[off]; uint8_t sl = rsn[off+1];
						if (off + 2 + sl > tlen) break;
						if (st == 0x01 && sl <= (int)sizeof(r0khid)) {
							memcpy(r0khid, rsn + off + 2, sl);
							r0khidLen = sl;
						}
						off += 2 + sl;
					}
				}
				beacon_next_ie:
				pos += 2 + tlen;
			}
			continue;
		}

		// --- Data frames ---
		if (frameType != 2) continue;

		int hdrLen = 24;
		if (toDS && fromDS) hdrLen += 6;        // Addr4
		if (subtype & 0x8) {                     // QoS Data
			hdrLen += 2;
			if ((fc >> 15) & 1) hdrLen += 4;    // +HTC
		}

		if (dot11Len < hdrLen + 8 + 4 + 77 + 16) continue; // need LLC + EAPOL hdr + MIC

		// Check LLC/SNAP: AA AA 03 00 00 00 88 8E
		if (dot11[hdrLen]   != 0xAA || dot11[hdrLen+1] != 0xAA
		 || dot11[hdrLen+2] != 0x03 || dot11[hdrLen+6] != 0x88
		 || dot11[hdrLen+7] != 0x8E) continue;

		// EAPOL type must be 3 (EAPOL-Key)
		if (dot11[hdrLen + 9] != 3) continue;

		// Key Info at EAPOL body offset 1 (big-endian)
		uint16_t keyInfo = (dot11[hdrLen + 13] << 8) | dot11[hdrLen + 14];
		bool kAck   = (keyInfo >> 7) & 1;
		bool kMic   = (keyInfo >> 8) & 1;
		bool kSecure = (keyInfo >> 9) & 1;

		// --- M1: ANonce ---
		if (kAck && !kMic && !hasAnonce) {
			if (dot11Len >= hdrLen + 25 + 32 + 4) { // ANonce(32) + FCS(4)
				memcpy(anonce, dot11 + hdrLen + 25, 32);
				hasAnonce = true;
				mp |= 1;
				Serial.printf("pcapToFTHash: M1 found, ANonce[0:8]=%02X%02X%02X%02X%02X%02X%02X%02X\n",
					anonce[0],anonce[1],anonce[2],anonce[3],anonce[4],anonce[5],anonce[6],anonce[7]);
			}
		}

		// --- M2: EAPOL PDU + MIC + STA MAC + R0KH-ID (tag 0x37) ---
		if (!kAck && kMic && !kSecure && m2pduLen == 0) {
			uint16_t eapolBodyLen = (dot11[hdrLen + 10] << 8) | dot11[hdrLen + 11];
			uint16_t pduLen = 4 + eapolBodyLen;
			if ((int)dot11Len >= hdrLen + 8 + pduLen + 4) {
				memcpy(m2pdu, dot11 + hdrLen + 8, pduLen);
				m2pduLen = pduLen;
				memcpy(m2mic, dot11 + hdrLen + 89, 16); // MIC at body offset 77
				mp |= 2;
				Serial.printf("pcapToFTHash: M2 found, pduLen=%u MIC[0:4]=%02X%02X%02X%02X\n",
					pduLen, m2mic[0],m2mic[1],m2mic[2],m2mic[3]);

				if (toDS && !fromDS) {
					memcpy(staMac, dot11 + 10, 6);
					hasStaMac = true;
				}

				// Parse M2 Key Data for tag 0x37 (Keenetic R0KH-ID)
				if (r0khidLen == 0 && pduLen >= 100) {
					uint16_t kdLen = (m2pdu[97] << 8) | m2pdu[98];
					if (kdLen > 0 && 99 + kdLen <= pduLen) {
						const uint8_t* kd = m2pdu + 99;
						int off = 0;
						while (off + 2 <= (int)kdLen) {
							uint8_t ieTag = kd[off]; uint8_t ieLen = kd[off+1];
							if (off + 2 + ieLen > (int)kdLen) break;
							if (ieTag == 0x37 && ieLen > 0) {
								// Skip leading zeros, then parse TLVs
								const uint8_t* d = kd + off + 2;
								int p = 0;
								while (p < ieLen && d[p] == 0) p++;
								while (p + 2 <= ieLen) {
									uint8_t st = d[p]; uint8_t sl = d[p+1];
									if (p + 2 + sl > ieLen) break;
									if (st == 0x03 && sl <= (int)sizeof(r0khid)) {
										memcpy(r0khid, d + p + 2, sl);
										r0khidLen = sl;
									}
									p += 2 + sl;
								}
							}
							off += 2 + ieLen;
						}
					}
				}
			}
		}
	}
	f.close();

	// --- Validate minimum data ---
	if (!hasAnonce || m2pduLen == 0 || !hasApMac) {
		Serial.printf("pcapToFTHash: missing data! hasAnonce=%d m2pduLen=%u hasApMac=%d\n",
			hasAnonce, m2pduLen, hasApMac);
		return false;
	}

	// --- Build hash line ---
	// Worst case: 8(WPA*04*) + 32(MIC) + 12*6(addrs) + 64(ssid) + 64(anonce)
	//   + 948(EAPOL max 474*2) + 2(mp) + 4(mdid) + 96(r0kh) + separators(~12) + newline
	//   = ~1300 bytes. 1536 gives safety margin.
	char hashLine[1536];
	int  hl = 0;
	#define HL_SAFE (hl < (int)sizeof(hashLine) - 2)

	// Helper: append raw string
	#define HASH_ADD(s) do { \
		const char* _s = (s); \
		while (*_s && HL_SAFE) hashLine[hl++] = *_s++; \
	} while(0)

	// Helper: append hex-encoded bytes
	#define HASH_HEX(data, len) do { \
		for (int _i = 0; _i < (int)(len) && HL_SAFE; _i++) { \
			uint8_t _b = (data)[_i]; \
			hashLine[hl++] = "0123456789ABCDEF"[_b >> 4]; \
			hashLine[hl++] = "0123456789ABCDEF"[_b & 0xF]; \
		} \
	} while(0)

	// Format: WPA*04*MIC*MAC_AP*MAC_CLIENT*ESSID*NONCE_AP*EAPOL_CLIENT*MESSAGEPAIR*MD-ID*R0KH-ID*R1KH-ID
	HASH_ADD("WPA*04*");

	HASH_HEX(m2mic, 16);      hashLine[hl++] = '*';
	HASH_HEX(apMac, 6);       hashLine[hl++] = '*';
	HASH_HEX(hasStaMac ? staMac : apMac, 6); hashLine[hl++] = '*';
	HASH_HEX((uint8_t*)ssid, ssidLen);       hashLine[hl++] = '*';
	HASH_HEX(anonce, 32);                    hashLine[hl++] = '*';

	// EAPOL_CLIENT with MIC zeroed
	{
		uint8_t m2copy[512];
		memcpy(m2copy, m2pdu, m2pduLen);
		if (m2pduLen >= 97) memset(m2copy + 81, 0, 16); // MIC at PDU offset 81
		HASH_HEX(m2copy, m2pduLen);
	}
	hashLine[hl++] = '*';

	// MessagePair
	hashLine[hl++] = "0123456789ABCDEF"[mp >> 4];
	hashLine[hl++] = "0123456789ABCDEF"[mp & 0xF];
	hashLine[hl++] = '*';

	// MD-ID
	if (hasMdid) {
		HASH_HEX(mdid, 2);
	} else {
		HASH_ADD("0000");
	}
	hashLine[hl++] = '*';

	// R0KH-ID
	if (r0khidLen > 0 && r0khidLen <= (int)sizeof(r0khid)) {
		HASH_HEX(r0khid, r0khidLen);
	} else {
		HASH_HEX(apMac, 6); // BSSID fallback
	}
	hashLine[hl++] = '*';

	// R1KH-ID = BSSID
	HASH_HEX(apMac, 6);
	hashLine[hl++] = '\n';
	hashLine[hl]   = '\0';

	#undef HASH_ADD
	#undef HASH_HEX

	// --- Write hash file (same name, .hash extension) ---
	String hashPath = pcapPath;
	hashPath.replace(".pcap", ".hash");
	if (hashPath == pcapPath) hashPath += ".hash"; // fallback if no .pcap in name

	Serial.printf("pcapToFTHash: hashLine %d bytes, starts: %.80s\n", hl, hashLine);

	File hf = SD.open(hashPath.c_str(), FILE_WRITE);
	if (!hf) { Serial.println("pcapToFTHash: cannot write hash file"); return false; }
	hf.write((uint8_t*)hashLine, hl);
	hf.close();
	Serial.printf("pcapToFTHash: wrote %d bytes to %s\n", hl, hashPath.c_str());
	return true;
}
