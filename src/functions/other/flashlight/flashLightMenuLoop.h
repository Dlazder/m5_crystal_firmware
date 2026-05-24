// pid PID::FLASHLIGHT_MENU

void flashLightMenuLoop() {
	MENU flashLightMenu[] = {
		{PID::OTHER,              L->MENU_BACK},
		{PID::FLASHLIGHT,         L->MENU_OTHER_FLASHLIGHT},
		{PID::FLASHLIGHT_FLICKER, L->MENU_OTHER_FLICKER},
		{PID::BRIGHTNESS,         L->MENU_SETTINGS_BRIGHTNESS},
		{PID::COLORS,             L->MENU_SETTINGS_COLORS},
	};
	int flashLightMenuSize = sizeof(flashLightMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(flashLightMenu, flashLightMenuSize);
	}
	menuLoop(flashLightMenu, flashLightMenuSize);
}
