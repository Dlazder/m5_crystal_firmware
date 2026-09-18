bool fpActive = false;
bool fpSelectedSd = false;

static int _fpCancel = 0;
static bool _fpSourceSelected = false;
static MENU* _fpMenu = nullptr;
static String* _fpPaths = nullptr;
static int _fpCount = 0;
static MENU _fpSourceMenu[3];
static String _fpCurrentDir = "/";

/**
 * @brief Root directory for the active picker source (SD uses sdRootDir(),
 * LittleFS is always "/").
 */
static String _fpRoot() { return fpSelectedSd ? sdRootDir() : "/"; }

/**
 * @brief Navigates `dir` to its parent. If already at root, stays at "/".
 */
static void _goParentDir(String& dir) {
	int lastSlash = dir.lastIndexOf('/');
	dir = (lastSlash <= 0) ? "/" : dir.substring(0, lastSlash);
}

/**
 * @brief Like _goParentDir, but never navigates above `root`.
 */
static void _goParentDirTo(String& dir, const String& root) {
	if (dir == root) return;
	int lastSlash = dir.lastIndexOf('/');
	dir = (lastSlash <= 0 || dir.substring(0, lastSlash).length() < root.length())
		? root
		: dir.substring(0, lastSlash);
}

/**
 * @brief Builds a full path from directory and entry name.
 */
static String _fpMakePath(const String& dir, const String& name) {
	return (dir == "/") ? "/" + name : dir + "/" + name;
}

/**
 * @brief Initializes and opens the file picker.
 * @param cancelPid PID to switch to when the user cancels.
 */
void filePickerSetup(int cancelPid) {
	fpActive = true;
	_fpSourceSelected = false;
	_fpCancel = cancelPid;
	_fpSourceMenu[0] = { 0, L->MENU_BACK, Icons::back };
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
	if (!Storage::mountLittleFS()) { centeredPrint("LittleFS error", MEDIUM_TEXT); return false; }

	String* names = nullptr;
	bool* isDir = nullptr;
	_fpCount = Storage::list(_fpCurrentDir, true, names, isDir);
	if (_fpCount < 0) return false;

	_fpMenu = new MENU[_fpCount + 1];
	_fpPaths = new String[_fpCount];
	_fpMenu[0] = { 0, L->MENU_BACK, Icons::back };

	for (int i = 0; i < _fpCount; i++) {
		_fpPaths[i] = _fpMakePath(_fpCurrentDir, names[i]);
		_fpMenu[i + 1] = { 0, isDir[i] ? "/" + names[i] : names[i], isDir[i] ? Icons::folder : Icons::file };
	}

	delete[] names;
	delete[] isDir;
	return true;
}

static bool _fpBuildSd() {
	// LittleFS may still be mounted from a prior LFS pick (holding ~10 KB of regular RAM).
	Storage::unmountLittleFS();
	if (!Storage::mountSD()) { centeredPrint("SD error", MEDIUM_TEXT); return false; }

	String* names = nullptr;
	bool* isDir = nullptr;
	_fpCount = Storage::list(_fpCurrentDir, false, names, isDir);
	if (_fpCount < 0) return false;

	_fpMenu = new MENU[_fpCount + 1];
	_fpPaths = new String[_fpCount];
	_fpMenu[0] = { 0, L->MENU_BACK, Icons::back };

	for (int i = 0; i < _fpCount; i++) {
		_fpPaths[i] = _fpMakePath(_fpCurrentDir, names[i]);
		_fpMenu[i + 1] = { 0, isDir[i] ? "/" + names[i] : names[i], isDir[i] ? Icons::folder : Icons::file };
	}

	delete[] names;
	delete[] isDir;
	return true;
}

static void _fpRebuildAndDraw() {
	_fpFree();
	fpSelectedSd ? _fpBuildSd() : _fpBuildLfs();
	cursor = 0;
	drawMenu(_fpMenu, _fpCount + 1);
}

/**
 * @brief Handles file picker input. Call every tick while `fpActive` is true.
 * @returns {bool} true while the picker is running, false when done.
 * @sideeffect On file selection, sets `selectedFilePath` and clears `fpActive`.
 * @sideeffect On cancel, calls `changeProcess(cancelPid)` and clears `fpActive`.
 */
bool filePickerLoop() {
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
			fpSelectedSd = (cursor == 2);
			_fpCurrentDir = _fpRoot();
			bool ok = fpSelectedSd ? _fpBuildSd() : _fpBuildLfs();
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
		if (cursor == 0) {
			if (_fpCurrentDir == _fpRoot()) {
				_fpFree();
				_fpSourceSelected = false;
				cursor = 0;
				drawMenu(_fpSourceMenu, 3);
				return true;
			}
			_goParentDirTo(_fpCurrentDir, _fpRoot());
			_fpRebuildAndDraw();
			return true;
		}

		String selectedPath = _fpPaths[cursor - 1];

		if (_fpMenu[cursor].name.startsWith("/")) {
			_fpCurrentDir = selectedPath;
			_fpRebuildAndDraw();
		} else {
			selectedFilePath = selectedPath;
			_fpFree(); fpActive = false; return false;
		}
	}

	return true;
}
