bool fpActive = false;
bool fpSelectedSd = false;

static int _fpCancel = 0;
static bool _fpSourceSelected = false;
static MENU* _fpMenu = nullptr;
static String* _fpPaths = nullptr;
static int _fpCount = 0;
static MENU _fpSourceMenu[3];

/**
 * @brief Initializes and opens the file picker.
 * @param cancelPid PID to switch to when the user cancels.
 */
void filePickerSetup(int cancelPid) {
	fpActive = true;
	_fpSourceSelected = false;
	_fpCancel = cancelPid;
	_fpSourceMenu[0] = { 0, L->MENU_BACK };
	_fpSourceMenu[1] = { 0, L->MENU_FILES_LITTLEFS };
	_fpSourceMenu[2] = { 0, L->MENU_FILES_SDCARD };
	cursor = 0;
	drawMenu(_fpSourceMenu, 3);
}

static void _fpFree() {
	delete[] _fpMenu;
	_fpMenu = nullptr;
	delete[] _fpPaths;
	_fpPaths = nullptr;
	_fpCount = 0;
}

static bool _fpBuildLfs() {
	if (!lfsBegin()) { centeredPrint("LittleFS error", MEDIUM_TEXT); return false; }
	File root = LittleFS.open("/");
	File f = root.openNextFile();
	while (f) { if (!f.isDirectory()) _fpCount++; f = root.openNextFile(); }
	_fpMenu = new MENU[_fpCount + 1];
	_fpPaths = new String[_fpCount];
	_fpMenu[0] = { 0, L->MENU_BACK };
	root = LittleFS.open("/");
	f = root.openNextFile();
	for (int i = 1; f && i <= _fpCount; f = root.openNextFile()) {
		if (!f.isDirectory()) {
			String name = String(f.name());
			_fpPaths[i - 1] = "/" + name;
			_fpMenu[i] = { 0, name.substring(0, 14) };
			i++;
		}
	}
	return true;
}

#if HAS_SD
static bool _fpBuildSd() {
	if (!sdBegin()) {
		centeredPrint("SD error", MEDIUM_TEXT);
		return false;
	}
	File root = SD.open("/");
	File f = root.openNextFile();
	while (f) {
		if (!f.isDirectory()) _fpCount++;
		f = root.openNextFile();
	}
	_fpMenu = new MENU[_fpCount + 1];
	_fpPaths = new String[_fpCount];
	_fpMenu[0] = { 0, L->MENU_BACK };
	root = SD.open("/");
	f = root.openNextFile();
	for (int i = 1; f && i <= _fpCount; f = root.openNextFile()) {
		if (!f.isDirectory()) {
			String name = String(f.name());
			_fpPaths[i - 1] = "/" + name;
			_fpMenu[i] = { 0, name.substring(0, 14) };
			i++;
		}
	}
	return true;
}
#endif

/**
 * @brief Handles file picker input. Call every tick while `fpActive` is true.
 * @returns {bool} true while the picker is running, false when done.
 * @sideeffect On file selection, sets `selectedFilePath` and clears `fpActive`.
 * @sideeffect On cancel, calls `changeProcess(cancelPid)` and clears `fpActive`.
 */
bool filePickerLoop() {
	DEVICE.update();

	// Source selection menu
	if (!_fpSourceSelected) {
		if (isBtnBWasPressed() || isKbDownPressed() || isWebControlDownWasPressed()) {
			cursor++;
			drawMenu(_fpSourceMenu, 3);
		} else if (isBtnPWRWasPressed() || isKbUpPressed() || isWebControlUpWasPressed()) {
			cursor--;
			drawMenu(_fpSourceMenu, 3);
		} else if (isBtnAWasPressed() || isKbEnterPressed()) {
			if (cursor == 0) {
				fpActive = false;
				changeProcess(_fpCancel);
				return false;
			}
			// cursor 1 = LittleFS, cursor 2 = SD
			fpSelectedSd = (cursor == 2);
			bool ok = fpSelectedSd
			#if HAS_SD
				? _fpBuildSd()
			#else
				? (centeredPrint("No SD", MEDIUM_TEXT), false)
			#endif
				: _fpBuildLfs();
			if (!ok) {
				fpActive = false;
				changeProcess(_fpCancel);
				return false;
			}
			_fpSourceSelected = true;
			cursor = 0;
			drawMenu(_fpMenu, _fpCount + 1);
		}
		return true;
	}

	// File list
	if (isBtnBWasPressed() || isKbDownPressed() || isWebControlDownWasPressed()) {
		cursor++;
		drawMenu(_fpMenu, _fpCount + 1);
	} else if (isBtnPWRWasPressed() || isKbUpPressed() || isWebControlUpWasPressed()) {
		cursor--;
		drawMenu(_fpMenu, _fpCount + 1);
	} else if (isBtnAWasPressed() || isKbEnterPressed()) {
		if (cursor == 0) { // back
			_fpFree();
			_fpSourceSelected = false;
			cursor = 0;
			drawMenu(_fpSourceMenu, 3);
			return true;
		}
		selectedFilePath = _fpPaths[cursor - 1];
		_fpFree(); fpActive = false; return false;
	}

	return true;
}

/**
 * Reads a file into a single String.
 * Uses fpSelectedSd to decide between SD and LittleFS.
 *
 * @param path  Full path to the file (e.g. "/script.txt")
 * @param out   Set to file contents on success.
 * @return true on success, false if the file could not be opened or is empty.
 */
bool readFileString(const String& path, String& out) {
	out = "";
	File f;
	#if HAS_SD
		if (fpSelectedSd) {
			if (!sdBegin()) return false;
			f = SD.open(path.c_str());
		} else {
	#endif
			if (!lfsBegin()) return false;
			f = LittleFS.open(path.c_str());
	#if HAS_SD
		}
	#endif
	if (!f) return false;
	out = f.readString();
	f.close();
	return out.length() > 0;
}

/**
 * Reads a text file line-by-line into a dynamically allocated String array.
 * Uses fpSelectedSd to decide between SD and LittleFS.
 *
 * @param path      Full path to the file (e.g. "/wordlist.txt")
 * @param outLines  Set to a newly allocated String[] on success; caller must delete[].
 * @param outCount  Set to the number of lines on success.
 * @return true on success, false if the file could not be opened or is empty.
 */
bool readFileLines(const String& path, String*& outLines, int& outCount) {
	outLines = nullptr;
	outCount = 0;

	File f;
	#if HAS_SD
		if (fpSelectedSd) {
			if (!sdBegin()) return false;
			f = SD.open(path.c_str());
		} else {
	#endif
			if (!lfsBegin()) return false;
			f = LittleFS.open(path.c_str());
	#if HAS_SD
		}
	#endif
	if (!f) return false;

	int count = 0;
	while (f.available()) {
		String line = f.readStringUntil('\n');
		line.trim();
		if (line.length() > 0) count++;
	}
	if (count == 0) { f.close(); return false; }

	outLines = new String[count];
	outCount = count;

	f.seek(0);
	int i = 0;
	while (f.available() && i < count) {
		String line = f.readStringUntil('\n');
		line.trim();
		if (line.length() > 0) outLines[i++] = line;
	}
	f.close();
	return true;
}
