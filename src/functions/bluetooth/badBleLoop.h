// pid PID::BAD_BLE

void badBleLoop() {
	static bool isBleConnected = false;
	static bool scriptRunning = false;
	static bool scriptDone = false;

	if (isSetup()) {
		isBleConnected = false;
		scriptRunning = false;
		scriptDone = false;

		if (lfsSelectedFile == "") {
			lfsOpenPicker(PID::BAD_BLE, PID::BAD_BLE_MENU);
			return;
		}

		if (!badBleLoadFile(lfsSelectedFile)) {
			centeredPrint(L->TXT_BT_FILE_ERROR, SMALL_TEXT);
			lfsSelectedFile = "";
			return;
		}
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		centeredPrint(L->TXT_WAITING_CONNECTION, SMALL_TEXT);
		updateTimer();
	}

	// BLE phase
	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			isBleConnected = true;
			centeredPrint(L->TXT_BT_PRESS_A_TO_RUN, SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			scriptRunning = false;
			centeredPrint(L->TXT_DISCONNECTED, SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	}

	// Script phase
	if (isBleConnected && !scriptDone) {
		if (!scriptRunning && isBtnAWasPressed()) {
			badBleLoadFile(lfsSelectedFile);
			scriptRunning = true;
			centeredPrint(L->TXT_BT_RUNNING, SMALL_TEXT);
		}
		if (scriptRunning) {
			if (badBleIsDelaying()) {
				checkExit();
				return;
			}
			if (!badBleNextLine()) {
				scriptRunning = false;
				scriptDone = true;
				centeredPrint(L->TXT_BT_DONE, SMALL_TEXT);
				DEVICE.Speaker.tone(3000, 300);
			}
		}
	}

	if (checkExit()) {
		isBleConnected = false;
		scriptRunning = false;
		scriptDone = false;
		lfsSelectedFile = "";
	}
}
