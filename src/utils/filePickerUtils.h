bool fpActive = false;

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
			bool ok = (cursor == 2)
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
