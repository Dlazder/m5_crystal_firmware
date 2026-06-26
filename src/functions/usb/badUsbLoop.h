// PID::BAD_USB

#ifdef ESP32S3

void badUsbLoop() {
	static bool scriptRunning = false;
	static bool scriptDone = false;
	static bool usbReady = false;

	if (isSetup()) {
		scriptRunning = false;
		scriptDone = false;
		usbReady = false;
		if (selectedFilePath == "") {
			filePickerSetup(PID::OTHER);
		}
	}

	// File picker phase
	if (fpActive) {
		if (filePickerLoop()) return;

		if (selectedFilePath == "") return; // cancelled, changeProcess already called

		if (!badUsbSetScript()) {
			centeredPrint(L->TXT_BT_FILE_ERROR, MEDIUM_TEXT);
			selectedFilePath = "";
			filePickerSetup(PID::OTHER);
			return;
		}

		// Initialize the native USB HID keyboard once and let the host enumerate it.
		if (!usbHidBegan) {
			usbKeyboard.begin();
			USB.begin();
			usbHidBegan = true;
		}
		usbReady = false;
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
		return;
	}

	// Wait for the host to enumerate the device before allowing the script to run.
	if (!usbReady) {
		if (checkTimer(1000, true)) {
			usbReady = true;
			centeredPrint(L->TXT_BT_PRESS_A_TO_RUN, MEDIUM_TEXT);
			soundSuccess();
		}
		if (checkExit()) {
			scriptRunning = false;
			scriptDone = false;
			usbReady = false;
			selectedFilePath = "";
			fpActive = false;
			badUsbFreeLines();
		}
		return;
	}

	// Script execution phase
	if (!scriptDone) {
		if ((!scriptRunning && isBtnAWasPressed()) || (isKbEnterPressed())) {
			badUsbCurrentLine = 0;
			badUsbDelayUntil = 0;
			scriptRunning = true;
			drawScript(badUsbLines, badUsbLineCount, badUsbCurrentLine);
		}
		if (scriptRunning) {
			if (badUsbIsDelaying()) {
				checkExit();
				return;
			}
			if (!badUsbNextLine()) {
				scriptRunning = false;
				scriptDone = true;
				centeredPrint(L->TXT_BT_DONE, MEDIUM_TEXT);
				soundSuccess();
			} else {
				drawScript(badUsbLines, badUsbLineCount, badUsbCurrentLine);
			}
		}
	}

	if (checkExit()) {
		scriptRunning = false;
		scriptDone = false;
		usbReady = false;
		selectedFilePath = "";
		fpActive = false;
		badUsbFreeLines();
	}
}

#else // !ESP32S3 — USB HID unsupported on this board

void badUsbLoop() {
	if (isSetup()) {
		centeredPrint(L->TXT_USB_HID_UNSOPPORTED, MEDIUM_TEXT);
	}
	checkExit();
}

#endif // ESP32S3
