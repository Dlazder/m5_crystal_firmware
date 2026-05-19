// pid 26

void flickerLoop() {
	static bool flickerStatusBarWasEnabled;
	static bool isOn = true;

	if (isSetup()) {
		flickerStatusBarWasEnabled = statusBar;
		statusBar = false;
	}

	if (checkTimer(300)) {
		updateTimer();
		isOn = !isOn;
		DISP.fillScreen(isOn ? FGCOLOR : BGCOLOR);
	}

	// Turn off dimming while using the flashlight.
	updateTimer(&dimmingPreviousTimer);

	if (checkExit()) {
		statusBar = flickerStatusBarWasEnabled;
	}
}