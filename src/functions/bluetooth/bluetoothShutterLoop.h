// pid PID::BT_SHUTTER

void bluetoothShutterLoop() {
	if (isSetup()) bleConnect();

	bleHandleConnection(
		[]() { centeredPrint(L->TXT_CONNECTED, MEDIUM_TEXT); soundSuccess(); },
		[]() { centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT); soundError(); }
	);

	if (isBtnAWasPressed() && checkTimer(100, true)) {
		bleKeyboard.write(KEY_RETURN);
	}

	if (checkExit()) {
		bleConnected = false;
		centeredPrint(L->TXT_DISCONNECTING, MEDIUM_TEXT);
	}
}
