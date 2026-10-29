// PID::BT_KEYBOARD

void bluetoothKeyboardLoop() {
	if (isSetup()) {
		kbReset();
		Ble::begin();
	}

	Ble::handleConnection(
		[]() { drawKeyboardUi(); soundSuccess(); },
		[]() { centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT); soundError(); }
	);

	if (!Ble::connected) {
		if (checkExit()) kbEnd();
		return;
	}

	if (isKbCursorUpPressed()) Ble::keyboard.write(KEY_UP_ARROW);
	if (isKbCursorDownPressed()) Ble::keyboard.write(KEY_DOWN_ARROW);
	if (isKbCursorLeftPressed()) Ble::keyboard.write(KEY_LEFT_ARROW);
	if (isKbCursorRightPressed()) Ble::keyboard.write(KEY_RIGHT_ARROW);

	keyboardLoop(
		[]() {
			kbEnd();
			DISP.clear();
			changeProcess(PID::BLUETOOTH);
		},
		[](const char* buf) {
			Ble::keyboard.write(KEY_RETURN);
		},
		[](char ch) {
			if (ch == '\b') Ble::keyboard.write(BLE_KEY_BACKSPACE);
			else Ble::keyboard.write((uint8_t)ch);
		}
	);
}
