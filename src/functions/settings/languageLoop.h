// pid PID::LANGUAGE

void languageLoop() {
	if (isSetup()) {
		String lines[] = {
			"Language:",
			String(localeNames[languageIndex]),
			"Press A"
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
			"Language:",
			String(localeNames[languageIndex]),
			"Press A"
		};
		DISP.clear();
		centeredPrintRows(lines, 3, SMALL_TEXT);
	}

	checkExit(PID::SETTINGS);
}
