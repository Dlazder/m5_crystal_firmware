// PID::NRF_CONFIG_RESET

static String _nrfPinsRow1() {
	return "CSN" + String(NrfPins::csn) + " SCK" + String(NrfPins::sck) + " MOSI" + String(NrfPins::mosi);
}

static String _nrfPinsRow2() {
	return "MISO" + String(NrfPins::miso) + " CE" + String(NrfPins::ce);
}

void nrfConfigureResetLoop() {
	if (isSetup()) {
		String lines[] = {
			"Reset NRF pins?",
			_nrfPinsRow1(),
			_nrfPinsRow2(),
		};
		centeredPrintRows(lines, 3, SMALL_TEXT, true);
		drawHintCustom("enter: reset", "A: reset");
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		Settings::resetGroup(SettingGroup::NRF_PINS);
		Serial.printf("NRF pins reset: %s %s\n", _nrfPinsRow1().c_str(), _nrfPinsRow2().c_str());

		String lines[] = {
			"NRF pins reset",
			_nrfPinsRow1(),
			_nrfPinsRow2(),
		};
		centeredPrintRows(lines, 3, SMALL_TEXT);
		soundSuccess();
	}

	checkExit();
}
