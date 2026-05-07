// pid PID::BT_SELECTED

void selectedBluetoothMenuLoop() {
	MENU selectedBluetoothMenu[] = {
		{PID::BT_SCAN,   L->MENU_BACK},
		{PID::BT_INFO,   L->MENU_BT_INFO},
		{PID::BT_FINDER, L->MENU_BT_FIND},
	};
	int selectedBluetoothMenuSize = sizeof(selectedBluetoothMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::BT_SCAN) {
			int idx = cursor - 2;
			BLEScanResults results = BLEDevice::getScan()->getResults();
			BLEAdvertisedDevice device = results.getDevice(idx);
			String devName = device.getName().c_str();
			bleAddress = device.getAddress().toString().c_str();
			bleRssi = device.getRSSI();
			bleName = (device.haveName() && devName.length() > 0) ? devName : L->TXT_UNNAMED;

			bleManufacturer = "";
			bleManufacturerRaw = "";
			bleTxPower = -128;
			bleAdvFlags = 0xFF;
			bleServiceUUIDs = "";
			bleAppearance = "";

			if (device.haveManufacturerData()) {
				std::string md = device.getManufacturerData();
				if (md.size() >= 2) {
					uint16_t companyId = (uint8_t)md[0] | ((uint8_t)md[1] << 8);
					switch (companyId) {
						case 0x004C: bleManufacturer = "Apple"; break;
						case 0x0006: bleManufacturer = "Microsoft"; break;
						case 0x0075: bleManufacturer = "Samsung"; break;
						case 0x00E0: bleManufacturer = "Google"; break;
						case 0x0157: bleManufacturer = "Garmin"; break;
						case 0x0059: bleManufacturer = "Nordic Semi"; break;
						case 0x0499: bleManufacturer = "Ruuvi"; break;
						case 0x0087: bleManufacturer = "Garmin"; break;
						case 0x0010: bleManufacturer = "Mitsumi"; break;
						case 0x0001: bleManufacturer = "Ericsson"; break;
						case 0x0002: bleManufacturer = "Nokia"; break;
						case 0x000F: bleManufacturer = "Broadcom"; break;
						case 0x004F: bleManufacturer = "M-Way"; break;
						case 0x0171: bleManufacturer = "Amazon"; break;
						case 0x0183: bleManufacturer = "Jabra"; break;
						case 0x00D2: bleManufacturer = "Xiaomi"; break;
						case 0x038F: bleManufacturer = "Amazfit"; break;
						case 0x0103: bleManufacturer = "Huawei"; break;
						case 0x02D5: bleManufacturer = "Espressif"; break;
						case 0x008C: bleManufacturer = "Sony"; break;
						case 0x0046: bleManufacturer = "Philips"; break;
						default: {
							char buf[8];
							sprintf(buf, "0x%04X", companyId);
							bleManufacturer = buf;
							break;
						}
					}
					String raw = "";
					for (size_t i = 2; i < md.size() && i < 10; i++) {
						char h[3];
						sprintf(h, "%02X", (uint8_t)md[i]);
						raw += h;
					}
					bleManufacturerRaw = raw;
				}
			}

			if (device.haveTXPower()) bleTxPower = device.getTXPower();
			if (device.haveAppearance()) bleAppearance = "app:0x" + String(device.getAppearance(), HEX);

			bleServiceUUIDs = "";
			if (device.haveServiceUUID()) {
				int count = device.getServiceUUIDCount();
				for (int s = 0; s < count && s < 4; s++) {
					BLEUUID uuid = device.getServiceUUID(s);
					String uuidStr = uuid.toString().c_str();
					if (uuidStr.length() > 8) uuidStr = uuidStr.substring(uuidStr.length() - 9);
					if (bleServiceUUIDs.length() > 0) bleServiceUUIDs += " ";
					bleServiceUUIDs += uuidStr;
				}
			}

			cursor = 0;
		}
		cursorOnTop();
		drawMenu(selectedBluetoothMenu, selectedBluetoothMenuSize);
	}
	menuLoop(selectedBluetoothMenu, selectedBluetoothMenuSize);
}
