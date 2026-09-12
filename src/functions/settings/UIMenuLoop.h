// PID::SETTINGS_UI

void settingsUIMenuLoop() {
	MENU settingsUIMenu[] = {
		{PID::SETTINGS, L->MENU_BACK, Icons::back},
		{PID::COLORS, L->MENU_SETTINGS_COLORS},
		{PID::FONTS, L->MENU_SETTINGS_FONT},
		{PID::SETTINGS_STATUS_BAR, L->MENU_SETTINGS_STATUS_BAR},
		{PID::PLACEHOLDER, L->MENU_SETTINGS_ICONS, nullptr, "icons"},
		{PID::BRIGHTNESS, L->MENU_SETTINGS_BRIGHTNESS},
		{PID::DIMMING, L->MENU_SETTINGS_DIMMING, Icons::sleep},
		{PID::PLACEHOLDER, L->MENU_SETTINGS_ROTATION, Icons::rotation, "rotation"},
	};
	int settingsUIMenuSize = sizeof(settingsUIMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(settingsUIMenu, settingsUIMenuSize);
	}
	menuLoop(settingsUIMenu, settingsUIMenuSize);
}