// Shared WiFi helpers used across multiple wifi loop files.

String wifiPasswordKey() {
	return "w:" + ssid.substring(0, 13);
}
