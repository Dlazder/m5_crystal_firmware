// pid PID::SETTINGS_STATUS_BAR

MENU settingsStatusBarMenu[] = {
	{PID::SETTINGS, "back"},
	{PID::TOGGLE_STATUS_BAR, "status bar"},
	{PID::TOGGLE_STATUS_BAR_PID, "PID"},
};

int settingsStatusBarMenuSize = sizeof(settingsStatusBarMenu) / sizeof(MENU);

void settingsStatusBarMenuLoop() {
	if (isSetup()) {
		if (previousProcess == PID::SETTINGS) cursor = 0;
		cursorOnTop();
		drawMenu(settingsStatusBarMenu, settingsStatusBarMenuSize);
	}
	menuLoop(settingsStatusBarMenu, settingsStatusBarMenuSize);
}
