// pid PID::BAD_BLE

void badBleLoop() {
	static bool isBleConnected = false;
	static bool scriptRunning = false;
	static bool scriptDone = false;

	if (isSetup()) {
		isBleConnected = false;
		scriptRunning = false;
		scriptDone = false;
		if (selectedFilePath == "") {
			filePickerSetup(PID::BLUETOOTH);
		}
	}

	// File picker phase
	if (fpActive) {
		if (filePickerLoop()) return;

		if (selectedFilePath == "") return; // cancelled, changeProcess already called

		if (!badBleLoadFile(selectedFilePath)) {
			centeredPrint(L->TXT_BT_FILE_ERROR, MEDIUM_TEXT);
			selectedFilePath = "";
			filePickerSetup(PID::BLUETOOTH);
			return;
		}
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
		return;
	}

	// BLE connection phase
	DEVICE.update();
	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			isBleConnected = true;
			centeredPrint(L->TXT_BT_PRESS_A_TO_RUN, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			scriptRunning = false;
			centeredPrint(L->TXT_DISCONNECTED, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
	}

	// Script execution phase
	if (isBleConnected && !scriptDone) {
		if (!scriptRunning && isBtnAWasPressed()) {
			badBleLoadFile(selectedFilePath);
			scriptRunning = true;
			centeredPrint(L->TXT_BT_RUNNING, MEDIUM_TEXT);
		}
		if (scriptRunning) {
			if (badBleIsDelaying()) {
				checkExit();
				return;
			}
			if (!badBleNextLine()) {
				scriptRunning = false;
				scriptDone = true;
				centeredPrint(L->TXT_BT_DONE, MEDIUM_TEXT);
				DEVICE.Speaker.tone(3000, 300);
			}
		}
	}

	if (checkExit()) {
		isBleConnected = false;
		scriptRunning	= false;
		scriptDone = false;
		selectedFilePath = "";
		fpActive = false;
	}
}
