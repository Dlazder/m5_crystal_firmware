// pid PID::OTHER

void otherMenuLoop() {
	MENU otherMenu[] = {
		{PID::MAIN_MENU,       L->MENU_BACK},
		{PID::SYSTEM,          L->MENU_OTHER_SYSTEM},
		{PID::CLOCK,           L->MENU_OTHER_CLOCK},
		{PID::SOUND_LEVEL,     L->MENU_OTHER_SOUND_LEVEL},
		{PID::FLASHLIGHT_MENU, L->MENU_OTHER_FLASHLIGHT},
		{PID::LEVEL_TOOL,      L->MENU_OTHER_LEVEL_TOOL},
		{PID::BATTERY,         L->MENU_OTHER_BATTERY_INFO},
	};
	int otherMenuSize = sizeof(otherMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(otherMenu, otherMenuSize);
	}
	menuLoop(otherMenu, otherMenuSize);
}
