// pid PID::FILE_PICKER

MENU* lfsPickerMenu = nullptr;
int lfsPickerCount = 0;

void lfsFilePickerLoop() {
	static bool lfsError = false;

	if (isSetup()) {
		lfsError = false;
		lfsSelectedFile = "";
		lfsSetup(lfsPickerMenu, lfsPickerCount, previousProcess, PID::FILE_PICKER);
		if (lfsPickerMenu == nullptr) {
			lfsError = true;
			return;
		}
	}

	if (lfsError) {
		checkExit(previousProcess);
		return;
	}

	String selected = lfsLoop(lfsPickerMenu, lfsPickerCount, previousProcess);
	if (selected != "") {
		lfsSelectedFile = selected;
		lfsClearMenu(lfsPickerMenu, lfsPickerCount);
		changeProcess(lfsReturnPid);
	}
}
