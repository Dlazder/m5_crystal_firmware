// pid 7

void colorsLoop() {
	if (isSetup()) {
		String lines[] = {String(L->TXT_SETTINGS_COLOR) + String(colorsEntry[colorIndex]), L->TXT_PRESS_A};
		centeredPrintRows(lines, 2, SMALL_TEXT);
		updateTimer();
	}
	if (isBtnAWasPressed() && checkTimer(100)) {
		colorIndex++;
		if (colorIndex == sizeof(colors) / sizeof(colors[0])) {
			colorIndex = 0;
		}
		Serial.printf("current colorIndex: %d\n", colorIndex);

		FGCOLOR = colors[colorIndex];
		setData("colorIndex", colorIndex);
		String lines[] = {String(L->TXT_SETTINGS_COLOR) + String(colorsEntry[colorIndex]), L->TXT_PRESS_A};
		centeredPrintRows(lines, 2, SMALL_TEXT);
	}
	checkExit();
}
