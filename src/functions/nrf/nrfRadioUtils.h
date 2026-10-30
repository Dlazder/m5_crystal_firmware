// Shared open/close helpers for the nRF24 screens.
//
// The module pins can overlap the GPIO UART (Serial2: G1 TX / G2 RX on StickS3).
// Toggling CSN or SCK on a pin that Serial2 listens to floods it with garbage and
// stalls loop() in handleSerialCommands(), so Serial2 is detached while the radio
// owns those pins and restored on exit.

static bool nrfSerial2Detached = false;

static bool _nrfPinsOverlapUart() {
	const uint8_t nrf[] = { nrfCsnPin, nrfSckPin, nrfMosiPin, nrfMisoPin, nrfCePin };
	for (uint8_t p : nrf) {
		if (p == uartRxPin || p == uartTxPin) return true;
	}
	return false;
}

bool nrfRadioOpen() {
	if (_nrfPinsOverlapUart()) {
		Serial2.end();
		nrfSerial2Detached = true;
	}
	return NRF24::begin(nrfCsnPin, nrfSckPin, nrfMosiPin, nrfMisoPin, nrfCePin);
}

void nrfRadioClose() {
	NRF24::end();
	if (nrfSerial2Detached) {
		Serial2.begin(uartBaud, SERIAL_8N1, uartRxPin, uartTxPin);
		Serial2.setTimeout(50);
		nrfSerial2Detached = false;
	}
}
