// PID::FILE_PICKER_SD

int sdFileCount = 0;
MENU* sdFileMenu = nullptr;
String* sdFileFullPaths = nullptr;

#if HAS_SD
void _sdBuildMenu() {
	if (sdFileMenu != nullptr) { delete[] sdFileMenu; sdFileMenu = nullptr; }
	if (sdFileFullPaths != nullptr) { delete[] sdFileFullPaths; sdFileFullPaths = nullptr; }
	sdFileCount = 0;

	if (!sdBegin()) {
		centeredPrint("SD error", MEDIUM_TEXT);
		return;
	}

	File root = SD.open("/");
	File f = root.openNextFile();
	while (f) {
		if (!f.isDirectory()) sdFileCount++;
		f = root.openNextFile();
	}

	sdFileMenu = new MENU[sdFileCount + 2];
	sdFileFullPaths = new String[sdFileCount];
	sdFileMenu[0] = { PID::FILES_MENU,   L->MENU_BACK };
	sdFileMenu[1] = { PID::FILE_CREATE,  "create" };

	root = SD.open("/");
	f = root.openNextFile();
	int idx = 2;
	while (f && idx < sdFileCount + 2) {
		if (!f.isDirectory()) {
			String name = String(f.name());
			sdFileFullPaths[idx - 2] = "/" + name;
			sdFileMenu[idx] = { PID::SELECTED_FILE_MENU, name };
			idx++;
		}
		f = root.openNextFile();
	}
}
#endif

void filePickerSDLoop() {
#if HAS_SD
	if (isSetup()) {
		cursor = 0;
		_sdBuildMenu();
		if (sdFileMenu == nullptr) return;
		drawMenu(sdFileMenu, sdFileCount + 2);
	}

	if (sdFileMenu == nullptr) {
		checkExit(PID::FILES_MENU);
		return;
	}

	if (cursor >= 2 && (isBtnAWasPressed() || isKbEnterPressed())) {
		selectedFilePath = sdFileFullPaths[cursor - 2];
		changeProcess(sdFileMenu[cursor].command);
		return;
	}

	menuLoop(sdFileMenu, sdFileCount + 2);
#else
	if (isSetup()) {
		centeredPrint("No SD", MEDIUM_TEXT);
	}
	checkExit(PID::FILES_MENU);
#endif
}
