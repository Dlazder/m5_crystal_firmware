// PID::WIFI_INFO

void wifiInfoLoop() {
	if (isSetup()) {
		Wifi::wiuBegin(Wifi::selected.bssid, Wifi::selected.channel);
	}
	Wifi::wiuUpdate();

	String lines[] = {
		Wifi::selected.ssid,
		"MAC: " + Wifi::selected.mac,
		"Security: " + String(Wifi::wiuAuthStr(Wifi::selected.authMode)),
		"WPS: "     + String(Wifi::wiuDone() ? (Wifi::wiuHasWps() ? "ON" : "OFF") : "..."),
		"FT-PSK: "  + String(Wifi::wiuDone() ? (Wifi::wiuHasFt()  ? "ON" : "OFF") : "..."),
		"Ch:" + String(Wifi::selected.channel) + "  RSSI:" + String(Wifi::selected.rssi),
	};
	centeredPrintRows(lines, 6, TINY_TEXT);

	if (checkExit()) {
		Wifi::wiuCleanup();
	}
}
