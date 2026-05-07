// pid PID::BT_SHUTTER

void bluetoothShutterLoop() {
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
			centeredPrint(L->TXT_CONNECTED, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
			isBleConnected = true;
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	}

	if (isBtnAWasPressed() && checkTimer(100, true)) {
		bleKeyboard.write(KEY_RETURN);
	}

	if (checkExit()) {
		isBleConnected = false;
		centeredPrint(L->TXT_DISCONNECTING, MEDIUM_TEXT);
	}
}
