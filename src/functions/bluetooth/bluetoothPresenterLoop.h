// pid PID::BT_PRESENTER

void bluetoothPresenterLoop() {
	static bool isBleConnected = false;

	if (isSetup()) {
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
	}

	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			isBleConnected = true;
			centeredPrint(L->TXT_CONNECTED, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	}

	if (isBtnAWasPressed() && checkTimer(100, true)) {
		bleKeyboard.write(KEY_RIGHT_ARROW);
	}

	if (isBtnPWRWasPressed() && checkTimer(100, true)) {
		bleKeyboard.write(KEY_LEFT_ARROW);
	}

	if (checkExit()) {
		isBleConnected = false;
		centeredPrint(L->TXT_DISCONNECTING, MEDIUM_TEXT);
	}
}
