// pid PID::MAIN_MENU

void mainMenuLoop() {
	MENU mainMenu[] = {
		{PID::WIFI,      L->MENU_WIFI},
		{PID::BLUETOOTH, L->MENU_BT},
		{PID::NFC,       L->MENU_NFC},
		{PID::SETTINGS,  L->MENU_SETTINGS},
		{PID::OTHER,     L->MENU_OTHER},
	};
	int mainMenuSize = sizeof(mainMenu) / sizeof(MENU);

	if (isSetup()) {
		cursorOnTop();
		drawMenu(mainMenu, mainMenuSize);
	}
	menuLoop(mainMenu, mainMenuSize);
}
