// pid PID::BAD_BLE

void badBleLoop() {
	static bool scriptRunning = false;
	static bool scriptDone = false;

	if (isSetup()) {
		bleConnected = false;
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
		bleConnect();
		return;
	}

	// BLE connection phase
	DEVICE.update();
	bleHandleConnection(
		[]() { centeredPrint(L->TXT_BT_PRESS_A_TO_RUN, MEDIUM_TEXT); soundSuccess(); },
		[]() { scriptRunning = false; centeredPrint(L->TXT_DISCONNECTED, MEDIUM_TEXT); soundError(); }
	);

	// Script execution phase
	if (bleConnected && !scriptDone) {
		if (!scriptRunning && isBtnAWasPressed() || (isKbEnterPressed())) {
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
				soundSuccess();
			}
		}
	}

	if (checkExit()) {
		bleConnected = false;
		scriptRunning	= false;
		scriptDone = false;
		selectedFilePath = "";
		fpActive = false;
	}
}
