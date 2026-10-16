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
		rfCsPin = CC1101_CS_PIN;
		rfSckPin = CC1101_SCK_PIN;
		rfMosiPin = CC1101_MOSI_PIN;
		rfMisoPin = CC1101_MISO_PIN;
		rfGdo0Pin = CC1101_GDO0_PIN;
		setData("rfCsPin", rfCsPin);
		setData("rfSckPin", rfSckPin);
		setData("rfMosiPin", rfMosiPin);
		setData("rfMisoPin", rfMisoPin);
		setData("rfGdo0Pin", rfGdo0Pin);
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
