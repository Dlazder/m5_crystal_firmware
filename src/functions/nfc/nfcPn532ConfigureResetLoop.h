// PID::NFC_PN532_RESET

void nfcPn532ConfigureResetLoop() {
	if (isSetup()) {
		String lines[] = {
			"Reset PN532 pins?",
			"SDA" + String(pn532SdaPin) + " SCL" + String(pn532SclPin),
		};
		centeredPrintRows(lines, 2, SMALL_TEXT, true);
		drawHintCustom("enter: reset", "A: reset");
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		Settings::resetGroup(SettingGroup::PN532_PINS);

		String lines[] = {
			"PN532 pins reset",
			"SDA" + String(pn532SdaPin) + " SCL" + String(pn532SclPin),
		};
		Serial.printf("PN532 pins reset: %s\n", lines[1].c_str());
		centeredPrintRows(lines, 2, SMALL_TEXT);
		soundSuccess();
	}

	checkExit();
}
