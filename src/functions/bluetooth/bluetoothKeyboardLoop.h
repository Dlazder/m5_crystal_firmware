// pid PID::BT_KEYBOARD

void bluetoothKeyboardLoop() {
	static bool isBleConnected = false;

	if (isSetup()) {
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		kbReset();
		isBleConnected = false;
		centeredPrint("Waiting connection", SMALL_TEXT);
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
			centeredPrint("Not connected", SMALL_TEXT);
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
			if (ch == '\b') bleKeyboard.write(KEY_BACKSPACE);
			else bleKeyboard.write((uint8_t)ch);
		}
	);
}
