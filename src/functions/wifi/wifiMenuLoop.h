// pid PID::WIFI

void wifiMenuLoop() {
	MENU wifiMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK},
		{PID::WIFI_SCAN, L->MENU_SCAN},
		{PID::WEATHER, L->MENU_WIFI_WEATHER},
		{PID::WIFI_TIME_SYNC, L->MENU_WIFI_SYNC_TIME},
		{PID::WIFI_DISCONNECT, L->MENU_WIFI_DISCONNECT},
		{PID::WIFI_WPA_BF_ALL, L->MENU_WIFI_WPA_BF_ALL}
	};
	int wifiMenuSize = sizeof(wifiMenu) / sizeof(MENU);

	if (isSetup()) {
		drawMenu(wifiMenu, wifiMenuSize);
	}
	menuLoop(wifiMenu, wifiMenuSize);
}
