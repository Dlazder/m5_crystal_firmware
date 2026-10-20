// PID::BAD_USB
//
// Works on any board: native USB HID on ESP32-S3 boards, or an external CH9329
// USB-HID-over-UART bridge when enabled in Settings → USB (see usbHid.h).
// On a board with neither (classic ESP32 boards with CH9329 disabled), falls
// through to the "unsupported" message, same as before usbHid.h existed.

#include "../../services/ch9329/usbHid.h"

static void usbSinkWrite(uint8_t c) { usbKbWrite(c); }
static void usbSinkPress(uint8_t c) { usbKbPress(c); }
static void usbSinkReleaseAll() { usbKbReleaseAll(); }
static const Duckyscript::Sink usbSink = { usbSinkWrite, usbSinkPress, usbSinkReleaseAll };

void badUsbLoop() {
	static bool scriptRunning = false;
	static bool scriptDone = false;
	static bool usbReady = false;

	if (isSetup()) {
		scriptRunning = false;
		scriptDone = false;
		usbReady = false;

		if (!usbHidActive()) {
			centeredPrint(L->TXT_USB_HID_UNSOPPORTED, MEDIUM_TEXT);
			return;
		}
		if (selectedFilePath == "") {
			filePickerSetup(PID::OTHER);
		}
	}

	if (!usbHidActive()) {
		checkExit();
		return;
	}
	usbHidPoll();

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

		usbHidBegin();
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
			usbHidEnd();
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
		usbHidEnd();
	}
}
