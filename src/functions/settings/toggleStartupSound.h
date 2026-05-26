// pid 28

void toggleStartupSound() {
	if (isSetup()) {
		centeredPrint(String(L->TXT_SETTINGS_SOUND) + String(getData("startupSound", startupSound) ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		startupSound = !getData("startupSound", startupSound);
		setData("startupSound", startupSound);

		centeredPrint(String(L->TXT_SETTINGS_SOUND) + String(getData("startupSound", startupSound) ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
	}

	checkExit();
}
