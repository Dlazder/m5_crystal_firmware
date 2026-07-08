// PID::WIFI_SELECTED

void selectedWifiMenuLoop() {
	MENU selectedWifiMenu[] = {
		{PID::WIFI_SCAN, L->MENU_BACK},
		{PID::WIFI_INFO, L->MENU_WIFI_INFO},
		{PID::WIFI_DEAUTH, L->MENU_WIFI_DEAUTH},
		{PID::WIFI_CONNECT, L->MENU_WIFI_CONNECT},
		{PID::WIFI_WPA_BF, L->MENU_WIFI_WPA_BF},
		{PID::WIFI_HANDSHAKE_CAPTURE, L->MENU_WIFI_HANDSHAKE},
		{PID::EVIL_TWIN, L->MENU_WIFI_EVIL_TWIN},
	};
	int selectedWifiMenuSize = sizeof(selectedWifiMenu) / sizeof(selectedWifiMenu[0]);

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
