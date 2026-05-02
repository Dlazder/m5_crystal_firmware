// pid PID::WIFI

MENU wifiMenu[] = {
	{PID::MAIN_MENU, "back"},
	// {PID::WIFI_ACCESS_POINT, "Wi-Fi AP"},
	{PID::WIFI_SCAN, "scan"}
};

int wifiMenuSize = sizeof(wifiMenu) / sizeof(MENU);

void wifiMenuLoop() {
	if (isSetup()) {
		cursorOnTop();
		drawMenu(wifiMenu, wifiMenuSize);
	}
	menuLoop(wifiMenu, wifiMenuSize);
}
