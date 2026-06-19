// pid 25

void flashLightLoop() {
	static bool flashLightStatusBarWasEnabled;

	if (isSetup()) {
		flashLightStatusBarWasEnabled = statusBar;
		statusBar = false;
		DISP.fillScreen(FGCOLOR);
	}

	// Turn off dimming while using the flashlight.
	updateTimer(&dimmingPreviousTimer);

	if (checkExit()) {
		statusBar = flashLightStatusBarWasEnabled;
		canvas.clear();
	}
}