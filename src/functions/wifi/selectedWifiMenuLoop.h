// pid PID::WIFI_SELECTED

MENU selectedWifiMenu[] = {
	{PID::WIFI_SCAN, "back"},
	{PID::WIFI_INFO, "info"},
	{PID::WIFI_DEAUTH, "deauth"}
};
int selectedWifiMenuSize = sizeof(selectedWifiMenu) / sizeof(MENU);

void selectedWifiMenuLoop() {
	if (isSetup()) {
		if (previousProcess == PID::WIFI_SCAN) {
			ssid = WiFi.SSID(cursor - 2);
			mac = WiFi.BSSIDstr(cursor - 2);
			channel = WiFi.channel(cursor - 2);
			bssid = WiFi.BSSID(cursor - 2);
			rssi = WiFi.RSSI(cursor - 2);
			cursor = 0;
		}
		cursorOnTop();
		drawMenu(selectedWifiMenu, selectedWifiMenuSize);
	}
	menuLoop(selectedWifiMenu, selectedWifiMenuSize);
}
