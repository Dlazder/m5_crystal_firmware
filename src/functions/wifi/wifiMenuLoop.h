// pid PID::WIFI

void wifiMenuLoop() {
	MENU wifiMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK, Icons::back},
		{PID::WIFI_SCAN, L->MENU_SCAN, Icons::find},
		{PID::WEATHER, L->MENU_WIFI_WEATHER},
		{PID::WIFI_TIME_SYNC, L->MENU_WIFI_SYNC_TIME},
		{PID::WIFI_DISCONNECT, L->MENU_WIFI_DISCONNECT},
		{PID::WIFI_WPA_BF_ALL, L->MENU_WIFI_WPA_BF_ALL},
		{PID::WIFI_DEAUTH_ALL, L->MENU_WIFI_DEAUTH_ALL},
		{PID::EVIL_PORTAL, L->MENU_WIFI_EVIL_PORTAL},
		{PID::WEB_SERVER, L->MENU_WIFI_WEB_SERVER},
		{PID::WIFI_PCAP_TO_HASH, "pcap -> FT Hash"},
		{PID::WIFI_CHANNEL_ANALYZER, L->MENU_WIFI_CHANNEL_ANALYZER}
	};
	int wifiMenuSize = sizeof(wifiMenu) / sizeof(MENU);

	if (isSetup()) {
		drawMenu(wifiMenu, wifiMenuSize);
	}
	menuLoop(wifiMenu, wifiMenuSize);
}
