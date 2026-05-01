// pid 43

void badBleLoop() {
	static bool isBleConnected = false;
	static bool scriptRunning = false;
	static bool scriptDone = false;

	if (isSetup()) {
		isBleConnected = false;
		scriptRunning = false;
		scriptDone = false;
		badBleSelectedFile = "";
		lfsSetup(badBleFileMenu, badBleFileCount, 42, 43);
	}

	// File browser phase
	if (badBleSelectedFile == "") {
		badBleSelectedFile = lfsLoop(badBleFileMenu, badBleFileCount, 42);
		if (badBleSelectedFile != "") {
			if (!badBleLoadFile(badBleSelectedFile)) {
				centeredPrint("File error", SMALL_TEXT);
				badBleSelectedFile = "";
				return;
			}
			if (!bleCompositeBegan) {
				bleKeyboard.begin();
				bleCompositeBegan = true;
			}
			centeredPrint("Waiting connection", SMALL_TEXT);
			updateTimer();
		}
		return;
	}

	// BLE phase
	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			isBleConnected = true;
			centeredPrint("Press A to run", SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			scriptRunning = false;
			centeredPrint("Disconnected", SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	}

	// Script phase
	if (isBleConnected && !scriptDone) {

		if (!scriptRunning && isBtnAWasPressed()) {
			badBleLoadFile(badBleSelectedFile);
			scriptRunning = true;
			centeredPrint("Running...", SMALL_TEXT);
		}
		
		if (scriptRunning) {
			if (badBleIsDelaying()) return;
			if (!badBleNextLine()) {
				scriptRunning = false;
				scriptDone = true;
				centeredPrint("Done!", SMALL_TEXT);
				DEVICE.Speaker.tone(3000, 300);
			}
		}
	}

	if (checkExit()) {
		isBleConnected = false;
		scriptRunning = false;
		scriptDone = false;
		badBleSelectedFile = "";
		lfsClearMenu(badBleFileMenu, badBleFileCount);
	}
}