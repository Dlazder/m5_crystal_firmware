// PID::OTHER

void otherMenuLoop() {
	MENU otherMenu[] = {
		{PID::MAIN_MENU,       L->MENU_BACK, Icons::back},
		{PID::CLOCK,           L->MENU_OTHER_CLOCK, Icons::clock},
		{PID::SOUND_LEVEL,     L->MENU_OTHER_SOUND_LEVEL, Icons::volume},
		{PID::LEVEL_TOOL,      L->MENU_OTHER_LEVEL_TOOL, Icons::degree},
		{PID::FLASHLIGHT_MENU, L->MENU_OTHER_FLASHLIGHT, Icons::flashlight},
		{PID::SYSTEM,          L->MENU_OTHER_SYSTEM, Icons::info},
	};
	int otherMenuSize = sizeof(otherMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(otherMenu, otherMenuSize);
	}
	menuLoop(otherMenu, otherMenuSize);
}
