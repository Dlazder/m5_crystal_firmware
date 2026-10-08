// PID::FILES_MENU

void filesMenuLoop() {
	MENU filesMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK, Icons::back},
		{PID::FILE_PICKER, L->MENU_FILES_LITTLEFS, Icons::lfs},
		{PID::FILE_PICKER_SD, L->MENU_FILES_SDCARD, Icons::sd},
		{PID::LFS_WEB_UI, "LFS WEB UI", Icons::lfs},
	};
	int filesMenuSize = sizeof(filesMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(filesMenu, filesMenuSize);
	}
	menuLoop(filesMenu, filesMenuSize);
}