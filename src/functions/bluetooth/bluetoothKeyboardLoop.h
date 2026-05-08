// PID::BT_KEYBOARD

void bluetoothKeyboardLoop() {
	static bool isBleConnected = false;

	if (isSetup()) {
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		kbReset();
		isBleConnected = false;
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
	}

	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			isBleConnected = true;
			drawKeyboardUi();
			DEVICE.Speaker.tone(2000, 200);
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
		checkExit();
		return;
	}

	if (!isBleConnected) checkExit();

	keyboardLoop(
		[]() {
			DISP.clear();
			changeProcess(PID::BLUETOOTH);
		},
		[](const char* buf) {
			bleKeyboard.write(KEY_RETURN);
		},
		[](char ch) {
			#ifdef CARDPUTER
				if (ch == '\b') bleKeyboard.write(0xB2); // KEY_BACKSPACE from BleCombo, overridden by M5Cardputer header
			#else
				if (ch == '\b') bleKeyboard.write(KEY_BACKSPACE);
			#endif
			else bleKeyboard.write((uint8_t)ch);
		}
	);
}
