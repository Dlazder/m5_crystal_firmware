// pid PID::BT_SNIFFER

// Live BLE advertising traffic monitor.
//
// The ESP32 radio cannot capture raw/arbitrary Bluetooth packets (no promiscuous
// mode like an Ubertooth). What it can do is run a continuous BLE scan and surface
// every advertising / scan-response packet as it arrives. This loop shows that
// traffic live: a scrolling log of nearby BLE packets with MAC, RSSI, packet type
// and a short payload preview, plus a running packet counter.

#define BT_SNIFFER_LOG_ROWS 8

struct BleSniffEntry {
	String mac;
	String label;   // device name or manufacturer, short
	int rssi;
	char type;      // 'N' = named, 'M' = manufacturer data, 'S' = service adv, '.' = bare
};

static BleSniffEntry bleSniffLog[BT_SNIFFER_LOG_ROWS];
static int bleSniffLogCount = 0; // how many rows currently filled (<= BT_SNIFFER_LOG_ROWS)
static unsigned long bleSniffTotal = 0; // total packets observed this session
static volatile bool bleSniffDirty = false;
static BLEScan* bleSniffScan = nullptr;

// Push a new packet to the top of the ring-ish log (newest first).
static void bleSniffPush(const BleSniffEntry& e) {
	for (int i = BT_SNIFFER_LOG_ROWS - 1; i > 0; i--) {
		bleSniffLog[i] = bleSniffLog[i - 1];
	}
	bleSniffLog[0] = e;
	if (bleSniffLogCount < BT_SNIFFER_LOG_ROWS) bleSniffLogCount++;
	bleSniffTotal++;
	bleSniffDirty = true;
}

class BleSniffCallbacks : public BLEAdvertisedDeviceCallbacks {
	void onResult(BLEAdvertisedDevice device) {
		BleSniffEntry e;
		e.mac = device.getAddress().toString().c_str();
		e.rssi = device.getRSSI();

		// Classify the packet by what its advertising payload carries. The BLE
		// stack here doesn't expose the raw ADV/SCAN_RSP PDU type, so we tag by
		// content: named > manufacturer-data > service-adv > bare.
		String devName = device.getName().c_str();
		if (device.haveName() && devName.length() > 0) {
			e.type = 'N';
			e.label = devName.substring(0, 12);
		} else if (device.haveManufacturerData()) {
			e.type = 'M';
			std::string md = device.getManufacturerData();
			if (md.size() >= 2) {
				uint16_t companyId = (uint8_t)md[0] | ((uint8_t)md[1] << 8);
				char buf[10];
				sprintf(buf, "m:%04X", companyId);
				e.label = buf;
			} else {
				e.label = "";
			}
		} else if (device.haveServiceUUID()) {
			e.type = 'S';
			String u = device.getServiceUUID().toString().c_str();
			if (u.length() > 8) u = u.substring(u.length() - 5);
			e.label = "s:" + u;
		} else {
			e.type = '.';
			e.label = "";
		}

		bleSniffPush(e);
	}
};

static BleSniffCallbacks bleSniffCb;

static void drawSnifferScreen() {
	canvas.clear();

	// Header: packet counter only.
	canvas.setTextSize(SMALL_TEXT);
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setCursor(0, 0);
	canvas.println("pkt:" + String(bleSniffTotal));

	int y = canvas.getCursorY() + 2;
	canvas.setTextSize(TINY_TEXT);

	if (bleSniffLogCount == 0) {
		canvas.setTextColor(TFT_DARKGREY, BGCOLOR);
		canvas.setCursor(0, y);
		canvas.print(L->TXT_SCANNING);
		canvas.setTextColor(FGCOLOR, BGCOLOR);
	}

	int lineH = canvas.fontHeight() + 1;
	int maxRows = (canvas.height() - y - lineH) / lineH; // leave room for hint
	if (maxRows > bleSniffLogCount) maxRows = bleSniffLogCount;

	for (int i = 0; i < maxRows; i++) {
		const BleSniffEntry& e = bleSniffLog[i];

		// Color by signal strength so close/loud packets stand out.
		uint16_t color;
		if (e.rssi >= -60) color = TFT_GREEN;
		else if (e.rssi >= -80) color = TFT_YELLOW;
		else color = TFT_RED;

		canvas.setTextColor(color, BGCOLOR);
		canvas.setCursor(0, y + i * lineH);

		// "N -55 AA:BB:CC:DD:EE:FF label" — full MAC.
		String row = String(e.type) + " " + String(e.rssi) + " " + e.mac;
		if (e.label.length() > 0) row += " " + e.label;
		canvas.print(row);
	}

	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.pushSprite(0, getStatusBarHeight());
}

void bluetoothSnifferLoop() {
	if (isSetup()) {
		bleSniffLogCount = 0;
		bleSniffTotal = 0;
		bleSniffDirty = false;

		BLEDevice::init("");
		bleSniffScan = BLEDevice::getScan();
		bleSniffScan->setAdvertisedDeviceCallbacks(&bleSniffCb, true); // want duplicates: that's the "traffic"
		bleSniffScan->setActiveScan(true);   // also pull scan responses
		bleSniffScan->setInterval(100);
		bleSniffScan->setWindow(99);
		bleSniffScan->clearResults();
		bleSniffScan->start(0, nullptr, false); // duration 0 = scan continuously, non-blocking

		drawSnifferScreen();
	}

	// Repaint when new packets arrived (throttled) or periodically to refresh counter.
	if ((bleSniffDirty && checkTimer(120)) ) {
		bleSniffDirty = false;
		drawSnifferScreen();
	}

	if (checkExit()) {
		canvas.clear();
		if (bleSniffScan != nullptr) {
			bleSniffScan->stop();
			bleSniffScan->setAdvertisedDeviceCallbacks(nullptr);
			bleSniffScan->clearResults();
			bleSniffScan = nullptr;
		}
	}
}
