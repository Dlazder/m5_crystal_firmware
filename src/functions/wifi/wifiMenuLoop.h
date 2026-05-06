// pid PID::WIFI

void wifiMenuLoop() {
	MENU wifiMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK},
		{PID::WIFI_SCAN, L->MENU_SCAN},
	};
	int wifiMenuSize = sizeof(wifiMenu) / sizeof(MENU);

	if (isSetup()) {
		cursorOnTop();
		drawMenu(wifiMenu, wifiMenuSize);
	}
	menuLoop(wifiMenu, wifiMenuSize);
}
