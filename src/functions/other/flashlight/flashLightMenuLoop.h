// PID::FLASHLIGHT_MENU

void flashLightMenuLoop() {
	MENU flashLightMenu[] = {
		{PID::OTHER,              L->MENU_BACK, Icons::back},
		{PID::FLASHLIGHT,         L->MENU_OTHER_FLASHLIGHT, Icons::flashlight},
		{PID::FLASHLIGHT_FLICKER, L->MENU_OTHER_FLICKER, Icons::flashlight},
		{PID::FLASHLIGHT_MENU,    L->MENU_SETTINGS_BRIGHTNESS, nullptr, "brightness"},
		{PID::FLASHLIGHT_MENU,    L->MENU_SETTINGS_COLORS, nullptr, "colorIndex"},
	};
	int flashLightMenuSize = sizeof(flashLightMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(flashLightMenu, flashLightMenuSize);
	}
	menuLoop(flashLightMenu, flashLightMenuSize);
}
