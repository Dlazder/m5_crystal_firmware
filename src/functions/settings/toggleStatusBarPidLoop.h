// pid 17

void toggleStatusBarPidLoop() {
	if (isSetup()) {
		centeredPrint(String(L->TXT_SETTINGS_PID) + String(getData("statusBarPid", statusBarPid) ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		DISP.clear();
		statusBarPid = !getData("statusBarPid", statusBarPid);
		setData("statusBarPid", statusBarPid);

		centeredPrint(String(L->TXT_SETTINGS_PID) + String(getData("statusBarPid", statusBarPid) ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
	}

	checkExit();
}
