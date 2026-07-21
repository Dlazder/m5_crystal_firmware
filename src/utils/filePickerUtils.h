/**
 * Generates a unique file path by appending an incrementing counter
 * before the extension until an unused name is found.
 *
 * Example: generateUniqueFilename("/handshake_MyWiFi", ".pcap", false)
 *          → "/handshake_MyWiFi_1.pcap" (or _2, _3, …)
 *
 * @param basePath    Path before the counter (e.g. "/handshake_MyWiFi")
 * @param ext         File extension including dot (e.g. ".pcap")
 * @param useLittleFS true = check LittleFS, false = check SD
 * @return Unique path like "/handshake_MyWiFi_1.pcap"
 */
String generateUniqueFilename(const String& basePath, const String& ext, bool useLittleFS) {
	String path;
	int n = 1;
	do {
		path = basePath + "_" + String(n) + ext;
		n++;
	} while (useLittleFS ? LittleFS.exists(path) : SD.exists(path));
	return path;
}

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
 * @brief Scans a directory and returns sorted entry names + directory flags.
 *				Directories come first, then files; each group sorted alphabetically.
 * @param fs Filesystem reference (LittleFS or SD).
 * @param dirPath Path to scan.
 * @param outNames Set to newly allocated String[] on success; caller must delete[].
 * @param outIsDir Set to newly allocated bool[] on success; caller must delete[].
 * @return Number of entries, or -1 on error.
 */
static int _scanDir(fs::FS& fs, const String& dirPath, String*& outNames, bool*& outIsDir) {
	File dir = fs.open(dirPath);
	if (!dir) return -1;

	// Count
	File f = dir.openNextFile();
	int count = 0;
	while (f) { count++; f = dir.openNextFile(); }

	if (count == 0) { outNames = nullptr; outIsDir = nullptr; return 0; }

	outNames = new String[count];
	outIsDir = new bool[count];

	// Collect
	dir = fs.open(dirPath);
	f = dir.openNextFile();
	for (int i = 0; i < count; i++) {
		outNames[i] = String(f.name());
		outIsDir[i] = f.isDirectory();
		f = dir.openNextFile();
	}

	// Sort: directories first (alphabetically), then files (alphabetically)
	for (int i = 0; i < count - 1; i++) {
		for (int j = i + 1; j < count; j++) {
			bool swap = false;
			if (!outIsDir[i] && outIsDir[j]) {
				swap = true;
			} else if (outIsDir[i] == outIsDir[j] && outNames[i].compareTo(outNames[j]) > 0) {
				swap = true;
			}
			if (swap) {
				String tmpName = outNames[i]; outNames[i] = outNames[j]; outNames[j] = tmpName;
				bool tmpDir = outIsDir[i]; outIsDir[i] = outIsDir[j]; outIsDir[j] = tmpDir;
			}
		}
	}

	return count;
}

/**
 * @brief Navigates `dir` to its parent. If already at root, stays at "/".
 */
static void _goParentDir(String& dir) {
	int lastSlash = dir.lastIndexOf('/');
	dir = (lastSlash <= 0) ? "/" : dir.substring(0, lastSlash);
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
	if (!lfsBegin()) { centeredPrint("LittleFS error", MEDIUM_TEXT); return false; }

	String* names = nullptr;
	bool* isDir = nullptr;
	_fpCount = _scanDir(LittleFS, _fpCurrentDir, names, isDir);
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

#if HAS_SD
static bool _fpBuildSd() {
	if (!sdBegin()) { centeredPrint("SD error", MEDIUM_TEXT); return false; }

	String* names = nullptr;
	bool* isDir = nullptr;
	_fpCount = _scanDir(SD, _fpCurrentDir, names, isDir);
	if (_fpCount < 0) return false;

	_fpMenu = new MENU[_fpCount + 1];
	_fpPaths = new String[_fpCount];
	_fpMenu[0] = { 0, L->MENU_BACK, Icons::back };

	for (int i = 0; i < _fpCount; i++) {
		_fpPaths[i] = _fpMakePath(_fpCurrentDir, names[i]);
		_fpMenu[i + 1] = { 0, isDir[i] ? "/" + names[i] : names[i], isDir[i] ? Icons::folder : Icons::file};
	}

	delete[] names;
	delete[] isDir;
	return true;
}
#endif

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
			fpSelectedSd = (cursor == 2);
			_fpCurrentDir = "/";
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
		if (cursor == 0) {
			if (_fpCurrentDir == "/") {
				_fpFree();
				_fpSourceSelected = false;
				cursor = 0;
				drawMenu(_fpSourceMenu, 3);
				return true;
			}
			_goParentDir(_fpCurrentDir);
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

/**
 * Reads a file into a single String.
 * Uses fpSelectedSd to decide between SD and LittleFS.
 *
 * @param path Full path to the file (e.g. "/script.txt")
 * @param out Set to file contents on success.
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
 * @param path Full path to the file (e.g. "/wordlist.txt")
 * @param outLines Set to a newly allocated String[] on success; caller must delete[].
 * @param outCount Set to the number of lines on success.
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
