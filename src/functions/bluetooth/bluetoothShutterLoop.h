// PID::BT_SHUTTER

void bluetoothShutterLoop() {
	if (isSetup()) Ble::begin();

	Ble::handleConnection(
		[]() { centeredPrint(L->TXT_CONNECTED, MEDIUM_TEXT); soundSuccess(); },
		[]() { centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT); soundError(); }
	);

	if (isBtnAWasPressed() && checkTimer(100, true)) {
		Ble::keyboard.write(KEY_RETURN);
	}

	if (checkExit()) {
		Ble::connected = false;
		centeredPrint(L->TXT_DISCONNECTING, MEDIUM_TEXT);
	}
}
