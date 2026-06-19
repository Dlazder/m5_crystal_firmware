// pid PID::BAD_BLE

/**
 * Renders the script with the current line highlighted.
 * All lines are drawn dark grey; the line at badBleCurrentLine is green and
 * anchored at a fixed row (the 4th line) so the script scrolls past it.
 */
void badBleDrawScript() {
	if (badBleLines == nullptr || badBleLineCount == 0) return;

	canvas.clear();
	canvas.setTextSize(SMALL_TEXT);

	int lineHeight = canvas.fontHeight();

	const int paddingX = 5;
	const int maxChars = 20;
	const int anchorRow = 3; // current line settles on the 4th row once reached

	int total = badBleLineCount;
	int current = min(badBleCurrentLine, total - 1);

	// The row the current line occupies: it climbs from the top to anchorRow,
	// then stays fixed so the script scrolls past it (no empty rows on top).
	int currentRow = min(current, anchorRow);

	for (int i = 0; i < total; i++) {
		int y = (currentRow + (i - current)) * lineHeight;
		if (y + lineHeight <= 0 || y >= canvas.height()) continue; // off-screen

		String text = badBleLines[i];
		if ((int)text.length() > maxChars) text = text.substring(0, maxChars);

		canvas.setTextColor(i == current ? FGCOLOR : TFT_DARKGREY, BGCOLOR);
		canvas.setCursor(paddingX, y);
		canvas.print(text.c_str());
	}

	canvas.pushSprite(0, getStatusBarOffset());
}

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

		if (!badBleSetScript()) {
			centeredPrint(L->TXT_BT_FILE_ERROR, MEDIUM_TEXT);
			selectedFilePath = "";
			filePickerSetup(PID::BLUETOOTH);
			return;
		}
		bleConnect();
		return;
	}

	// BLE connection phase
	bleHandleConnection(
		[]() { centeredPrint(L->TXT_BT_PRESS_A_TO_RUN, MEDIUM_TEXT); soundSuccess(); },
		[]() { scriptRunning = false; centeredPrint(L->TXT_DISCONNECTED, MEDIUM_TEXT); soundError(); }
	);

	// Script execution phase
	if (bleConnected && !scriptDone) {
		if (!scriptRunning && isBtnAWasPressed() || (isKbEnterPressed())) {
			badBleCurrentLine = 0;
			badBleDelayUntil = 0;
			scriptRunning = true;
			badBleDrawScript();
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
			} else {
				badBleDrawScript();
			}
		}
	}

	if (checkExit()) {
		bleConnected = false;
		scriptRunning	= false;
		scriptDone = false;
		selectedFilePath = "";
		fpActive = false;
		badBleFreeLines();
	}
}
