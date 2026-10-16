// PID::RF_CONFIG_MOSI — pick the GPIO used for the CC1101 MOSI line.

static int rfMosiConfigPinIndex = 0;

// Find the index of the current pin in devicePins; snap to the first pin if absent.
static int _rfMosiPinIndexFor(uint8_t pin) {
	for (int i = 0; i < devicePinsCount; i++) {
		if (devicePins[i] == pin) return i;
	}
	return 0;
}

static void _rfMosiConfigDraw(uint8_t pin) {
	String lines[] = {
		"CC1101 MOSI pin",
		"G" + String(pin),
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
	drawHintRange();
}

void rfConfigureMosiLoop() {
	if (isSetup()) {
		rfMosiConfigPinIndex = _rfMosiPinIndexFor(rfMosiPin);
		_rfMosiConfigDraw(rfMosiPin);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbRightPressed()) && checkTimer(100)) {
		rfMosiConfigPinIndex = (rfMosiConfigPinIndex + 1) % devicePinsCount;
		rfMosiPin = devicePins[rfMosiConfigPinIndex];
		setData("rfMosiPin", rfMosiPin);
		Serial.printf("CC1101 MOSI pin: GPIO %d\n", rfMosiPin);
		_rfMosiConfigDraw(rfMosiPin);
	}

	if ((isBtnPWRWasPressed() || isKbLeftPressed()) && checkTimer(100)) {
		rfMosiConfigPinIndex = (rfMosiConfigPinIndex <= 0) ? devicePinsCount - 1 : rfMosiConfigPinIndex - 1;
		rfMosiPin = devicePins[rfMosiConfigPinIndex];
		setData("rfMosiPin", rfMosiPin);
		Serial.printf("CC1101 MOSI pin: GPIO %d\n", rfMosiPin);
		_rfMosiConfigDraw(rfMosiPin);
	}

	checkExit();
}
