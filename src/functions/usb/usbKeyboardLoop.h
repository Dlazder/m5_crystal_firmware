// PID::USB_KEYBOARD

#ifdef ESP32S3

void usbKeyboardLoop() {
	static bool usbReady = false;

	if (isSetup()) {
		usbReady = false;
		kbReset();
		if (!usbHidBegan) {
			usbKeyboard.begin();
			USB.begin();
			usbHidBegan = true;
		}
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
	}

	if (!usbReady) {
		if (checkTimer(1000, true)) {
			usbReady = true;
			soundSuccess();
			drawKeyboardUi();
		}
		if (checkExit()) kbEnd();
		return;
	}

	if (isKbCursorUpPressed()) usbKeyboard.write(KEY_UP_ARROW);
	if (isKbCursorDownPressed()) usbKeyboard.write(KEY_DOWN_ARROW);
	if (isKbCursorLeftPressed()) usbKeyboard.write(KEY_LEFT_ARROW);
	if (isKbCursorRightPressed()) usbKeyboard.write(KEY_RIGHT_ARROW);

	keyboardLoop(
		[]() {
			kbEnd();
			usbReady = false;
			DISP.clear();
			changeProcess(PID::USB);
		},
		[](const char* buf) {
			usbKeyboard.write(KEY_RETURN);
		},
		[](char ch) {
			// correct HID value
			if (ch == '\b') usbKeyboard.write(BLE_KEY_BACKSPACE);
			else usbKeyboard.write((uint8_t)ch);
		}
	);
}

#else // !ESP32S3 — USB HID unsupported on this board

void usbKeyboardLoop() {
	if (isSetup()) {
		centeredPrint(L->TXT_USB_HID_UNSOPPORTED, MEDIUM_TEXT);
	}
	checkExit();
}

#endif // ESP32S3
