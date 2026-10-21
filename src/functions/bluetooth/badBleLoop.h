// PID::BAD_BLE

static void bleSinkWrite(uint8_t c) { Ble::keyboard.write(c); }
static void bleSinkPress(uint8_t c) { Ble::keyboard.press(c); }
static void bleSinkReleaseAll() { Ble::keyboard.releaseAll(); }
static const Duckyscript::Sink bleSink = { bleSinkWrite, bleSinkPress, bleSinkReleaseAll };

void badBleLoop() {
	static bool scriptRunning = false;
	static bool scriptDone = false;

	if (isSetup()) {
		Ble::connected = false;
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

		String* lines = nullptr;
		int count = 0;
		if (!Storage::readLines(selectedFilePath, lines, count, !fpSelectedSd)) {
			centeredPrint(L->TXT_BT_FILE_ERROR, MEDIUM_TEXT);
			selectedFilePath = "";
			filePickerSetup(PID::BLUETOOTH);
			return;
		}
		Duckyscript::begin(lines, count, bleSink);
		Ble::begin();
		return;
	}

	// BLE connection phase
	Ble::handleConnection(
		[]() { centeredPrint(L->TXT_BT_PRESS_A_TO_RUN, MEDIUM_TEXT); soundSuccess(); },
		[]() { scriptRunning = false; centeredPrint(L->TXT_DISCONNECTED, MEDIUM_TEXT); soundError(); }
	);

	// Script execution phase
	if (Ble::connected && !scriptDone) {
		if (!scriptRunning && isBtnAWasPressed() || (!scriptRunning && isKbEnterPressed())) {
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
		Ble::connected = false;
		scriptRunning	= false;
		scriptDone = false;
		selectedFilePath = "";
		fpActive = false;
		Duckyscript::end();
	}
}
