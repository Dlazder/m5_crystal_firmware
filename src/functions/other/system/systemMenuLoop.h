// pid PID::SYSTEM

void systemMenuLoop() {
	MENU systemMenu[] = {
		{PID::OTHER,       L->MENU_BACK, Icons::back},
		{PID::SHUTDOWN,    L->MENU_SYSTEM_SHUTDOWN},
		{PID::SYSTEM_INFO, L->MENU_SYSTEM_INFO},
		{PID::BATTERY,     L->MENU_SYSTEM_BATTERY_INFO},
	};
	int systemMenuSize = sizeof(systemMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(systemMenu, systemMenuSize);
	}
	menuLoop(systemMenu, systemMenuSize);
}
