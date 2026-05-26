// PID::BT_KEYBOARD

void bluetoothKeyboardLoop() {
	if (isSetup()) {
		kbReset();
		bleConnect();
	}

	bleHandleConnection(
		[]() { drawKeyboardUi(); soundSuccess(); },
		[]() { centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT); soundError(); }
	);

	if (!bleConnected) { checkExit(); return; }

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
