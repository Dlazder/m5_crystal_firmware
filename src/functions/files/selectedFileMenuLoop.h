// PID::SELECTED_FILE_MENU

int selectedFileSourcePid = PID::FILE_PICKER;

void selectedFileMenuLoop() {
	MENU selectedFileMenu[] = {
		{PID::FILE_PICKER, L->MENU_BACK},
		{PID::FILE_INFO, L->MENU_FILES_INFO},
		{PID::FILE_VIEW, L->MENU_FILES_VIEW},
		{PID::FILE_EDIT, L->MENU_FILES_EDIT},
		{PID::FILE_RENAME, L->MENU_FILES_RENAME},
		{PID::FILE_DELETE, L->MENU_FILES_DELETE},
	};
	int selectedFileMenuSize = sizeof(selectedFileMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::FILE_PICKER) {
			selectedFileSourcePid = PID::FILE_PICKER;
		} else if (previousProcess == PID::FILE_PICKER_SD) {
			selectedFileSourcePid = PID::FILE_PICKER_SD;
		}
		cursor = 0;
		drawMenu(selectedFileMenu, selectedFileMenuSize);
	}
	selectedFileMenu[0].command = selectedFileSourcePid;
	menuLoop(selectedFileMenu, selectedFileMenuSize);
}
