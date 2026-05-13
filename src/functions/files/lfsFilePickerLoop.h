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
	lfsFileMenu[0] = { PID::FILES_MENU,  L->MENU_BACK };
	lfsFileMenu[1] = { PID::FILE_CREATE, "create" };

	root = LittleFS.open("/");
	f = root.openNextFile();
	int idx = 2;
	while (f && idx < lfsFileCount + 2) {
		if (!f.isDirectory()) {
			String name = String(f.name());
			lfsFileFullPaths[idx - 2] = "/" + name;
			lfsFileMenu[idx] = { PID::SELECTED_FILE_MENU, name.substring(0, 14) };
			idx++;
		}
		f = root.openNextFile();
	}
}

void lfsFilePickerLoop() {
	if (isSetup()) {
		cursor = 0;
		_lfsBuildMenu();
		if (lfsFileMenu == nullptr) return;
		cursorOnTop();
		drawMenu(lfsFileMenu, lfsFileCount + 2);
	}

	if (lfsFileMenu == nullptr) {
		checkExit(PID::FILES_MENU);
		return;
	}

	menuLoop(lfsFileMenu, lfsFileCount + 2);
}
