// pid PID::SETTINGS_SSID

static bool ssidEditDone = false;

void setSsidLoop() {
	if (isSetup()) {
		ssidEditDone = false;
		kbReset();
		drawKeyboardUi();
		updateTimer();
	}

	if (!ssidEditDone) {
		keyboardLoop(
			[]() {
				// onExit — discard changes, keep previous SSID
				ssidEditDone = true;
				kbEnd();
				DISP.clear();
				changeProcess(PID::SETTINGS);
			},
			[](const char* buf) {
				// onEnter — save if non-empty
				ssidEditDone = true;
				kbEnd();
				if (strlen(buf) > 0) {
					wifiSsid = String(buf);
					setDataString("wifiSsid", buf);
				}
				DISP.clear();
				String lines[] = { L->TXT_SETTINGS_WIFI_SSID, wifiSsid };
				centeredPrintRows(lines, 2, MEDIUM_TEXT);
				soundSuccess();
			},
			nullptr
		);
		return;
	}

	checkExit();
}
