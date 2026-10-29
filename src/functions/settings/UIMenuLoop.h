// PID::SETTINGS_UI

void settingsUIMenuLoop() {
	MENU settingsUIMenu[] = {
		{PID::SETTINGS, L->MENU_BACK, Icons::back},
		{PID::SETTINGS_UI, L->MENU_SETTINGS_COLORS, nullptr, "colorIndex"},
		{PID::SETTINGS_UI, L->MENU_SETTINGS_FONT, nullptr, "fontIndex"},
		{PID::SETTINGS_MENU_ITEMS, L->MENU_SETTINGS_CUSTOMIZE_MENU, Icons::view},
		{PID::SETTINGS_STATUS_BAR, L->MENU_SETTINGS_STATUS_BAR},
		{PID::PLACEHOLDER, L->MENU_SETTINGS_ICONS, nullptr, "icons"},
		{PID::SETTINGS_UI, L->MENU_SETTINGS_BRIGHTNESS, nullptr, "brightness"},
		{PID::SETTINGS_UI, L->MENU_SETTINGS_DIMMING, Icons::sleep, "dimTimeout"},
		{PID::PLACEHOLDER, L->MENU_SETTINGS_ROTATION, Icons::rotation, "rotation"},
	};
	int settingsUIMenuSize = sizeof(settingsUIMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(settingsUIMenu, settingsUIMenuSize);
	}
	menuLoop(settingsUIMenu, settingsUIMenuSize);
}