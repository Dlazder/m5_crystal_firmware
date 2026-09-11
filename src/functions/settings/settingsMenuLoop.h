// PID::SETTINGS

void settingsMenuLoop() {
	MENU settingsMenu[] = {
		{PID::MAIN_MENU,           L->MENU_BACK, Icons::back},
		{PID::SETTINGS_UI,         L->MENU_SETTINGS_UI, Icons::personalization},
		{PID::STARTUP_SOUND,       L->MENU_SETTINGS_STARTUP_SOUND, Icons::volume, "startupSound"},
		{PID::VOLUME,              L->MENU_SETTINGS_VOLUME, Icons::volume},
		{PID::LANGUAGE,            L->MENU_SETTINGS_LANGUAGE},
		{PID::KB_GYRO,             L->MENU_SETTINGS_KB_GYRO, nullptr, "kbGyro"},
		{PID::WEBSERVER_FS,        L->MENU_SETTINGS_WEBSERVER_FS, nullptr, "webServerFs"},
		{PID::SETTINGS_SSID,       L->MENU_SETTINGS_WIFI_SSID},
		{PID::SETTINGS_CLOCK,      L->MENU_SETTINGS_TIME, Icons::clock},
		{PID::SETTINGS_TIMEZONE,   L->MENU_SETTINGS_TIMEZONE, Icons::clock},
	};
	int settingsMenuSize = sizeof(settingsMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::MAIN_MENU || previousProcess == PID::SETTINGS_UI) cursor = 0;
		drawMenu(settingsMenu, settingsMenuSize);
	}
	menuLoop(settingsMenu, settingsMenuSize);
}
