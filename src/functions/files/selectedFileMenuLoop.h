// PID::SELECTED_FILE_MENU

int selectedFileSourcePid = PID::FILE_PICKER;

void selectedFileMenuLoop() {
	MENU selectedFileMenu[] = {
		{PID::FILE_PICKER, L->MENU_BACK, Icons::back},
		{PID::FILE_INFO, L->MENU_FILES_INFO, Icons::info},
		{PID::FILE_VIEW, L->MENU_FILES_VIEW, Icons::view},
		{PID::FILE_EDIT, L->MENU_FILES_EDIT, Icons::edit},
		{PID::FILE_RENAME, L->MENU_FILES_RENAME, Icons::edit},
		{PID::FILE_DELETE, L->MENU_FILES_DELETE, Icons::remove},
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
