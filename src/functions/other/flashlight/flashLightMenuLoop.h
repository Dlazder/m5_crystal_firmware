// pid PID::FLASHLIGHT_MENU

void flashLightMenuLoop() {
	MENU flashLightMenu[] = {
		{PID::OTHER,              L->MENU_BACK, Icons::back},
		{PID::FLASHLIGHT,         L->MENU_OTHER_FLASHLIGHT, Icons::flashlight},
		{PID::FLASHLIGHT_FLICKER, L->MENU_OTHER_FLICKER, Icons::flashlight},
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
