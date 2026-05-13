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
			if (previousProcess == PID::BAD_BLE_MENU && cursor == 2) {
				// SD card selected
				selectedFileSourcePid = PID::FILE_PICKER_SD;
				changeProcess(PID::FILE_PICKER_SD);
			} else {
				// LittleFS selected (cursor == 1) or default
				lfsReturnPid = PID::BAD_BLE;
				lfsCancelPid = PID::BAD_BLE_MENU;
				changeProcess(PID::FILE_PICKER);
			}
			return;
		}

		if (!badBleLoadFile(selectedFilePath)) {
			centeredPrint(L->TXT_BT_FILE_ERROR, MEDIUM_TEXT);
			selectedFilePath = "";
			return;
		}
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
	}

	// BLE phase
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

	// Script phase
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
		scriptRunning = false;
		scriptDone = false;
		selectedFilePath = "";
	}
}
