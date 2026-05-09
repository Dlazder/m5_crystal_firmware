// pid 35

void drawclockSettingsUi(int currentState, int hours, int minutes) {
	canvas.setTextSize(BIG_TEXT);

	int textWidth = canvas.textWidth("00:00");
	int textHeight = canvas.fontHeight() * 2;
	int x = canvas.width() / 2 - (textWidth / 2);
	int y = ((canvas.height() - getStatusBarHeight()) - textHeight) / 2;
	
	canvas.clear();
	canvas.setCursor(x, y);

	if (currentState == 0) canvas.setTextColor(BGCOLOR, FGCOLOR);
	canvas.printf("%02d", hours);
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.print(":");
	if (currentState == 1) canvas.setTextColor(BGCOLOR, FGCOLOR);
	canvas.printf("%02d", minutes);
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	if (currentState == 2) canvas.setTextColor(BGCOLOR, FGCOLOR);
	canvas.println();
	canvas.drawCenterString("OK", canvas.width() / 2, canvas.getCursorY());
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	
	canvas.pushSprite(0, getStatusBarHeight());
}

void settingsClockLoop() {
	static int tempHours = 0;
	static int tempMinutes = 0;
	static int currentState = 0;
	if (isSetup()) {
		auto dt = getDeviceTime();
		currentState = 0;
		tempHours = dt.hours;
		tempMinutes = dt.minutes;
		updateTimer();
		drawclockSettingsUi(currentState, tempHours, tempMinutes);
	}
	
	DEVICE.update();

	#ifdef CARDPUTER
		if (isKbLeftPressed() && checkTimer(100, true)) {
			currentState = (currentState + 2) % 3;
			drawclockSettingsUi(currentState, tempHours, tempMinutes);
		}
		if (isKbRightPressed() && checkTimer(100, true)) {
			currentState = (currentState + 1) % 3;
			drawclockSettingsUi(currentState, tempHours, tempMinutes);
		}
		if (isKbPlusPressed() && checkTimer(100, true)) {
			if (currentState == 0) tempHours = (tempHours + 1) % 24;
			else if (currentState == 1) tempMinutes = (tempMinutes + 1) % 60;
			drawclockSettingsUi(currentState, tempHours, tempMinutes);
		}
		if (isKbMinusPressed() && checkTimer(100, true)) {
			if (currentState == 0) tempHours = (tempHours + 23) % 24;
			else if (currentState == 1) tempMinutes = (tempMinutes + 59) % 60;
			drawclockSettingsUi(currentState, tempHours, tempMinutes);
		}
		if (isKbEnterPressed() && checkTimer(100, true)) {
			setDeviceTime(tempHours, tempMinutes, 0);
			changeProcess(PID::SETTINGS);
		}
	#else
		if (isBtnBWasPressed() && checkTimer(100, true)) {
			currentState = (currentState + 1) % 3;
			drawclockSettingsUi(currentState, tempHours, tempMinutes);
		}

		if ((isBtnAWasPressed() || isKbUpPressed()) && checkTimer(100, true)) {
			if (currentState == 0) {
				tempHours = (tempHours + 1) % 24;
				drawclockSettingsUi(currentState, tempHours, tempMinutes);
			} else if (currentState == 1) {
				tempMinutes = (tempMinutes + 1) % 60;
				drawclockSettingsUi(currentState, tempHours, tempMinutes);
			} else if (currentState == 2) {
				setDeviceTime(tempHours, tempMinutes, 0);
				changeProcess(PID::SETTINGS);
			}
		}

		if ((isBtnPWRWasPressed() || isKbDownPressed()) && checkTimer(100, true)) {
			if (currentState == 0) {
				tempHours = (tempHours + 23) % 24;
				drawclockSettingsUi(currentState, tempHours, tempMinutes);
			} else if (currentState == 1) {
				tempMinutes = (tempMinutes + 59) % 60;
				drawclockSettingsUi(currentState, tempHours, tempMinutes);
			} else if (currentState == 2) {
				setDeviceTime(tempHours, tempMinutes, 0);
				changeProcess(PID::SETTINGS);
			}
		}
	#endif
}