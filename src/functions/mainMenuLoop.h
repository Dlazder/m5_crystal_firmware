// pid PID::MAIN_MENU

void mainMenuLoop() {
	MENU mainMenu[] = {
		{PID::WIFI,      L->MENU_WIFI},
		{PID::BLUETOOTH, L->MENU_BT},
		{PID::NFC,       L->MENU_NFC},
		{PID::IR,        L->MENU_IR},
		{PID::USB,       L->MENU_USB},
		{PID::SETTINGS,  L->MENU_SETTINGS},
		{PID::FILES_MENU, L->MENU_FILES},
		{PID::OTHER,     L->MENU_OTHER},
	};
	int mainMenuSize = sizeof(mainMenu) / sizeof(MENU);

	if (isSetup()) {
		drawMenu(mainMenu, mainMenuSize);
	}
	menuLoop(mainMenu, mainMenuSize);
}
