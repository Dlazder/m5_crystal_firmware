// pid 17

void toggleStatusBarPidLoop() {
	if (isSetup()) {
		String lines[] = {
			String(L->TXT_SETTINGS_PID) + String(getData("statusBarPid", statusBarPid) ? L->TXT_ENABLED : L->TXT_DISABLED),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		DISP.clear();
		statusBarPid = !getData("statusBarPid", statusBarPid);
		setData("statusBarPid", statusBarPid);

		String lines[] = {
			String(L->TXT_SETTINGS_PID) + String(getData("statusBarPid", statusBarPid) ? L->TXT_ENABLED : L->TXT_DISABLED),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
	}

	checkExit();
}
