// pid PID::LANGUAGE

void languageLoop() {
	if (isSetup()) {
		String lines[] = {
			L->TXT_SETTINGS_LANGUAGE + String(localeNames[languageIndex]),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		languageIndex++;
		if (languageIndex >= localesCount) languageIndex = 0;

		setLocale(locales[languageIndex]);
		setData("languageIndex", languageIndex);

		String lines[] = {
			L->TXT_SETTINGS_LANGUAGE + String(localeNames[languageIndex]),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
	}

	checkExit(PID::SETTINGS);
}
