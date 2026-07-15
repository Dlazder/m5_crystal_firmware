// PID::ICONS

void toggleIconsLoop() {
	if (isSetup()) {
		centeredPrint(String("Icons: " + String(iconsEnabled ? L->TXT_ENABLED : L->TXT_DISABLED)), MEDIUM_TEXT, true);

		drawHintSwitch();
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		iconsEnabled = !getData("icons", iconsEnabled);
		setData("icons", iconsEnabled);
		centeredPrint("Icons: " + String(iconsEnabled ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
	}

	checkExit();
}