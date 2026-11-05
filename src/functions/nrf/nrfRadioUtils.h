// Shared open/close helpers for the nRF24 screens.
//
// The module pins can overlap the GPIO UART (Serial2: G1 TX / G2 RX on StickS3).
// Toggling CSN or SCK on a pin that Serial2 listens to floods it with garbage and
// stalls loop() in handleSerialCommands(), so Serial2 is detached while the radio
// owns those pins and restored on exit.

static bool nrfSerial2Detached = false;

static bool _nrfPinsOverlapUart() {
	const uint8_t nrf[] = { NrfPins::csn, NrfPins::sck, NrfPins::mosi, NrfPins::miso, NrfPins::ce };
	for (uint8_t p : nrf) {
		if (p == UartCfg::rx || p == UartCfg::tx) return true;
	}
	return false;
}

bool nrfRadioOpen() {
	if (_nrfPinsOverlapUart()) {
		Serial2.end();
		nrfSerial2Detached = true;
	}
	return NRF24::begin(NrfPins::csn, NrfPins::sck, NrfPins::mosi, NrfPins::miso, NrfPins::ce);
}

void nrfRadioClose() {
	NRF24::end();
	if (nrfSerial2Detached) {
		Serial2.begin(UartCfg::baud, SERIAL_8N1, UartCfg::rx, UartCfg::tx);
		Serial2.setTimeout(50);
		nrfSerial2Detached = false;
	}
}
