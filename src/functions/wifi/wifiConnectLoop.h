// pid PID::WIFI_CONNECT

#define WIFI_CONNECT_TIMEOUT_MS 15000

static unsigned long wifiConnectStart = 0;
static bool wifiConnectResultShown = false;
static bool wifiConnectPasswordDone = false;

static void startWifiConnection(const char* password) {
	wifiPassword = String(password);
	WiFi.disconnect(true);
	isWebInterfaceEnabled = false;
	WiFi.mode(WIFI_STA);
	delay(100);
	if (wifiPassword.length() > 0)
		WiFi.begin(ssid.c_str(), wifiPassword.c_str());
	else
		WiFi.begin(ssid.c_str());
	wifiConnectStart = millis();
	centeredPrint("Connecting...", SMALL_TEXT);
}

static String wifiPasswordKey() {
	return "w:" + ssid.substring(0, 13);
}

void wifiConnectLoop() {
	if (isSetup()) {
		wifiConnectPasswordDone = false;
		wifiConnectResultShown  = false;
		kbReset();

		if (WiFi.encryptionType(wifiScanIndex) == WIFI_AUTH_OPEN) {
			wifiConnectPasswordDone = true;
			startWifiConnection("");
		} else {
			String saved = getDataString(wifiPasswordKey().c_str());
			if (saved.length() > 0) {
				wifiConnectPasswordDone = true;
				startWifiConnection(saved.c_str());
			} else {
				wifiPassword = "";
				drawKeyboardUi();
			}
		}
		updateTimer();
	}

	// password keyboard phase
	if (!wifiConnectPasswordDone) {
		if (keyboardLoop(
			[]() {
				DISP.clear();
				changeProcess(PID::WIFI_SELECTED);
			},
			[](const char* buf) {
				wifiConnectPasswordDone = true;
				startWifiConnection(buf);
			},
			nullptr
		)) return;
		return;
	}

	// connection status phase
	if (!wifiConnectResultShown) {
		wl_status_t status = WiFi.status();
		bool timedOut = (millis() - wifiConnectStart > WIFI_CONNECT_TIMEOUT_MS);

		if (status == WL_CONNECTED) {
			wifiConnectResultShown = true;
			if (wifiPassword.length() > 0)
				setDataString(wifiPasswordKey().c_str(), wifiPassword.c_str());
			String ip = WiFi.localIP().toString();
			String lines[] = { "Connected!", ssid.substring(0, 16), ip };
			centeredPrintRows(lines, 3, SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 100);
			delay(50);
			DEVICE.Speaker.tone(2500, 100);
		} else if (status == WL_CONNECT_FAILED || status == WL_NO_SSID_AVAIL || timedOut) {
			wifiConnectResultShown = true;
			WiFi.disconnect(true);

			const char* reason;
			if (status == WL_NO_SSID_AVAIL) reason = "SSID not found";
			else if (timedOut) reason = "Timeout";
			else reason = "Wrong password";

			String lines[] = { "Failed", String(reason) };
			centeredPrintRows(lines, 2, SMALL_TEXT);
			DEVICE.Speaker.tone(500, 300);
		}
	}

	checkExit();
}
