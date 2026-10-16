// PID::RF_CONFIG_MISO — pick the GPIO used for the CC1101 MISO line.

static int rfMisoConfigPinIndex = 0;

// Find the index of the current pin in devicePins; snap to the first pin if absent.
static int _rfMisoPinIndexFor(uint8_t pin) {
	for (int i = 0; i < devicePinsCount; i++) {
		if (devicePins[i] == pin) return i;
	}
	return 0;
}

static void _rfMisoConfigDraw(uint8_t pin) {
	String lines[] = {
		"CC1101 MISO pin",
		"G" + String(pin),
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
	drawHintRange();
}

void rfConfigureMisoLoop() {
	if (isSetup()) {
		rfMisoConfigPinIndex = _rfMisoPinIndexFor(rfMisoPin);
		_rfMisoConfigDraw(rfMisoPin);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbRightPressed()) && checkTimer(100)) {
		rfMisoConfigPinIndex = (rfMisoConfigPinIndex + 1) % devicePinsCount;
		rfMisoPin = devicePins[rfMisoConfigPinIndex];
		setData("rfMisoPin", rfMisoPin);
		Serial.printf("CC1101 MISO pin: GPIO %d\n", rfMisoPin);
		_rfMisoConfigDraw(rfMisoPin);
	}

	if ((isBtnPWRWasPressed() || isKbLeftPressed()) && checkTimer(100)) {
		rfMisoConfigPinIndex = (rfMisoConfigPinIndex <= 0) ? devicePinsCount - 1 : rfMisoConfigPinIndex - 1;
		rfMisoPin = devicePins[rfMisoConfigPinIndex];
		setData("rfMisoPin", rfMisoPin);
		Serial.printf("CC1101 MISO pin: GPIO %d\n", rfMisoPin);
		_rfMisoConfigDraw(rfMisoPin);
	}

	checkExit();
}
