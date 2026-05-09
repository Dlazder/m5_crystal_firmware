// PID::WIFI_DISCONNECT

void wifiDisconnectLoop() {
	if (isSetup()) {
		updateTimer();

		if (!WiFi.isConnected()) {
			centeredPrint(L->TXT_DISCONNECTED, MEDIUM_TEXT);
		} else {
			String lines[] = {
				L->TXT_CONNECTED,
				L->TXT_PRESS_A,
				"To disconnect"
			};
			centeredPrintRows(lines, 3, MEDIUM_TEXT);
		}
	}

	if (WiFi.isConnected() && (isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		WiFi.disconnect(true);
		centeredPrint(L->TXT_DISCONNECTED, MEDIUM_TEXT);
	}

	checkExit();
}
