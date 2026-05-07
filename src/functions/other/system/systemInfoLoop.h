// pid 34

void systemInfoLoop() {

	if (isSetup()) {
		DISP.clear();
		String lines[] = {
			"Crystal",
			"firmware",
			"v" + String(FIRMWARE_VERSION),
		};
		centeredPrintRows(lines, 3, MEDIUM_TEXT);
	}

	checkExit();
}