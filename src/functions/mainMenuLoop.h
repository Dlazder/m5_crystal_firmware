// PID::MAIN_MENU

void mainMenuLoop() {
	MENU mainMenu[] = {
		{PID::WIFI, L->MENU_WIFI, Icons::wifi},
		{PID::BLUETOOTH, L->MENU_BT, Icons::bluetooth},
		{PID::NFC, L->MENU_NFC, Icons::nfc},
		{PID::IR, L->MENU_IR, Icons::ir},
		{PID::USB, L->MENU_USB, Icons::usb},
		{PID::UART, L->MENU_UART, Icons::terminal},
		{PID::SETTINGS, L->MENU_SETTINGS, Icons::settings},
		{PID::FILES_MENU, L->MENU_FILES, Icons::folder},
		{PID::OTHER, L->MENU_OTHER, Icons::other},
	};
	int mainMenuSize = sizeof(mainMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(mainMenu, mainMenuSize);
	}
	menuLoop(mainMenu, mainMenuSize);
}
