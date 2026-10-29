// PID::RF_CONFIG_RESET

static String _rfPinsSummary() {
	return "CS" + String(rfCsPin) + " SCK" + String(rfSckPin) + " MOSI" + String(rfMosiPin) +
				" MISO" + String(rfMisoPin) + " G0:" + String(rfGdo0Pin);
}

void rfConfigureResetLoop() {
	if (isSetup()) {
		String lines[] = {
			"Reset RF pins?",
			_rfPinsSummary(),
		};
		centeredPrintRows(lines, 2, SMALL_TEXT, true);
		drawHintCustom("enter: reset", "A: reset");
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		Settings::resetGroup(SettingGroup::RF_PINS);
		Serial.printf("CC1101 pins reset: %s\n", _rfPinsSummary().c_str());

		String lines[] = {
			"RF pins reset",
			_rfPinsSummary(),
		};
		centeredPrintRows(lines, 2, SMALL_TEXT);
		soundSuccess();
	}

	checkExit();
}
