// pid PID::LANGUAGE

void languageLoop() {
	if (isSetup()) {
		String lines[] = {
			L->TXT_SETTINGS_LANGUAGE,
			String(localeNames[languageIndex]),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 3, SMALL_TEXT);
		updateTimer();
	}

	if (isBtnAWasPressed() && checkTimer(100)) {
		languageIndex++;
		if (languageIndex >= localesCount) languageIndex = 0;

		setLocale(locales[languageIndex]);
		setData("languageIndex", languageIndex);

		String lines[] = {
			L->TXT_SETTINGS_LANGUAGE,
			String(localeNames[languageIndex]),
			L->TXT_PRESS_A
		};
		DISP.clear();
		centeredPrintRows(lines, 3, SMALL_TEXT);
	}

	checkExit(PID::SETTINGS);
}
