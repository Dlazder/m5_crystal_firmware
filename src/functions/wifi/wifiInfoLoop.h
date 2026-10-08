// PID::WIFI_INFO

void wifiInfoLoop() {
	if (isSetup()) {
		Wifi::wiuBegin(bssid, channel);
	}
	Wifi::wiuUpdate();

	String lines[] = {
		ssid,
		"MAC: " + mac,
		"Security: " + String(Wifi::wiuAuthStr(wifiAuthMode)),
		"WPS: "     + String(Wifi::wiuDone() ? (Wifi::wiuHasWps() ? "ON" : "OFF") : "..."),
		"FT-PSK: "  + String(Wifi::wiuDone() ? (Wifi::wiuHasFt()  ? "ON" : "OFF") : "..."),
		"Ch:" + String(channel) + "  RSSI:" + String(rssi),
	};
	centeredPrintRows(lines, 6, TINY_TEXT);

	if (checkExit()) {
		Wifi::wiuCleanup();
	}
}
