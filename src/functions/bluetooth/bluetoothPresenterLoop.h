// PID::BT_PRESENTER

void bluetoothPresenterLoop() {
	if (isSetup()) Ble::begin();

	Ble::handleConnection(
		[]() { centeredPrint(L->TXT_CONNECTED, MEDIUM_TEXT); soundSuccess(); },
		[]() { centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT); soundError(); }
	);

	if (isBtnAWasPressed() && checkTimer(100, true)) {
		Ble::keyboard.write(KEY_RIGHT_ARROW);
	}

	if (isBtnPWRWasPressed() && checkTimer(100, true)) {
		Ble::keyboard.write(KEY_LEFT_ARROW);
	}

	if (checkExit()) {
		Ble::connected = false;
		centeredPrint(L->TXT_DISCONNECTING, MEDIUM_TEXT);
	}
}
