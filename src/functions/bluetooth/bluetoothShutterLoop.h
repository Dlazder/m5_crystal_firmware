// pid PID::BT_SHUTTER

void bluetoothShutterLoop() {
	static bool isBleConnected = false;

	if (isSetup()) {
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		centeredPrint("Waiting connection", SMALL_TEXT);
		updateTimer();
	}

	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			centeredPrint("Connected", SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
			isBleConnected = true;
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			centeredPrint("Not connected", SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	}

	if (isBtnAWasPressed() && checkTimer(100, true)) {
		bleKeyboard.write(KEY_RETURN);
	}

	if (checkExit()) {
		isBleConnected = false;
		centeredPrint("Disconnecting...", SMALL_TEXT);
	}
}
