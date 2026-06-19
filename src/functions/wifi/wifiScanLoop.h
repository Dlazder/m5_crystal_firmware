// pid PID::WIFI_SCAN

int wifiCount = 0;
MENU* wifiScanMenu = nullptr;
void wifiScanLoop() {
	if (isSetup()) {
		// Scanning if:
		// If wifiCount = 0 that means we are scanning for the first time or no networks was found last time
		// If previous process was WIFI_SCAN that means we rescan networks.
		if (wifiCount == 0 || previousProcess == PID::WIFI_SCAN) {
			if (wifiScanMenu != nullptr) {
				delete[] wifiScanMenu;
			}
			cursor = 0;
			centeredPrint(L->TXT_SCANNING, MEDIUM_TEXT);
			Serial.println("Scanning...");
			wifiCount = WiFi.scanNetworks();
			wifiScanMenu = new MENU[wifiCount + 2];
			wifiScanMenu[0].name = L->MENU_BACK;
			wifiScanMenu[0].command = PID::WIFI;
			wifiScanMenu[1].name = L->MENU_RESCAN;
			wifiScanMenu[1].command = PID::WIFI_SCAN;
			for (int i = 0; i < wifiCount; i++) {
				wifiScanMenu[i + 2].name = WiFi.SSID(i);
				wifiScanMenu[i + 2].command = PID::WIFI_SELECTED;
			}
		}
		drawMenu(wifiScanMenu, wifiCount + 2);
	}
	menuLoop(wifiScanMenu, wifiCount + 2);
}
