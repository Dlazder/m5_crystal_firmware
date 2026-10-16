// PID::RF_CONFIG_GDO0 — pick the GPIO used for the CC1101 GDO0 line.

static int rfGdo0ConfigPinIndex = 0;

// Find the index of the current pin in devicePins; snap to the first pin if absent.
static int _rfGdo0PinIndexFor(uint8_t pin) {
	for (int i = 0; i < devicePinsCount; i++) {
		if (devicePins[i] == pin) return i;
	}
	return 0;
}

static void _rfGdo0ConfigDraw(uint8_t pin) {
	String lines[] = {
		"CC1101 GDO0 pin",
		"G" + String(pin),
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
	drawHintRange();
}

void rfConfigureGdo0Loop() {
	if (isSetup()) {
		rfGdo0ConfigPinIndex = _rfGdo0PinIndexFor(rfGdo0Pin);
		_rfGdo0ConfigDraw(rfGdo0Pin);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbRightPressed()) && checkTimer(100)) {
		rfGdo0ConfigPinIndex = (rfGdo0ConfigPinIndex + 1) % devicePinsCount;
		rfGdo0Pin = devicePins[rfGdo0ConfigPinIndex];
		setData("rfGdo0Pin", rfGdo0Pin);
		Serial.printf("CC1101 GDO0 pin: GPIO %d\n", rfGdo0Pin);
		_rfGdo0ConfigDraw(rfGdo0Pin);
	}

	if ((isBtnPWRWasPressed() || isKbLeftPressed()) && checkTimer(100)) {
		rfGdo0ConfigPinIndex = (rfGdo0ConfigPinIndex <= 0) ? devicePinsCount - 1 : rfGdo0ConfigPinIndex - 1;
		rfGdo0Pin = devicePins[rfGdo0ConfigPinIndex];
		setData("rfGdo0Pin", rfGdo0Pin);
		Serial.printf("CC1101 GDO0 pin: GPIO %d\n", rfGdo0Pin);
		_rfGdo0ConfigDraw(rfGdo0Pin);
	}

	checkExit();
}
