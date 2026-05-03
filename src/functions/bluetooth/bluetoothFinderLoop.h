// pid PID::BT_FINDER

static bool bleScanDone = false;
static int bleFoundRssi = -100;

void onBleScanComplete(BLEScanResults results) {
	bleFoundRssi = -100;
	for (int i = 0; i < results.getCount(); i++) {
		BLEAdvertisedDevice device = results.getDevice(i);
		if (String(device.getAddress().toString().c_str()) == bleAddress) {
			bleFoundRssi = device.getRSSI();
			break;
		}
	}
	BLEDevice::getScan()->clearResults();
	bleScanDone = true;
}

void drawFinderScreen(int currentRssi) {
	uint16_t color;
	if (currentRssi >= -60) color = TFT_GREEN;
	else if (currentRssi >= -80) color = TFT_YELLOW;
	else color = TFT_RED;

	canvas.clear();
	canvas.setTextSize(SMALL_TEXT);
	canvas.setTextColor(FGCOLOR);
	canvas.drawCenterString(bleName.substring(0, 14), DISP.width() / 2, getStatusBarHeight() + 4);
	canvas.drawCenterString(bleAddress, DISP.width() / 2, getStatusBarHeight() + 4 + canvas.fontHeight() + 2);

	canvas.setTextSize(BIG_TEXT);
	canvas.setTextColor(color);
	String rssiStr = (currentRssi == -100) ? "n/a" : String(currentRssi);
	canvas.drawCenterString(rssiStr, DISP.width() / 2, canvas.getCursorY());
	canvas.pushSprite(0, getStatusBarHeight());
}

void bluetoothFinderLoop() {
	static bool scanning = false;

	if (isSetup()) {
		scanning = false;
		bleScanDone = false;
		bleFoundRssi = -100;
		BLEDevice::init("");
		BLEScan* pScan = BLEDevice::getScan();
		pScan->setActiveScan(false);
		pScan->setInterval(100);
		pScan->setWindow(99);
		drawFinderScreen(bleFoundRssi);
	}

	if (bleScanDone) {
		bleScanDone = false;
		scanning = false;
		drawFinderScreen(bleFoundRssi);
	}

	if (!scanning && checkTimer(1000)) {
		BLEScan* pScan = BLEDevice::getScan();
		pScan->clearResults();
		pScan->start(1, onBleScanComplete, false);
		scanning = true;
	}

	checkExit();
}
