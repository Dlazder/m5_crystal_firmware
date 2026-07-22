// pid NONE

void statusBarLoop() {
	static int statusBarTimer = 0;
	static int battery = DEVICE.Power.getBatteryLevel();
	
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
	sprintf(formatString, "%02d:%02d:%02d   ", dt.hours, dt.minutes, dt.seconds);
	statusBarCanvas.print(formatString);

	// Battery
	if (checkTimer(3000, true, &statusBarTimer)) {
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

	statusBarCanvas.setCursor(batteryX, 4);
	statusBarCanvas.printf("%d%%", battery);

	statusBarCanvas.drawLine(0, 19, DISP.width(), 19, FGCOLOR);
	statusBarCanvas.pushSprite(0, 0);
}
