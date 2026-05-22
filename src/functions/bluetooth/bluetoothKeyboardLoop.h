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
			soundSuccess();
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT);
			soundError();
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
			if (ch == '\b') bleKeyboard.write(BLE_KEY_BACKSPACE);
			else bleKeyboard.write((uint8_t)ch);
		}
	);
}
