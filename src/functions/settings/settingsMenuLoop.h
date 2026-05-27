// pid PID::SETTINGS

void settingsMenuLoop() {
	MENU settingsMenu[] = {
		{PID::MAIN_MENU,           L->MENU_BACK},
		{PID::SETTINGS_CLOCK,      L->MENU_SETTINGS_TIME},
		{PID::BRIGHTNESS,          L->MENU_SETTINGS_BRIGHTNESS},
		{PID::DIMMING,             L->MENU_SETTINGS_DIMMING},
		{PID::ROTATION,            L->MENU_SETTINGS_ROTATION},
		{PID::COLORS,              L->MENU_SETTINGS_COLORS},
		{PID::SETTINGS_STATUS_BAR, L->MENU_SETTINGS_STATUS_BAR},
		{PID::STARTUP_SOUND,       L->MENU_SETTINGS_STARTUP_SOUND},
		{PID::VOLUME,              L->MENU_SETTINGS_VOLUME},
		{PID::FONTS,               L->MENU_SETTINGS_FONT},
		{PID::LANGUAGE,            L->MENU_SETTINGS_LANGUAGE},
		{PID::KB_GYRO,             L->MENU_SETTINGS_KB_GYRO},
	};
	int settingsMenuSize = sizeof(settingsMenu) / sizeof(MENU);

	if (isSetup()) {
		DISP.setTextSize(MEDIUM_TEXT);
		if (previousProcess == PID::MAIN_MENU) cursor = 0;
		drawMenu(settingsMenu, settingsMenuSize);
	}
	menuLoop(settingsMenu, settingsMenuSize);
}
