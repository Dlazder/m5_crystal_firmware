// PID::BT_SELECTED

void selectedBluetoothMenuLoop() {
	MENU selectedBluetoothMenu[] = {
		{PID::BT_SCAN, L->MENU_BACK, Icons::back},
		{PID::BT_INFO, L->MENU_BT_INFO, Icons::info},
		{PID::BT_FINDER, L->MENU_BT_FIND, Icons::find},
	};
	int selectedBluetoothMenuSize = sizeof(selectedBluetoothMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::BT_SCAN) {
			int idx = cursor - 2;
			BLEScanResults results = BLEDevice::getScan()->getResults();
			BLEAdvertisedDevice device = results.getDevice(idx);
			String devName = device.getName().c_str();
			Ble::selected.address = device.getAddress().toString().c_str();
			Ble::selected.rssi = device.getRSSI();
			Ble::selected.name = (device.haveName() && devName.length() > 0) ? devName : L->TXT_UNNAMED;

			Ble::selected.manufacturer = "";
			Ble::selected.manufacturerRaw = "";
			Ble::selected.txPower = -128;
			Ble::selected.advFlags = 0xFF;
			Ble::selected.serviceUUIDs = "";
			Ble::selected.appearance = "";

			if (device.haveManufacturerData()) {
				std::string md = device.getManufacturerData();
				if (md.size() >= 2) {
					uint16_t companyId = (uint8_t)md[0] | ((uint8_t)md[1] << 8);
					Ble::selected.manufacturer = Ble::manufacturerName(companyId);
					String raw = "";
					for (size_t i = 2; i < md.size() && i < 10; i++) {
						char h[3];
						sprintf(h, "%02X", (uint8_t)md[i]);
						raw += h;
					}
					Ble::selected.manufacturerRaw = raw;
				}
			}

			if (device.haveTXPower()) Ble::selected.txPower = device.getTXPower();
			if (device.haveAppearance()) Ble::selected.appearance = "app:0x" + String(device.getAppearance(), HEX);

			Ble::selected.serviceUUIDs = "";
			if (device.haveServiceUUID()) {
				int count = device.getServiceUUIDCount();
				for (int s = 0; s < count && s < 4; s++) {
					BLEUUID uuid = device.getServiceUUID(s);
					String uuidStr = uuid.toString().c_str();
					if (uuidStr.length() > 8) uuidStr = uuidStr.substring(uuidStr.length() - 9);
					if (Ble::selected.serviceUUIDs.length() > 0) Ble::selected.serviceUUIDs += " ";
					Ble::selected.serviceUUIDs += uuidStr;
				}
			}

			cursor = 0;
		}
		drawMenu(selectedBluetoothMenu, selectedBluetoothMenuSize);
	}
	menuLoop(selectedBluetoothMenu, selectedBluetoothMenuSize);
}
