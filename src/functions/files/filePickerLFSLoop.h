// PID::FILE_PICKER

int lfsFileCount = 0;
MENU* lfsFileMenu = nullptr;
String* lfsFileFullPaths = nullptr;
String lfsCurrentDir = "/";

void _lfsBuildMenu() {
	if (lfsFileMenu != nullptr) { delete[] lfsFileMenu; lfsFileMenu = nullptr; }
	if (lfsFileFullPaths != nullptr) { delete[] lfsFileFullPaths; lfsFileFullPaths = nullptr; }
	lfsFileCount = 0;

	if (!Storage::mountLittleFS()) { centeredPrint("LittleFS error", MEDIUM_TEXT); return; }

	String* names = nullptr;
	bool* isDir = nullptr;
	lfsFileCount = Storage::list(lfsCurrentDir, true, names, isDir);
	if (lfsFileCount < 0) return;

	lfsFileMenu = new MENU[lfsFileCount + 2];
	lfsFileFullPaths = new String[lfsFileCount];

	int backPid = (lfsCurrentDir == "/") ? PID::FILES_MENU : 0;
	int filePid = PID::SELECTED_FILE_MENU;

	lfsFileMenu[0] = { backPid, L->MENU_BACK, Icons::back };
	lfsFileMenu[1] = { PID::FILE_OPTIONS, L->MENU_FILES_OPTIONS, Icons::other };

	for (int i = 0; i < lfsFileCount; i++) {
		lfsFileFullPaths[i] = _fpMakePath(lfsCurrentDir, names[i]);
		int cmd = isDir[i] ? PID::FILE_PICKER : filePid;
		lfsFileMenu[i + 2] = { cmd, isDir[i] ? "/" + names[i] : names[i], isDir[i] ? Icons::folder : Icons::file };
	}

	delete[] names;
	delete[] isDir;
}

void filePickerLFSLoop() {
	if (isSetup()) {
		cursor = 0;
		lfsCurrentDir = "/";
		_lfsBuildMenu();
		if (lfsFileMenu == nullptr) return;
		drawMenu(lfsFileMenu, lfsFileCount + 2);
	}

	if (lfsFileMenu == nullptr) {
		checkExit(PID::FILES_MENU);
		return;
	}

	int totalItems = lfsFileCount + 2;

	if (isBtnBWasPressed() || isKbDownPressed() || isWebControlDownWasPressed()) {
		cursor++;
		drawMenu(lfsFileMenu, totalItems);
	}
	if (isBtnPWRWasPressed() || isKbUpPressed() || isWebControlUpWasPressed()) {
		cursor--;
		drawMenu(lfsFileMenu, totalItems);
	}

	if (isBtnAWasPressed() || isKbEnterPressed()) {
		if (cursor == 0) {
			if (lfsCurrentDir == "/") {
				changeProcess(PID::FILES_MENU);
			} else {
				_goParentDir(lfsCurrentDir);
				_lfsBuildMenu();
				cursor = 0;
				drawMenu(lfsFileMenu, lfsFileCount + 2);
			}
			return;
		}

		if (cursor == 1) {
			createFileCurrentDir = lfsCurrentDir;
			fileOptionsSourcePid = PID::FILE_PICKER;
			changeProcess(PID::FILE_OPTIONS);
			return;
		}

		String selectedPath = lfsFileFullPaths[cursor - 2];

		if (lfsFileMenu[cursor].name.startsWith("/")) {
			lfsCurrentDir = selectedPath;
			_lfsBuildMenu();
			cursor = 0;
			drawMenu(lfsFileMenu, lfsFileCount + 2);
		} else {
			selectedFilePath = selectedPath;
			changeProcess(lfsFileMenu[cursor].command);
		}
		return;
	}

	if (isWebDataRequested()) {
		webData = generateWebData("menu", generateMenuString(lfsFileMenu, totalItems));
	}
}
