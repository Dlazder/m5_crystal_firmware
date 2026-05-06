// pid 41

void toggleKbGyroLoop() {
	if (isSetup()) {
		String lines[] = {
			String(L->TXT_SETTINGS_KB_GYRO) + String(getData("kbGyro", kbGyroEnabled) ? L->TXT_ENABLED : L->TXT_DISABLED),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, SMALL_TEXT);
		updateTimer();
	}

	if (isBtnAWasPressed() && checkTimer(100)) {
		kbGyroEnabled = !getData("kbGyro", kbGyroEnabled);
		setData("kbGyro", kbGyroEnabled);
		DISP.clear();

		String lines[] = {
			String(L->TXT_SETTINGS_KB_GYRO) + String(kbGyroEnabled ? L->TXT_ENABLED : L->TXT_DISABLED),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, SMALL_TEXT);
	}

	checkExit();
}
