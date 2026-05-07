// pid PID::FILE_PICKER

MENU* lfsPickerMenu = nullptr;
int lfsPickerCount = 0;

void lfsFilePickerLoop() {
	static bool lfsError = false;

	if (isSetup()) {
		lfsError = false;
		lfsSetup(lfsPickerMenu, lfsPickerCount, lfsCancelPid);
		if (lfsPickerMenu == nullptr) {
			lfsError = true;
			return;
		}
	}

	if (lfsError) {
		checkExit(lfsCancelPid);
		return;
	}

	String selected = lfsLoop(lfsPickerMenu, lfsPickerCount, lfsCancelPid);
	if (selected != "") {
		lfsSelectedFile = selected;
		lfsClearMenu(lfsPickerMenu, lfsPickerCount);
		changeProcess(lfsReturnPid);
	}
}
