// PID::NFC_RC522_RESET

void nfcRc522ConfigureResetLoop() {
	if (isSetup()) {
		String lines[] = {
			"Reset RC522 pins?",
			"SS" + String(rc522SsPin) + " SCK" + String(rc522SckPin) + " MOSI" + String(rc522MosiPin),
			"MISO" + String(rc522MisoPin) + " RST" + String(rc522RstPin),
		};
		centeredPrintRows(lines, 3, SMALL_TEXT, true);
		drawHintCustom("enter: reset", "A: reset");
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		Settings::resetGroup(SettingGroup::RC522_PINS);

		String lines[] = {
			"RC522 pins reset",
			"SS" + String(rc522SsPin) + " SCK" + String(rc522SckPin) + " MOSI" + String(rc522MosiPin),
			"MISO" + String(rc522MisoPin) + " RST" + String(rc522RstPin),
		};
		Serial.printf("RC522 pins reset: %s %s\n", lines[1].c_str(), lines[2].c_str());
		centeredPrintRows(lines, 3, SMALL_TEXT);
		soundSuccess();
	}

	checkExit();
}
