// pid 12

void wifiInfoLoop() {
	
	String lines[] = {
		ssid,
		mac,
		"rssi" + String(rssi),
		"channel: " + String(channel),
	};

	if (isSetup()) {
		centeredPrintRows(lines, 4, MEDIUM_TEXT);
	}
	checkExit();
}