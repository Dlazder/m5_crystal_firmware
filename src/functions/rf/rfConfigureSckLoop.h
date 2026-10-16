// PID::RF_CONFIG_SCK — pick the GPIO used for the CC1101 SCK line.

static int rfSckConfigPinIndex = 0;

// Find the index of the current pin in devicePins; snap to the first pin if absent.
static int _rfSckPinIndexFor(uint8_t pin) {
	for (int i = 0; i < devicePinsCount; i++) {
		if (devicePins[i] == pin) return i;
	}
	return 0;
}

static void _rfSckConfigDraw(uint8_t pin) {
	String lines[] = {
		"CC1101 SCK pin",
		"G" + String(pin),
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
	drawHintRange();
}

void rfConfigureSckLoop() {
	if (isSetup()) {
		rfSckConfigPinIndex = _rfSckPinIndexFor(rfSckPin);
		_rfSckConfigDraw(rfSckPin);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbRightPressed()) && checkTimer(100)) {
		rfSckConfigPinIndex = (rfSckConfigPinIndex + 1) % devicePinsCount;
		rfSckPin = devicePins[rfSckConfigPinIndex];
		setData("rfSckPin", rfSckPin);
		Serial.printf("CC1101 SCK pin: GPIO %d\n", rfSckPin);
		_rfSckConfigDraw(rfSckPin);
	}

	if ((isBtnPWRWasPressed() || isKbLeftPressed()) && checkTimer(100)) {
		rfSckConfigPinIndex = (rfSckConfigPinIndex <= 0) ? devicePinsCount - 1 : rfSckConfigPinIndex - 1;
		rfSckPin = devicePins[rfSckConfigPinIndex];
		setData("rfSckPin", rfSckPin);
		Serial.printf("CC1101 SCK pin: GPIO %d\n", rfSckPin);
		_rfSckConfigDraw(rfSckPin);
	}

	checkExit();
}
