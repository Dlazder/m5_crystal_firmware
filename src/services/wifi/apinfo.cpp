#include "wifi.h"
#include <WiFi.h>

namespace Wifi {

// ═══════════════════════════════════════════════════════════════════════
// AP info probe (WPS / FT-PSK beacon detection)
// ═══════════════════════════════════════════════════════════════════════

static uint8_t _wiuBssid[6];
static int     _wiuChannel = 0;

static volatile bool _wiuBeaconDone = false;
static volatile bool _wiuHasWps = false;
static volatile bool _wiuHasFt = false;

static enum { WIU_IDLE, WIU_WAIT, WIU_DONE } _wiuState = WIU_IDLE;
static uint32_t _wiuTimer = 0;
static bool _wiuResultWps = false;
static bool _wiuResultFt = false;

static void _wiuBeaconCb(void* buf, wifi_promiscuous_pkt_type_t type) {
	if (_wiuBeaconDone) return;
	if (type != WIFI_PKT_MGMT) return;

	wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
	uint16_t len = pkt->rx_ctrl.sig_len;
	if (len < 40) return;

	uint8_t* frame = pkt->payload;
	uint16_t fc = frame[0] | (frame[1] << 8);
	if (((fc >> 2) & 0x3) != 0) return; // not MGMT
	if (((fc >> 4) & 0xF) != 8) return;             // not Beacon
	if (memcmp(frame + 16, _wiuBssid, 6) != 0) return; // not our BSSID

	// Walk tagged parameters: timestamp(8) + interval(2) + capabilities(2) = 12
	int pos = 36;
	int end = (int)len - 4; // exclude FCS
	while (pos + 2 <= end) {
		uint8_t tag  = frame[pos];
		uint8_t tlen = frame[pos + 1];
		if (pos + 2 + tlen > end) break;

		// RSN IE (0x30) — check for FT-PSK AKM suite 00-0F-AC-04
		if (tag == 0x30 && tlen >= 20) {
			const uint8_t* rsn = frame + pos + 2;
			int off = 2 + 4; // version(2) + group cipher suite(4)
			uint16_t pwCnt = rsn[off] | (rsn[off + 1] << 8);
			off += 2 + pwCnt * 4; // skip pairwise cipher list
			if (off + 2 <= tlen) {
				uint16_t akmCnt = rsn[off] | (rsn[off + 1] << 8);
				off += 2;
				for (int i = 0; i < (int)akmCnt && off + 4 <= tlen; i++) {
					if (rsn[off] == 0x00 && rsn[off + 1] == 0x0F
						&& rsn[off + 2] == 0xAC && rsn[off + 3] == 0x04) {
						_wiuHasFt = true;
					}
					off += 4;
				}
			}
		}

		// Vendor Specific IE (0xDD) — WPS: WFA OUI 00-50-F2 type 0x04
		if (tag == 0xDD && tlen >= 4) {
			const uint8_t* vs = frame + pos + 2;
			if (vs[0] == 0x00 && vs[1] == 0x50 && vs[2] == 0xF2 && vs[3] == 0x04) {
				_wiuHasWps = true;
			}
		}

		pos += 2 + tlen;
	}

	_wiuBeaconDone = true;
}

void wiuBegin(uint8_t* targetBssid, int targetChannel) {
	memcpy(_wiuBssid, targetBssid, 6);
	_wiuChannel = targetChannel;
	_wiuBeaconDone = false;
	_wiuHasWps = false;
	_wiuHasFt = false;
	_wiuResultWps = false;
	_wiuResultFt = false;

	WiFi.mode(WIFI_STA);
	delay(50);
	esp_wifi_set_channel(_wiuChannel, WIFI_SECOND_CHAN_NONE);
	esp_wifi_set_promiscuous_rx_cb(&_wiuBeaconCb);
	esp_wifi_set_promiscuous(true);

	_wiuTimer = millis();
	_wiuState = WIU_WAIT;
}

void wiuUpdate() {
	if (_wiuState != WIU_WAIT) return;
	if (_wiuBeaconDone || (millis() - _wiuTimer > 2500)) {
		esp_wifi_set_promiscuous(false);
		esp_wifi_set_promiscuous_rx_cb(nullptr);
		_wiuResultWps = _wiuHasWps;
		_wiuResultFt = _wiuHasFt;
		_wiuState = WIU_DONE;
	}
}

bool wiuDone() { return _wiuState == WIU_DONE; }
bool wiuHasWps() { return _wiuResultWps; }
bool wiuHasFt() { return _wiuResultFt; }

void wiuCleanup() {
	if (_wiuState == WIU_WAIT) {
		esp_wifi_set_promiscuous(false);
		esp_wifi_set_promiscuous_rx_cb(nullptr);
	}
	_wiuState = WIU_IDLE;
}

const char* wiuAuthStr(wifi_auth_mode_t m) {
	switch (m) {
		case WIFI_AUTH_OPEN:            return "Open";
		case WIFI_AUTH_WEP:             return "WEP";
		case WIFI_AUTH_WPA_PSK:         return "WPA";
		case WIFI_AUTH_WPA2_PSK:        return "WPA2";
		case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
		case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-Ent";
		case WIFI_AUTH_WPA3_PSK:        return "WPA3";
		case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/WPA3";
		case WIFI_AUTH_WAPI_PSK:        return "WAPI";
		case WIFI_AUTH_WPA3_ENT_192:    return "WPA3-Ent";
		default: return "?";
	}
}

} // namespace Wifi
