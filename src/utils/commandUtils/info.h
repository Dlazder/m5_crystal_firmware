void infoCommand() {
	Serial.printf("Detected board id: %d\n", (int)DEVICE.getBoard());
	Serial.printf("SDA=%d  SCL=%d\n", SDA, SCL);
}