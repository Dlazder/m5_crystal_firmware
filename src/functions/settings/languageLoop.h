// pid PID::LANGUAGE

void languageLoop() {
	if (isSetup()) {
		centeredPrint(String(L->TXT_SETTINGS_LANGUAGE) + String(localeNames[languageIndex]), MEDIUM_TEXT, true);
		drawHintSwitch();
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		languageIndex++;
		if (languageIndex >= localesCount) languageIndex = 0;

		setLocale(locales[languageIndex]);
		setData("languageIndex", languageIndex);

		centeredPrint(String(L->TXT_SETTINGS_LANGUAGE) + String(localeNames[languageIndex]), MEDIUM_TEXT, true);
		drawHintSwitch();
	}

	checkExit(PID::SETTINGS);
}
