// pid: NONE

void defaultLoop() {
	if (isSetup()) {
		DISP.setCursor(0, 60);
		String lines[] = {
			"this PID is not",
			"defined in swicher"
		};
		centeredPrintRows(lines, 2, SMALL_TEXT);
	}
	checkExit(PID::MAIN_MENU);
	if (isWebDataRequested()) {
		String res = generateFunctionElement("this PID is not", SMALL_TEXT, "center") + generateFunctionElement("defined in switcher", SMALL_TEXT, "center");
		webData = generateWebData("function", res);
	}
}