// pid PID::BT_PRESENTER

void bluetoothPresenterLoop() {
	static bool isBleConnected = false;

	if (isSetup()) bleConnect();

	bleHandleConnection(isBleConnected,
		[]() { centeredPrint(L->TXT_CONNECTED, MEDIUM_TEXT); soundSuccess(); },
		[]() { centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT); soundError(); }
	);

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
