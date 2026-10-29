// PID::WIFI_SELECTED

void selectedWifiMenuLoop() {
	MENU selectedWifiMenu[] = {
		{PID::WIFI_SCAN, L->MENU_BACK, Icons::back},
		{PID::WIFI_INFO, L->MENU_WIFI_INFO, Icons::info},
		{PID::WIFI_DEAUTH, L->MENU_WIFI_DEAUTH},
		{PID::WIFI_CONNECT, L->MENU_WIFI_CONNECT},
		{PID::WIFI_WPA_BF, L->MENU_WIFI_WPA_BF},
		{PID::WIFI_HANDSHAKE_CAPTURE, L->MENU_WIFI_HANDSHAKE},
		{PID::EVIL_TWIN, L->MENU_WIFI_EVIL_TWIN},
		{PID::WIFI_PMKID_CAPTURE, L->MENU_WIFI_PMKID},
		{PID::WIFI_WPS_PBC, L->MENU_WIFI_WPS_PBC},
	};
	int selectedWifiMenuSize = sizeof(selectedWifiMenu) / sizeof(selectedWifiMenu[0]);

	if (isSetup()) {
		if (previousProcess == PID::WIFI_SCAN) {
			Wifi::selected.scanIndex = cursor - 2;
			Wifi::selected.ssid = WiFi.SSID(Wifi::selected.scanIndex);
			Wifi::selected.mac = WiFi.BSSIDstr(Wifi::selected.scanIndex);
			Wifi::selected.channel = WiFi.channel(Wifi::selected.scanIndex);
			Wifi::selected.bssid = WiFi.BSSID(Wifi::selected.scanIndex);
			Wifi::selected.rssi = WiFi.RSSI(Wifi::selected.scanIndex);
			Wifi::selected.authMode = WiFi.encryptionType(Wifi::selected.scanIndex);
			Wifi::selected.hasWps = false;
			Wifi::selected.hasFtPsk = false;
		}
		cursor = 0;
		drawMenu(selectedWifiMenu, selectedWifiMenuSize);
	}
	menuLoop(selectedWifiMenu, selectedWifiMenuSize);
}
