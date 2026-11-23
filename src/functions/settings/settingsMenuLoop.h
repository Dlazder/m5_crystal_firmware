// PID::SETTINGS

void settingsMenuLoop() {
	MENU settingsMenu[] = {
		{PID::MAIN_MENU,           L->MENU_BACK, Icons::back},
		{PID::SETTINGS_UI,         L->MENU_SETTINGS_UI, Icons::personalization},
		{PID::SETTINGS_SYSTEM,     L->MENU_SETTINGS_SYSTEM, Icons::settings},
		{PID::PLACEHOLDER,         L->MENU_SETTINGS_KB_GYRO, Icons::keyboard, "kbGyro"},
		{PID::SETTINGS_SSID,       L->MENU_SETTINGS_WIFI_SSID, Icons::wifi},
		{PID::PLACEHOLDER,         L->MENU_SETTINGS_WEBSERVER_FS, Icons::folder, "webServerFs"},
		{PID::PLACEHOLDER,         L->MENU_SETTINGS_SD_ROOT, Icons::sd, "sdMountCrystal"},
	};
	int settingsMenuSize = sizeof(settingsMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::MAIN_MENU || previousProcess == PID::SETTINGS_UI) cursor = 0;
		drawMenu(settingsMenu, settingsMenuSize);
	}
	menuLoop(settingsMenu, settingsMenuSize);
}
