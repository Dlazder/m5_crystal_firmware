#include <LittleFS.h>

bool lfsBegun = false;
String* lfsFilePaths = nullptr;
String lfsSelectedFile = "";
int lfsReturnPid = 0;

/**
 * Mounts LittleFS. Safe to call multiple times.
 * @return true if mounted successfully, false on failure
 */
bool lfsBegin() {
	if (lfsBegun) return true;
	lfsBegun = LittleFS.begin(false);
	return lfsBegun;
}

/**
 * Clears MENU array and path list built by lfsSetup().
 * @param menu   reference to MENU pointer to free (set to nullptr)
 * @param count  reference to file count (reset to 0)
 */
void lfsClearMenu(MENU*& menu, int& count) {
	if (menu != nullptr) { delete[] menu; menu = nullptr; }
	if (lfsFilePaths != nullptr) { delete[] lfsFilePaths; lfsFilePaths = nullptr; }
	count = 0;
}

/**
 * Scans the LittleFS root, builds a MENU array and draws it.
 * Full paths are stored in lfsFilePaths[] parallel to menu entries.
 * Caller must clear with lfsClearMenu().
 * @param menu       reference to MENU pointer to allocate
 * @param count      reference to file count (output)
 * @param backPid    PID to switch to when "back" is selected
 * @param targetPid  PID assigned to each file entry (used by lfsLoop internally)
 */
void lfsSetup(MENU*& menu, int& count, int backPid, int targetPid) {
	lfsClearMenu(menu, count);

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
 * Redirects to the file picker process (PID 44) and returns to the current process after selection.
 * Call this when lfsSelectedFile is empty and a file is needed.
 */
bool lfsPickFile() {
	if (lfsSelectedFile != "") return false;
	lfsReturnPid = process;
	changeProcess(44);
	return true;
}

/**
 * Handles navigation for an LFS file menu built by lfsSetup().
 * Must be called every loop tick while in file browser phase.
 * @param menu     MENU array built by lfsSetup()
 * @param count    number of files (excluding "back" entry)
 * @param backPid  PID to switch to when "back" is selected or list is empty
 * @return full path of the selected file, or "" if nothing selected yet
 */
String lfsLoop(MENU menu[], int count, int backPid) {
	if (count == 0) { changeProcess(backPid); return ""; }

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
		if (cursor == 0) { changeProcess(backPid); return ""; }
		return lfsFilePaths[cursor - 1];
	}

	return "";
}
