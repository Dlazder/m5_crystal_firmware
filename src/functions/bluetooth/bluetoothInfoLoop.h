// pid PID::BT_INFO

void bluetoothInfoLoop() {

	// TX
	String txLine = "";
	if (bleTxPower != -128) {
		int pathLoss = bleTxPower - bleRssi;
		float distM = pow(10.0f, pathLoss / 20.0f);
		char distBuf[12];
		if (distM < 1.0f)        snprintf(distBuf, sizeof(distBuf), "~%dcm", (int)(distM * 100));
		else if (distM < 100.0f) snprintf(distBuf, sizeof(distBuf), "~%.1fm", distM);
		else                     snprintf(distBuf, sizeof(distBuf), ">100m");
		txLine = "tx:" + String(bleTxPower) + "dBm " + distBuf;
	}

	String mfrLine = bleManufacturer.length() > 0 ? "mfr:" + String(bleManufacturer) : String("mfr: ") + L->TXT_NA;
	String rawLine = bleManufacturerRaw.length() > 0 ? "dat:" + bleManufacturerRaw : "";
	String svcLine = bleServiceUUIDs.length() > 0 ? "svc:" + String(bleServiceUUIDs) : "";

	// build array dynamically
	String lines[8];
	int count = 0;
	lines[count++] = bleName.length() > 0 ? bleName : L->TXT_UNNAMED;
	lines[count++] = bleAddress;
	lines[count++] = mfrLine;
	lines[count++] = "rssi: " + String(bleRssi);
	if (txLine.length() > 0)  lines[count++] = txLine;
	if (rawLine.length() > 0) lines[count++] = rawLine;
	if (svcLine.length() > 0) lines[count++] = svcLine;
	if (bleAppearance.length() > 0) lines[count++] = bleAppearance;

	if (isSetup()) {
		if (count >= 7) centeredPrintRows(lines, count, TINY_TEXT);
		else centeredPrintRows(lines, count, SMALL_TEXT);
	}
	checkExit();
}
