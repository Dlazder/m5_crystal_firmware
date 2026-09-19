// PID::SETTINGS_STATUS_BAR

void statusBarLoop() {
	static int statusBarTimer = 0;

	// Redraw at most once per second — avoids SPI pushSprite + I2C RTC read on every tick
	if (!checkTimer(1000, true, &statusBarTimer)) {
		return;
	}

	statusBarCanvas.clear();
	statusBarCanvas.setTextColor(FGCOLOR, BGCOLOR);
	statusBarCanvas.setCursor(5, 4);
	// note: hardcode the text size value so that modifiers do not affect the text size when changing the font
	statusBarCanvas.setTextSize(1.5);
	
	// PID
	if (getData("statusBarPid", statusBarPid)) {
		statusBarCanvas.printf("#:%d; ", process);
	}
	
	// Clock
	auto dt = deviceGetTime();
	char formatString[30];
	if (statusBarSeconds) {
		sprintf(formatString, "%02d:%02d:%02d   ", dt.hours, dt.minutes, dt.seconds);
	} else {
		sprintf(formatString, "%02d:%02d   ", dt.hours, dt.minutes);
	}
	statusBarCanvas.print(formatString);

	// Battery
	static int battery = DEVICE.Power.getBatteryLevel();
	static int batteryTimer = 0;
	if (checkTimer(5000, true, &batteryTimer)) {
		battery = DEVICE.Power.getBatteryLevel();
	}
	char batteryText[10];
	sprintf(batteryText, "%d%%", battery);
	int batteryTextWidth = statusBarCanvas.textWidth(batteryText);
	const int STATUS_BAR_GAP = 5;
	int batteryX = DISP.width() - batteryTextWidth - STATUS_BAR_GAP;

	// WiFi indicator
	if (WiFi.isConnected()) {
		int wx = batteryX - STATUS_BAR_GAP - MENU_ICON_W;
		statusBarCanvas.drawBitmap(wx, 2, Icons::wifi, MENU_ICON_W, MENU_ICON_H, FGCOLOR);
	}

	// Bluetooth indicator
	if (bleCompositeBegan && bleKeyboard.isConnected()) {
		int bx = batteryX - STATUS_BAR_GAP - MENU_ICON_W;
		if (WiFi.isConnected()) {
			bx -= MENU_ICON_W + STATUS_BAR_GAP;
		}
		statusBarCanvas.drawBitmap(bx, 2, Icons::bluetooth, MENU_ICON_W, MENU_ICON_H, FGCOLOR);
	}

	// Clipboard indicator (a file is staged for paste)
	if (clipboardHasFile) {
		int cx = batteryX - STATUS_BAR_GAP - MENU_ICON_W;
		if (WiFi.isConnected()) {
			cx -= MENU_ICON_W + STATUS_BAR_GAP;
		}
		if (bleCompositeBegan && bleKeyboard.isConnected()) {
			cx -= MENU_ICON_W + STATUS_BAR_GAP;
		}
		statusBarCanvas.drawBitmap(cx, 2, Icons::file, MENU_ICON_W, MENU_ICON_H, FGCOLOR);
	}

	statusBarCanvas.setCursor(batteryX, 4);
	statusBarCanvas.printf("%d%%", battery);

	statusBarCanvas.drawLine(0, 19, DISP.width(), 19, FGCOLOR);
	statusBarCanvas.pushSprite(0, 0);
}
