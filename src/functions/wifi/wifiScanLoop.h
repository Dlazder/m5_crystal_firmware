// pid PID::WIFI_SCAN

int wifiCount = 0;
MENU* wifiScanMenu = nullptr;
static bool _wifiScanning = false;

void wifiScanLoop() {
	if (isSetup()) {
		// Start a fresh scan if needed and no scan is already running
		// (a background scan may have been left from a previous session).
		if ((wifiCount == 0 || previousProcess == PID::WIFI_SCAN) && !_wifiScanning) {
			delete[] wifiScanMenu;
			wifiScanMenu = nullptr;
			cursor = 0;
			WiFi.scanDelete();
			Serial.println("Scanning...");
			WiFi.scanNetworks(true);
			_wifiScanning = true;
		}

		// If not scanning, draw the menu immediately (cached results)
		if (!_wifiScanning && wifiScanMenu != nullptr) {
			drawMenu(wifiScanMenu, wifiCount + 2);
		}
	}

	// Async scan polling
	if (_wifiScanning) {
		int result = WiFi.scanComplete();

		if (result == WIFI_SCAN_RUNNING) {
			centeredPrint(L->TXT_SCANNING, MEDIUM_TEXT, true);
			drawSpinner();
			canvas.pushSprite(0, getStatusBarHeight());

			// Exit leaves the scan running in background.
			// Results will be available on next entry.
			if (checkExit(PID::WIFI)) {
				return;
			}
			return;
		}

		// Scan completed
		_wifiScanning = false;
		wifiCount = (result >= 0) ? result : 0;

		// Build menu from scan results
		if (wifiScanMenu != nullptr) {
			delete[] wifiScanMenu;
		}
		wifiScanMenu = new MENU[wifiCount + 2];
		wifiScanMenu[0].name = L->MENU_BACK;
		wifiScanMenu[0].command = PID::WIFI;
		wifiScanMenu[0].icon = Icons::back;
		wifiScanMenu[1].name = L->MENU_RESCAN;
		wifiScanMenu[1].command = PID::WIFI_SCAN;
		wifiScanMenu[1].icon = nullptr;
		for (int i = 0; i < wifiCount; i++) {
			wifiScanMenu[i + 2].name = WiFi.SSID(i);
			wifiScanMenu[i + 2].command = PID::WIFI_SELECTED;
			wifiScanMenu[i + 2].icon = nullptr;
		}

		cursor = 0;
		drawMenu(wifiScanMenu, wifiCount + 2);
	}

	// ---- Menu navigation ----
	if (!_wifiScanning && wifiScanMenu != nullptr) {
		menuLoop(wifiScanMenu, wifiCount + 2);
	}
}
