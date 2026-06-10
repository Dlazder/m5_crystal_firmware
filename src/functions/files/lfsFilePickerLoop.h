// PID::FILE_PICKER

int lfsFileCount = 0;
MENU* lfsFileMenu = nullptr;
String* lfsFileFullPaths = nullptr;

void _lfsBuildMenu() {
	if (lfsFileMenu != nullptr) { delete[] lfsFileMenu; lfsFileMenu = nullptr; }
	if (lfsFileFullPaths != nullptr) { delete[] lfsFileFullPaths; lfsFileFullPaths = nullptr; }
	lfsFileCount = 0;

	if (!lfsBegin()) {
		centeredPrint("LittleFS error", MEDIUM_TEXT);
		return;
	}

	File root = LittleFS.open("/");
	File f = root.openNextFile();
	while (f) {
		if (!f.isDirectory()) lfsFileCount++;
		f = root.openNextFile();
	}

	lfsFileMenu = new MENU[lfsFileCount + 2];
	lfsFileFullPaths = new String[lfsFileCount];
	int backPid = (lfsCancelPid != 0) ? lfsCancelPid : PID::FILES_MENU;
	int filePid = (lfsReturnPid != 0) ? lfsReturnPid : PID::SELECTED_FILE_MENU;
	lfsFileMenu[0] = { backPid,          L->MENU_BACK };
	lfsFileMenu[1] = { PID::FILE_CREATE, "create" };

	root = LittleFS.open("/");
	f = root.openNextFile();
	int idx = 2;
	while (f && idx < lfsFileCount + 2) {
		if (!f.isDirectory()) {
			String name = String(f.name());
			lfsFileFullPaths[idx - 2] = "/" + name;
			lfsFileMenu[idx] = { filePid, name };
			idx++;
		}
		f = root.openNextFile();
	}
	lfsReturnPid = 0;
	lfsCancelPid = 0;
}

void lfsFilePickerLoop() {
	if (isSetup()) {
		cursor = 0;
		_lfsBuildMenu();
		if (lfsFileMenu == nullptr) return;
		drawMenu(lfsFileMenu, lfsFileCount + 2);
	}

	if (lfsFileMenu == nullptr) {
		checkExit(PID::FILES_MENU);
		return;
	}

	// Intercept file selection to populate selectedFilePath before switching
	if (cursor >= 2 && (isBtnAWasPressed() || isKbEnterPressed())) {
		selectedFilePath = lfsFileFullPaths[cursor - 2];
		changeProcess(lfsFileMenu[cursor].command);
		return;
	}

	menuLoop(lfsFileMenu, lfsFileCount + 2);
}
