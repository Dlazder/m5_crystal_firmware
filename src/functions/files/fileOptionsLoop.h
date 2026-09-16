// PID::FILE_OPTIONS

void fileOptionsLoop() {
	MENU optionsMenu[] = {
		{PID::FILE_PICKER, L->MENU_BACK, Icons::back},
		{PID::FILE_CREATE, L->MENU_FILES_CREATE_FILE, Icons::create},
		{PID::FILE_CREATE_DIR, L->MENU_FILES_CREATE_DIR, Icons::create},
		{PID::FILE_FOLDER_INFO, L->MENU_FILES_FOLDER_INFO, Icons::info},
		{PID::FILE_RENAME_DIR, L->MENU_FILES_RENAME_DIR, Icons::edit},
		{PID::FILE_DELETE_DIR, L->MENU_FILES_DELETE_DIR, Icons::remove},
	};
	int optionsMenuSize = sizeof(optionsMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(optionsMenu, optionsMenuSize);
	}

	optionsMenu[0].command = fileOptionsSourcePid;
	menuLoop(optionsMenu, optionsMenuSize);
}
