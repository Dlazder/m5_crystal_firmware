// Shared open/close helpers for the CC1101 screens.
//
// The radio pins can overlap the GPIO UART (Serial2: G1 TX / G2 RX on StickS3).
// Toggling CS or SCK on a pin that Serial2 listens to floods it with garbage and
// stalls loop() in handleSerialCommands(), so Serial2 is detached while the radio
// owns those pins and restored on exit.

static bool rfSerial2Detached = false;

static bool _rfPinsOverlapUart() {
	const uint8_t rf[] = { RfPins::cs, RfPins::sck, RfPins::mosi, RfPins::miso, RfPins::gdo0 };
	for (uint8_t p : rf) {
		if (p == UartCfg::rx || p == UartCfg::tx) return true;
	}
	return false;
}

bool rfRadioOpen() {
	if (_rfPinsOverlapUart()) {
		Serial2.end();
		rfSerial2Detached = true;
	}
	return CC1101::begin(RfPins::cs, RfPins::sck, RfPins::mosi, RfPins::miso, RfPins::gdo0);
}

void rfRadioClose() {
	CC1101::end();
	if (rfSerial2Detached) {
		Serial2.begin(UartCfg::baud, SERIAL_8N1, UartCfg::rx, UartCfg::tx);
		Serial2.setTimeout(50);
		rfSerial2Detached = false;
	}
}
