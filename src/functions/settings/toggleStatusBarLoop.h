// pid 16

void toggleStatusBarLoop() {
	if (isSetup()) {
		centeredPrint(String(L->TXT_SETTINGS_STATUS_BAR) + String(getData("statusBar", statusBar) ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		statusBar = !getData("statusBar", statusBar);
		setData("statusBar", statusBar);
		recreateCanvas();
		DISP.clear();

		String lines[] = {
			String(L->TXT_SETTINGS_STATUS_BAR) + String(statusBar ? L->TXT_ENABLED : L->TXT_DISABLED),
		};
		centeredPrintRows(lines, 1, MEDIUM_TEXT, true);
		drawHintSwitch();
	}

	checkExit();
}
