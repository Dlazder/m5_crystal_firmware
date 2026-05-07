// pid PID::SETTINGS

void settingsMenuLoop() {
	MENU settingsMenu[] = {
		{PID::MAIN_MENU,           L->MENU_BACK},
		{PID::SETTINGS_CLOCK,      L->MENU_SETTINGS_TIME},
		{PID::BRIGHTNESS,          L->MENU_SETTINGS_BRIGHTNESS},
		{PID::ROTATION,            L->MENU_SETTINGS_ROTATION},
		{PID::COLORS,              L->MENU_SETTINGS_COLORS},
		{PID::SETTINGS_STATUS_BAR, L->MENU_SETTINGS_STATUS_BAR},
		{PID::STARTUP_SOUND,       L->MENU_SETTINGS_STARTUP_SOUND},
		// {PID::FONTS,               L->MENU_SETTINGS_FONT},
		{PID::KB_GYRO,             L->MENU_SETTINGS_KB_GYRO},
		{PID::LANGUAGE,            L->MENU_SETTINGS_LANGUAGE},
	};
	int settingsMenuSize = sizeof(settingsMenu) / sizeof(MENU);

	if (isSetup()) {
		DISP.setTextSize(MEDIUM_TEXT);
		cursorOnTop();
		if (previousProcess == PID::MAIN_MENU) cursor = 0;
		drawMenu(settingsMenu, settingsMenuSize);
	}
	menuLoop(settingsMenu, settingsMenuSize);
}
