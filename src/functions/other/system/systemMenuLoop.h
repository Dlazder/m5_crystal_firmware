// pid PID::SYSTEM

void systemMenuLoop() {
	MENU systemMenu[] = {
		{PID::OTHER,       L->MENU_BACK},
		{PID::SHUTDOWN,    L->MENU_SYSTEM_SHUTDOWN},
		{PID::SYSTEM_INFO, L->MENU_SYSTEM_INFO},
	};
	int systemMenuSize = sizeof(systemMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(systemMenu, systemMenuSize);
	}
	menuLoop(systemMenu, systemMenuSize);
}
