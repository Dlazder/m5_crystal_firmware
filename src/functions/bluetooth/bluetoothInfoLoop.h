// PID::BT_INFO

void bluetoothInfoLoop() {

	// TX
	String txLine = "";
	if (Ble::selected.txPower != -128) {
		int pathLoss = Ble::selected.txPower - Ble::selected.rssi;
		float distM = pow(10.0f, pathLoss / 20.0f);
		char distBuf[12];
		if (distM < 1.0f)        snprintf(distBuf, sizeof(distBuf), "~%dcm", (int)(distM * 100));
		else if (distM < 100.0f) snprintf(distBuf, sizeof(distBuf), "~%.1fm", distM);
		else                     snprintf(distBuf, sizeof(distBuf), ">100m");
		txLine = "tx:" + String(Ble::selected.txPower) + "dBm " + distBuf;
	}

	String mfrLine = Ble::selected.manufacturer.length() > 0 ? "mfr:" + String(Ble::selected.manufacturer) : String("mfr: ") + L->TXT_NA;
	String rawLine = Ble::selected.manufacturerRaw.length() > 0 ? "dat:" + Ble::selected.manufacturerRaw : "";
	String svcLine = Ble::selected.serviceUUIDs.length() > 0 ? "svc:" + String(Ble::selected.serviceUUIDs) : "";

	// build array dynamically
	String lines[8];
	int count = 0;
	lines[count++] = Ble::selected.name.length() > 0 ? Ble::selected.name : L->TXT_UNNAMED;
	lines[count++] = Ble::selected.address;
	lines[count++] = mfrLine;
	lines[count++] = "rssi: " + String(Ble::selected.rssi);
	if (txLine.length() > 0)  lines[count++] = txLine;
	if (rawLine.length() > 0) lines[count++] = rawLine;
	if (svcLine.length() > 0) lines[count++] = svcLine;
	if (Ble::selected.appearance.length() > 0) lines[count++] = Ble::selected.appearance;

	if (isSetup()) {
		if (count >= 7) centeredPrintRows(lines, count, SMALL_TEXT);
		else centeredPrintRows(lines, count, SMALL_TEXT);
	}
	checkExit();
}
