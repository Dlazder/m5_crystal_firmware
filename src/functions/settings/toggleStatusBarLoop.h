// pid 16

void toggleStatusBarLoop() {
	if (isSetup()) {
		String lines[] = {
			String(L->TXT_SETTINGS_STATUS_BAR) + String(getData("statusBar", statusBar) ? L->TXT_ENABLED : L->TXT_DISABLED),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		updateTimer();
	}

	if (isBtnAWasPressed() && checkTimer(100)) {
		statusBar = !getData("statusBar", statusBar);
		setData("statusBar", statusBar);
		DISP.clear();

		String lines[] = {
			String(L->TXT_SETTINGS_STATUS_BAR) + String(statusBar ? L->TXT_ENABLED : L->TXT_DISABLED),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
	}

	checkExit();
}
