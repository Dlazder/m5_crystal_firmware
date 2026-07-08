// PID::WEBSERVER_FS

void toggleWebServerFs() {
	if (isSetup()) {
		centeredPrint(String(L->TXT_SETTINGS_WEBSERVER_FS) + String(getData("webServerFs", webServerFs) ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		webServerFs = !getData("webServerFs", webServerFs);
		setData("webServerFs", webServerFs);

		centeredPrint(String(L->TXT_SETTINGS_WEBSERVER_FS) + String(getData("webServerFs", webServerFs) ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
	}

	checkExit();
}
