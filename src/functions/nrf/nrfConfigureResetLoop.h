// PID::NRF_CONFIG_RESET

static String _nrfPinsRow1() {
	return "CSN" + String(nrfCsnPin) + " SCK" + String(nrfSckPin) + " MOSI" + String(nrfMosiPin);
}

static String _nrfPinsRow2() {
	return "MISO" + String(nrfMisoPin) + " CE" + String(nrfCePin);
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
