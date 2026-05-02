// pid PID::WIFI_SCAN

int wifiCount = 0;
MENU* wifiScanMenu = nullptr;
void wifiScanLoop() {
	if (isSetup()) {
		if (wifiCount == 0 || isMenuItemSelected(PID::WIFI_SCAN)) {
			// clear memory
			if (wifiScanMenu != nullptr) {
				delete[] wifiScanMenu;
			}
			cursor = 0;
			centeredPrint("Scanning...", SMALL_TEXT);
			Serial.println("Scanning...");
			wifiCount = WiFi.scanNetworks();
			wifiScanMenu = new MENU[wifiCount + 2];
			wifiScanMenu[0].name = "back";
			wifiScanMenu[0].command = PID::WIFI;
			wifiScanMenu[1].name = "rescan";
			wifiScanMenu[1].command = PID::WIFI_SCAN;
			for (int i = 0; i < wifiCount; i++) {
				wifiScanMenu[i + 2].name = WiFi.SSID(i).substring(0, 14);
				wifiScanMenu[i + 2].command = PID::WIFI_SELECTED;
			}
		}
		cursorOnTop();
		drawMenu(wifiScanMenu, wifiCount + 2);
	}
	menuLoop(wifiScanMenu, wifiCount + 2, true);
}
