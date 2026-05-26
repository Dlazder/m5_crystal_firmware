bool bleConnected = false;

void bleConnect() {
	if (!bleCompositeBegan) {
		bleKeyboard.begin();
		bleCompositeBegan = true;
	}
	bleConnected = false;
	centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
	updateTimer();
}

void bleHandleConnection(void (*onConnect)(), void (*onDisconnect)()) {
	if (bleKeyboard.isConnected()) {
		if (!bleConnected) {
			bleConnected = true;
			onConnect();
		}
	} else {
		if (bleConnected) {
			bleConnected = false;
			onDisconnect();
		}
	}
}