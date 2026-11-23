// PID::SETTINGS_SYSTEM

void settingsSystemMenuLoop() {
	MENU settingsSystemMenu[] = {
		{PID::SETTINGS,       L->MENU_BACK, Icons::back},
		{PID::SETTINGS,       L->MENU_SETTINGS_LANGUAGE, Icons::language, "languageIndex"},
		{PID::SETTINGS_CLOCK, L->MENU_SETTINGS_TIME, Icons::clock},
		{PID::PLACEHOLDER,    L->MENU_SETTINGS_TIMEZONE, Icons::clock, "timezone"},
		{PID::PLACEHOLDER,    L->MENU_SETTINGS_STARTUP_SOUND, Icons::volume, "startupSound"},
		{PID::PLACEHOLDER,    L->MENU_SETTINGS_VOLUME, Icons::volume, "volume"},
	};
	int settingsSystemMenuSize = sizeof(settingsSystemMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(settingsSystemMenu, settingsSystemMenuSize);
	}
	menuLoop(settingsSystemMenu, settingsSystemMenuSize);
}
