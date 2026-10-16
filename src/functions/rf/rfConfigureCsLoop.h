// PID::RF_CONFIG_CS — pick the GPIO used for the CC1101 CS line.

static int rfCsConfigPinIndex = 0;

// Find the index of the current pin in devicePins; snap to the first pin if absent.
static int _rfCsPinIndexFor(uint8_t pin) {
	for (int i = 0; i < devicePinsCount; i++) {
		if (devicePins[i] == pin) return i;
	}
	return 0;
}

static void _rfCsConfigDraw(uint8_t pin) {
	String lines[] = {
		"CC1101 CS pin",
		"G" + String(pin),
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
	drawHintRange();
}

void rfConfigureCsLoop() {
	if (isSetup()) {
		rfCsConfigPinIndex = _rfCsPinIndexFor(rfCsPin);
		_rfCsConfigDraw(rfCsPin);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbRightPressed()) && checkTimer(100)) {
		rfCsConfigPinIndex = (rfCsConfigPinIndex + 1) % devicePinsCount;
		rfCsPin = devicePins[rfCsConfigPinIndex];
		setData("rfCsPin", rfCsPin);
		Serial.printf("CC1101 CS pin: GPIO %d\n", rfCsPin);
		_rfCsConfigDraw(rfCsPin);
	}

	if ((isBtnPWRWasPressed() || isKbLeftPressed()) && checkTimer(100)) {
		rfCsConfigPinIndex = (rfCsConfigPinIndex <= 0) ? devicePinsCount - 1 : rfCsConfigPinIndex - 1;
		rfCsPin = devicePins[rfCsConfigPinIndex];
		setData("rfCsPin", rfCsPin);
		Serial.printf("CC1101 CS pin: GPIO %d\n", rfCsPin);
		_rfCsConfigDraw(rfCsPin);
	}

	checkExit();
}
