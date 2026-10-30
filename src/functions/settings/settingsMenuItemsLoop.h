// PID::SETTINGS_MENU_ITEMS

void settingsMenuItemsLoop() {
	MENU settingsMenuItemsMenu[] = {
		{PID::SETTINGS_UI, L->MENU_BACK, Icons::back},
		{PID::PLACEHOLDER, L->MENU_WIFI, Icons::view, "showMenuWifi"},
		{PID::PLACEHOLDER, L->MENU_BT, Icons::view, "showMenuBt"},
		{PID::PLACEHOLDER, L->MENU_NFC, Icons::view, "showMenuNfc"},
		{PID::PLACEHOLDER, L->MENU_IR, Icons::view, "showMenuIr"},
		{PID::PLACEHOLDER, L->MENU_USB, Icons::view, "showMenuUsb"},
		{PID::PLACEHOLDER, "RF", Icons::view, "showMenuRf"},
		{PID::PLACEHOLDER, "NRF", Icons::view, "showMenuNrf"},
		{PID::PLACEHOLDER, L->MENU_UART, Icons::view, "showMenuUart"},
		{PID::PLACEHOLDER, L->MENU_FILES, Icons::view, "showMenuFiles"},
		{PID::PLACEHOLDER, L->MENU_OTHER, Icons::view, "showMenuOther"},
	};
	int settingsMenuItemsMenuSize = sizeof(settingsMenuItemsMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(settingsMenuItemsMenu, settingsMenuItemsMenuSize);
	}
	menuLoop(settingsMenuItemsMenu, settingsMenuItemsMenuSize);
}
