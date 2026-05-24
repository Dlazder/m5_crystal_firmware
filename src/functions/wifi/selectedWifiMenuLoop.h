// pid PID::WIFI_SELECTED

void selectedWifiMenuLoop() {
	MENU selectedWifiMenu[] = {
		{PID::WIFI_SCAN,    L->MENU_BACK},
		{PID::WIFI_INFO,    L->MENU_WIFI_INFO},
		{PID::WIFI_DEAUTH,  L->MENU_WIFI_DEAUTH},
		{PID::WIFI_CONNECT, L->MENU_WIFI_CONNECT},
	};
	int selectedWifiMenuSize = sizeof(selectedWifiMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::WIFI_SCAN) {
			wifiScanIndex = cursor - 2;
			ssid = WiFi.SSID(wifiScanIndex);
			mac = WiFi.BSSIDstr(wifiScanIndex);
			channel = WiFi.channel(wifiScanIndex);
			bssid = WiFi.BSSID(wifiScanIndex);
			rssi = WiFi.RSSI(wifiScanIndex);
			cursor = 0;
		}
		drawMenu(selectedWifiMenu, selectedWifiMenuSize);
	}
	menuLoop(selectedWifiMenu, selectedWifiMenuSize);
}
