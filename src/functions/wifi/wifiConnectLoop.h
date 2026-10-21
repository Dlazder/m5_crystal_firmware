// PID::WIFI_CONNECT

#define WIFI_CONNECT_TIMEOUT_MS 15000

static unsigned long wifiConnectStart = 0;
static bool wifiConnectResultShown = false;
static bool wifiConnectPasswordDone = false;

static void startWifiConnection(const char* password) {
	isWebInterfaceEnabled = false;
	Wifi::selected.password = String(password);
	WiFi.mode(WIFI_STA);
	delay(100);
	if (Wifi::selected.password.length() > 0)
		WiFi.begin(Wifi::selected.ssid.c_str(), Wifi::selected.password.c_str());
	else
		WiFi.begin(Wifi::selected.ssid.c_str());
	wifiConnectStart = millis();
	centeredPrint(L->TXT_CONNECTING, MEDIUM_TEXT);
}


void wifiConnectLoop() {
	if (isSetup()) {
		wifiConnectPasswordDone = false;
		wifiConnectResultShown  = false;
		kbReset();

		if (WiFi.encryptionType(Wifi::selected.scanIndex) == WIFI_AUTH_OPEN) {
			wifiConnectPasswordDone = true;
			kbEnd();
			startWifiConnection("");
		} else {
			String saved = Wifi::loadWifiPassword(Wifi::selected.ssid);
			if (saved.length() > 0) {
				wifiConnectPasswordDone = true;
				kbEnd();
				startWifiConnection(saved.c_str());
			} else {
				Wifi::selected.password = "";
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
			if (Wifi::selected.password.length() > 0)
				Wifi::saveWifiPassword(Wifi::selected.ssid, Wifi::selected.password);
			String ip = WiFi.localIP().toString();
			String lines[] = { L->TXT_CONNECTED, Wifi::selected.ssid.substring(0, 16), ip };
			centeredPrintRows(lines, 3, MEDIUM_TEXT);
			soundSuccess();
			Wifi::autoNtpSync(timezoneOffset);
		} else if (status == WL_CONNECT_FAILED || status == WL_NO_SSID_AVAIL || timedOut) {
			wifiConnectResultShown = true;
			WiFi.disconnect(true);

			const char* reason;
			if (status == WL_NO_SSID_AVAIL) reason = L->TXT_WIFI_SSID_NOT_FOUND;
			else if (timedOut)              reason = L->TXT_WIFI_TIMEOUT;
			else                            reason = L->TXT_WIFI_WRONG_PASSWORD;

			String lines[] = { L->TXT_FAILED, String(reason) };
			centeredPrintRows(lines, 2, MEDIUM_TEXT);
			soundError();
		}
	}

	checkExit();
}
