// pid NONE

void statusBarLoop() {
	static int statusBarTimer = 0;
	static int battery = DEVICE.Power.getBatteryLevel();
	
	statusBarCanvas.clear();
	statusBarCanvas.setTextColor(FGCOLOR, BGCOLOR);
	statusBarCanvas.setCursor(5, 4);
	statusBarCanvas.setTextSize(TINY_TEXT);
	
	// PID
	if (getData("statusBarPid", statusBarPid)) {
		statusBarCanvas.printf("#:%d; ", process);
	}
	
	// Clock
	auto dt = DEVICE.Rtc.getDateTime();
	char formatString[30];
	sprintf(formatString, "%02d:%02d:%02d   ", dt.time.hours, dt.time.minutes, dt.time.seconds);
	statusBarCanvas.print(formatString);

	// Battery
	if (checkTimer(3000, true, &statusBarTimer)) {
		battery = DEVICE.Power.getBatteryLevel();
	}
	char batteryText[10];
	sprintf(batteryText, "%d%%", battery);
	int batteryTextWidth = DISP.textWidth(batteryText);

	// WiFi indicator (14px tall, matching BT icon height)
	if (WiFi.isConnected()) {
		const int wcx = DISP.width() - batteryTextWidth - 20;
		const int wcy = 14;
		statusBarCanvas.fillCircle(wcx, wcy, 1, FGCOLOR);
		statusBarCanvas.drawArc(wcx, wcy, 4,  3,  210, 330, FGCOLOR);
		statusBarCanvas.drawArc(wcx, wcy, 7,  6,  210, 330, FGCOLOR);
		statusBarCanvas.drawArc(wcx, wcy, 10, 9,  210, 330, FGCOLOR);
	}

	// Bluetooth indicator
	if (bleCompositeBegan && bleKeyboard.isConnected()) {
		const float svgH = 86.0f;
		const float h = 14.0f;
		const float scale = h / svgH;
		const int by = 2;
		int bleIndicatorWidth = (int)(50.695f * scale) + 3;
		int wifiOffset = WiFi.isConnected() ? 26 : 0;
		int bx = DISP.width() - batteryTextWidth - bleIndicatorWidth - 10 - wifiOffset;

		auto px = [&](float x) { return bx + (int)(x * scale); };
		auto py = [&](float y) { return by + (int)(y * scale); };

		statusBarCanvas.drawLine(px(28.649f), py(6.825f),  px(28.649f), py(80.035f), FGCOLOR);
		statusBarCanvas.drawLine(px(28.649f), py(6.825f),  px(50.695f), py(28.871f), FGCOLOR);
		statusBarCanvas.drawLine(px(50.695f), py(28.871f), px(36.136f), py(43.430f), FGCOLOR);
		statusBarCanvas.drawLine(px(36.136f), py(43.430f), px(50.695f), py(57.989f), FGCOLOR);
		statusBarCanvas.drawLine(px(50.695f), py(57.989f), px(28.649f), py(80.035f), FGCOLOR);
		statusBarCanvas.drawLine(px(28.649f), py(35.889f), px(12.112f), py(28.172f), FGCOLOR);
		statusBarCanvas.drawLine(px(28.649f), py(50.971f), px(12.112f), py(58.688f), FGCOLOR);
	}

	statusBarCanvas.setCursor(DISP.width() - batteryTextWidth, 4);
	statusBarCanvas.printf("%d%%", battery);

	statusBarCanvas.drawLine(0, 19, DISP.width(), 19, FGCOLOR);
	statusBarCanvas.pushSprite(0, 0);
}
