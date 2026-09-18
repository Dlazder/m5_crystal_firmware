bool bleConnected = false;

void bleConnect() {
	if (!bleCompositeBegan) {
		// Free the SD card before BLE init: Bluedroid needs a large contiguous
		// block of internal RAM, and the SD driver + script (already loaded into
		// badUsbLines) are no longer needed once the file has been read. On the
		// Cardputer ADV (no PSRAM) this is the difference between a successful
		// BLE init and xSemaphoreCreateBinary() returning NULL (heap exhaustion).
		Storage::unmountSD();
		bleKeyboard.begin();
		bleCompositeBegan = true;
	}
	bleConnected = false;
	centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
	updateTimer();
}

void bleHandleConnection(void (*onConnect)(), void (*onDisconnect)()) {
	if (bleKeyboard.isConnected()) {
		if (!bleConnected) {
			bleConnected = true;
			onConnect();
		}
	} else {
		if (bleConnected) {
			bleConnected = false;
			onDisconnect();
		}
	}
}