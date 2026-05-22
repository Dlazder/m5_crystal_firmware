// PID::VOLUME

void volumeLoop() {
	if (isSetup()) {
		String lines[] = {
			String(L->TXT_SETTINGS_VOLUME) + String(volume / volumeDividor) + "/10",
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		volume += volumeDividor;
		if (volume > volumeMax) volume = volumeMin;
		DEVICE.Speaker.setVolume(volume);
		soundBeep();
		setData("volume", volume);

		Serial.printf("Volume: %d\n", volume / volumeDividor);

		String lines[] = {
			String(L->TXT_SETTINGS_VOLUME) + String(volume / volumeDividor) + "/10",
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
	}
	checkExit();
}
