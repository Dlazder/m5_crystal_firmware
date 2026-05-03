// pid PID::WIFI_SELECTED

MENU selectedWifiMenu[] = {
	{PID::WIFI_SCAN, "back"},
	{PID::WIFI_INFO, "info"},
	{PID::WIFI_DEAUTH, "deauth"},
	{PID::WIFI_CONNECT, "connect"}
};
int selectedWifiMenuSize = sizeof(selectedWifiMenu) / sizeof(MENU);

void selectedWifiMenuLoop() {
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
		cursorOnTop();
		drawMenu(selectedWifiMenu, selectedWifiMenuSize);
	}
	menuLoop(selectedWifiMenu, selectedWifiMenuSize);
}
