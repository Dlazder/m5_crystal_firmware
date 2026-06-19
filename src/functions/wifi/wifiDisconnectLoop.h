// PID::WIFI_DISCONNECT

void wifiDisconnectLoop() {
	if (isSetup()) {
		updateTimer();

		if (!WiFi.isConnected()) {
			centeredPrint(L->TXT_DISCONNECTED, MEDIUM_TEXT);
		} else {
			String lines[] = {
				L->TXT_CONNECTED,
			};
			centeredPrintRows(lines, 1, MEDIUM_TEXT, true);
			drawHintCustom("enter: disconnect", "A: disconnect");
		}
	}

	if (WiFi.isConnected() && (isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		WiFi.disconnect(true);
		centeredPrint(L->TXT_DISCONNECTED, MEDIUM_TEXT);
	}

	checkExit();
}
