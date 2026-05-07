// pid 2

void batteryLoop() {
	int battery = DEVICE.Power.getBatteryLevel();
	
	DISP.setTextColor(FGCOLOR, BGCOLOR);

	String lines[] = {
		String(battery) + "%",
		String(DEVICE.Power.getBatteryVoltage()) + " V",
	};

	if (isSetup()) {
		centeredPrintRows(lines, 2, BIG_TEXT);
	}

	if (checkTimer(2000)) {
		centeredPrintRows(lines, 2, BIG_TEXT);
	}
	
	checkExit();

	if (isWebDataRequested()) {
		char text[10];
		sprintf(text, "%d%%", battery);
		webData = generateWebData("function", generateFunctionElement(text, BIG_TEXT, "center"));
	}
}