void bleConnect() {
	if (!bleCompositeBegan) {
		bleKeyboard.begin();
		bleCompositeBegan = true;
	}
	centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
	updateTimer();
}

void bleHandleConnection(bool& connected, std::function<void()> onConnect, std::function<void()> onDisconnect) {
	if (bleKeyboard.isConnected()) {
		if (!connected) {
			connected = true;
			onConnect();
		}
	} else {
		if (connected) {
			connected = false;
			onDisconnect();
		}
	}
}