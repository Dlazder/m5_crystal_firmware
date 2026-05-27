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

	if (isKbCursorUpPressed()) bleKeyboard.write(KEY_UP_ARROW);
	if (isKbCursorDownPressed()) bleKeyboard.write(KEY_DOWN_ARROW);
	if (isKbCursorLeftPressed()) bleKeyboard.write(KEY_LEFT_ARROW);
	if (isKbCursorRightPressed()) bleKeyboard.write(KEY_RIGHT_ARROW);

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
