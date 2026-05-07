// pid 28

void toggleStartupSound() {
	if (isSetup()) {
		String lines[] = {
			String(L->TXT_SETTINGS_SOUND) + String(getData("startupSound", startupSound) ? L->TXT_ENABLED : L->TXT_DISABLED),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		updateTimer();
	}

	if (isBtnAWasPressed() && checkTimer(100)) {
		startupSound = !getData("startupSound", startupSound);
		setData("startupSound", startupSound);

		String lines[] = {
			String(L->TXT_SETTINGS_SOUND) + String(getData("startupSound", startupSound) ? L->TXT_ENABLED : L->TXT_DISABLED),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
	}

	checkExit();
}
