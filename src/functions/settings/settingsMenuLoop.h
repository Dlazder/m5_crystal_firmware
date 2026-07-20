// pid PID::SETTINGS

void settingsMenuLoop() {
	MENU settingsMenu[] = {
		{PID::MAIN_MENU,           L->MENU_BACK, Icons::back},
		{PID::SETTINGS_CLOCK,      L->MENU_SETTINGS_TIME},
		{PID::BRIGHTNESS,          L->MENU_SETTINGS_BRIGHTNESS},
		{PID::DIMMING,             L->MENU_SETTINGS_DIMMING},
		{PID::ROTATION,            L->MENU_SETTINGS_ROTATION, nullptr, "rotation"},
		{PID::COLORS,              L->MENU_SETTINGS_COLORS},
		{PID::ICONS,               L->MENU_SETTINGS_ICONS, nullptr, "icons"},
		{PID::SETTINGS_STATUS_BAR, L->MENU_SETTINGS_STATUS_BAR},
		{PID::STARTUP_SOUND,       L->MENU_SETTINGS_STARTUP_SOUND, nullptr, "startupSound"},
		{PID::VOLUME,              L->MENU_SETTINGS_VOLUME},
		{PID::FONTS,               L->MENU_SETTINGS_FONT},
		{PID::LANGUAGE,            L->MENU_SETTINGS_LANGUAGE},
		{PID::KB_GYRO,             L->MENU_SETTINGS_KB_GYRO, nullptr, "kbGyro"},
		{PID::WEBSERVER_FS,        L->MENU_SETTINGS_WEBSERVER_FS, nullptr, "webServerFs"},
		{PID::SETTINGS_SSID,        L->MENU_SETTINGS_WIFI_SSID},
	};
	int settingsMenuSize = sizeof(settingsMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::MAIN_MENU) cursor = 0;
		drawMenu(settingsMenu, settingsMenuSize);
	}
	menuLoop(settingsMenu, settingsMenuSize);
}
