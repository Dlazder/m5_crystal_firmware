// pid 41

void toggleKbGyroLoop() {
	if (isSetup()) {
		centeredPrint(String(L->TXT_SETTINGS_KB_GYRO) + String(getData("kbGyro", kbGyroEnabled) ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		kbGyroEnabled = !getData("kbGyro", kbGyroEnabled);
		setData("kbGyro", kbGyroEnabled);

		centeredPrint(String(L->TXT_SETTINGS_KB_GYRO) + String(kbGyroEnabled ? L->TXT_ENABLED : L->TXT_DISABLED), MEDIUM_TEXT, true);
		drawHintSwitch();
	}

	checkExit();
}
