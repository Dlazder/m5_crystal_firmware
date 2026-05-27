// pid PID::BT_SCAN

int bleCount = 0;
MENU* bleScanMenu = nullptr;

void bluetoothScanLoop() {
	if (isSetup()) {
		// Scanning if:
		// If bleCount = 0 that means we are scanning for the first time or no devices was found last time
		// If previous process was BT_SCAN that means we rescan devices.
		if (bleCount == 0 || previousProcess == PID::BT_SCAN) {
			if (bleScanMenu != nullptr) {
				delete[] bleScanMenu;
				bleScanMenu = nullptr;
			}
			cursor = 0;
			centeredPrint(L->TXT_SCANNING, MEDIUM_TEXT);
			Serial.println("BLE Scanning...");

			BLEDevice::init("");

			BLEScan* pScan = BLEDevice::getScan();
			pScan->setActiveScan(true);
			pScan->setInterval(100);
			pScan->setWindow(99);

			pScan->clearResults();

			BLEScanResults results = pScan->start(5, false);
			bleCount = results.getCount();

			bleScanMenu = new MENU[bleCount + 2];
			bleScanMenu[0].name = L->MENU_BACK;
			bleScanMenu[0].command = PID::BLUETOOTH;
			bleScanMenu[1].name = L->MENU_RESCAN;
			bleScanMenu[1].command = PID::BT_SCAN;

			for (int i = 0; i < bleCount; i++) {
				BLEAdvertisedDevice device = results.getDevice(i);
				Serial.printf("Device %d; name: %s; MAC: %s\n", i, device.getName().c_str(), device.getAddress().toString().c_str());
				String devName = device.getName().c_str();
				String name = (device.haveName() && devName.length() > 0)
					? devName.substring(0, 14)
					: String(device.getAddress().toString().substr(0, 14).c_str());

				bleScanMenu[i + 2].name = name;
				bleScanMenu[i + 2].command = PID::BT_SELECTED;
			}
		}
		drawMenu(bleScanMenu, bleCount + 2);
	}
	menuLoop(bleScanMenu, bleCount + 2);
}
