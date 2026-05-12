// PID::CLOCK

void clockLoop() {
	static int oldSeconds;
	auto dt = deviceGetTime();
	char formatString[30];
	sprintf(formatString, "%02d:%02d:%02d", dt.hours, dt.minutes, dt.seconds);
	if (dt.seconds != oldSeconds) {
		oldSeconds = dt.seconds;
		centeredPrint(formatString, BIG_TEXT);
		canvas.pushSprite(0, getStatusBarHeight());
	}
	checkExit();
	
	if (isWebDataRequested()) {
		webData = generateWebData("function", generateFunctionElement(formatString, MEDIUM_TEXT, "center"));
	}
}