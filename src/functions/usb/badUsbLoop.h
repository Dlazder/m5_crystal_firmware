// PID::BAD_USB

#ifdef ESP32S3

static void usbSinkWrite(uint8_t c) { usbKeyboard.write(c); }
static void usbSinkPress(uint8_t c) { usbKeyboard.press(c); }
static void usbSinkReleaseAll() { usbKeyboard.releaseAll(); }
static const Duckyscript::Sink usbSink = { usbSinkWrite, usbSinkPress, usbSinkReleaseAll };

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

		String* lines = nullptr;
		int count = 0;
		if (!Storage::readLines(selectedFilePath, lines, count, !fpSelectedSd)) {
			centeredPrint(L->TXT_BT_FILE_ERROR, MEDIUM_TEXT);
			selectedFilePath = "";
			filePickerSetup(PID::OTHER);
			return;
		}
		Duckyscript::begin(lines, count, usbSink);

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
			Duckyscript::end();
		}
		return;
	}

	// Script execution phase
	if (!scriptDone) {
		if ((!scriptRunning && isBtnAWasPressed()) || (!scriptRunning && isKbEnterPressed())) {
			Duckyscript::rewind();
			scriptRunning = true;
			drawScript(Duckyscript::lines(), Duckyscript::lineCount(), Duckyscript::currentLine());
		}
		if (scriptRunning) {
			Duckyscript::Progress p = Duckyscript::step();
			if (p == Duckyscript::Progress::Finished) {
				scriptRunning = false;
				scriptDone = true;
				centeredPrint(L->TXT_BT_DONE, MEDIUM_TEXT);
				soundSuccess();
			} else if (p == Duckyscript::Progress::LineAdvanced) {
				drawScript(Duckyscript::lines(), Duckyscript::lineCount(), Duckyscript::currentLine());
			}
		}
	}

	if (checkExit()) {
		scriptRunning = false;
		scriptDone = false;
		usbReady = false;
		selectedFilePath = "";
		fpActive = false;
		Duckyscript::end();
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
