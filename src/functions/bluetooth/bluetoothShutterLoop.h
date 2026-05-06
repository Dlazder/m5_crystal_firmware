// pid PID::BT_SHUTTER

void bluetoothShutterLoop() {
	static bool isBleConnected = false;

	if (isSetup()) {
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		centeredPrint(L->TXT_WAITING_CONNECTION, SMALL_TEXT);
		updateTimer();
	}

	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			centeredPrint(L->TXT_CONNECTED, SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
			isBleConnected = true;
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			centeredPrint(L->TXT_NOT_CONNECTED, SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	}

	if (isBtnAWasPressed() && checkTimer(100, true)) {
		bleKeyboard.write(KEY_RETURN);
	}

	if (checkExit()) {
		isBleConnected = false;
		centeredPrint(L->TXT_DISCONNECTING, SMALL_TEXT);
	}
}
