#include <LittleFS.h>

bool lfsBegun = false;
String* lfsFilePaths = nullptr;
String lfsSelectedFile = "";
int lfsReturnPid = 0;
int lfsCancelPid = 0;

/**
 * Mounts LittleFS. Safe to call multiple times.
 * @return true if mounted successfully, false on failure
 */
bool lfsBegin() {
	if (lfsBegun) return true;
	lfsBegun = LittleFS.begin(true);
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
 * @param menu     reference to MENU pointer to allocate
 * @param count    reference to file count (output)
 * @param backPid  PID to switch to when "back" is selected
 */
void lfsSetup(MENU*& menu, int& count, int backPid) {
	lfsClearMenu(menu, count);

	if (!lfsBegin()) { centeredPrint("LittleFS error", MEDIUM_TEXT); return; }

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
			menu[idx] = { PID::FILE_PICKER, name };
			idx++;
		}
		f = root.openNextFile();
	}

	cursor = 0;
	count > 0 ? drawMenu(menu, count + 1) : centeredPrint("No files", MEDIUM_TEXT);
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

	if (isBtnBWasPressed() || isKbDownPressed()) {
		cursor++;
		drawMenu(menu, count + 1);
	}

	if (isBtnPWRWasPressed() || isKbUpPressed()) {
		cursor--;
		drawMenu(menu, count + 1);
	}

	if (isBtnAWasPressed() || isKbEnterPressed()) {
		if (cursor == 0) { changeProcess(backPid); return ""; }
		return lfsFilePaths[cursor - 1];
	}

	return "";
}

/**
 * Opens the file picker.
 * @param returnPid  PID to switch to after a file is selected
 * @param cancelPid  PID to switch to if the user presses "back"
 */
void lfsOpenPicker(int returnPid, int cancelPid) {
	lfsReturnPid = returnPid;
	lfsCancelPid = cancelPid;
	changeProcess(PID::FILE_PICKER);
}
