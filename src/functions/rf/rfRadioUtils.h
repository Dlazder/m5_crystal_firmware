// Shared open/close helpers for the CC1101 screens.
//
// The radio pins can overlap the GPIO UART (Serial2: G1 TX / G2 RX on StickS3).
// Toggling CS or SCK on a pin that Serial2 listens to floods it with garbage and
// stalls loop() in handleSerialCommands(), so Serial2 is detached while the radio
// owns those pins and restored on exit.

static bool rfSerial2Detached = false;

static bool _rfPinsOverlapUart() {
	const uint8_t rf[] = { rfCsPin, rfSckPin, rfMosiPin, rfMisoPin, rfGdo0Pin };
	for (uint8_t p : rf) {
		if (p == uartRxPin || p == uartTxPin) return true;
	}
	return false;
}

bool rfRadioOpen() {
	if (_rfPinsOverlapUart()) {
		Serial2.end();
		rfSerial2Detached = true;
	}
	return CC1101::begin(rfCsPin, rfSckPin, rfMosiPin, rfMisoPin, rfGdo0Pin);
}

void rfRadioClose() {
	CC1101::end();
	if (rfSerial2Detached) {
		Serial2.begin(uartBaud, SERIAL_8N1, uartRxPin, uartTxPin);
		Serial2.setTimeout(50);
		rfSerial2Detached = false;
	}
}
