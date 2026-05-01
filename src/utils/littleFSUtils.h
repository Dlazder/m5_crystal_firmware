#include <LittleFS.h>

bool lfsBegun = false;
String* lfsFilePaths = nullptr;

/**
 * Mounts LittleFS. Returns false on failure.
 */
bool lfsBegin() {
	if (lfsBegun) return true;
	lfsBegun = LittleFS.begin(false);
	return lfsBegun;
}

/**
 * Scans the LittleFS root, builds a MENU array and draws it.
 * File entries get command=targetPid. "back" entry gets command=backPid.
 * Full paths are stored in lfsFilePaths[] parallel to menu entries.
 * Caller must free with lfsFreeMenu().
 */
void lfsSetup(MENU*& menu, int& count, int backPid, int targetPid) {
	if (menu != nullptr) { delete[] menu; menu = nullptr; }
	if (lfsFilePaths != nullptr) { delete[] lfsFilePaths; lfsFilePaths = nullptr; }
	count = 0;

	if (!lfsBegin()) { centeredPrint("LittleFS error", SMALL_TEXT); return; }

	File root = LittleFS.open("/");
	File f = root.openNextFile();
	while (f) {
		if (!f.isDirectory()) count++;
		f = root.openNextFile();
	}

	menu = new MENU[count + 1];
	lfsFilePaths = new String[count];
	menu[0] = { backPid, "back" };

	root = LittleFS.open("/");
	f = root.openNextFile();
	int idx = 1;
	while (f && idx <= count) {
		if (!f.isDirectory()) {
			String name = String(f.name());
			lfsFilePaths[idx - 1] = "/" + name;
			menu[idx] = { targetPid, name.substring(0, 14) };
			idx++;
		}
		f = root.openNextFile();
	}

	cursor = 0;
	cursorOnTop();
	count > 0 ? drawMenu(menu, count + 1) : centeredPrint("No files", SMALL_TEXT);
}

/**
 * Handles navigation for an LFS file menu.
 * Returns the full path of the selected file, or "" if nothing selected yet.
 * On back (cursor==0) calls checkExit(backPid).
 */
String lfsLoop(MENU* menu, int count, int backPid) {
	if (count == 0) { checkExit(backPid); return ""; }

	DEVICE.update();

	// We do not use menuLoop, as it causes a process change. Processing navigation manually
	if (isBtnBWasPressed()) {
		cursor++;
		cursorOnTop();
		drawMenu(menu, count + 1);
	}

	if (isBtnPWRWasPressed()) {
		cursor--;
		cursorOnTop();
		drawMenu(menu, count + 1);
	}

	if (isBtnAWasPressed()) {
		if (cursor == 0) { checkExit(backPid); return ""; }
		return lfsFilePaths[cursor - 1];
	}

	return "";
}

/**
 * Frees MENU array and path list built by lfsSetup().
 */
void lfsFreeMenu(MENU*& menu, int& count) {
	if (menu != nullptr) { delete[] menu; menu = nullptr; }
	if (lfsFilePaths != nullptr) { delete[] lfsFilePaths; lfsFilePaths = nullptr; }
	count = 0;
}
