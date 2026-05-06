// pid PID::SETTINGS

MENU settingsMenu[] = {
	{PID::MAIN_MENU, "back"},
	{PID::SETTINGS_CLOCK, "time"},
	{PID::BRIGHTNESS, "brightness"},
	{PID::ROTATION, "rotation"},
	{PID::COLORS, "colors"},
	{PID::SETTINGS_STATUS_BAR, "status bar"},
	{PID::STARTUP_SOUND, "startup sound"},
	{PID::FONTS, "font"},
	{PID::KB_GYRO, "keyboard gyro"},
	{PID::LANGUAGE, "language"},
};
int settingsMenuSize = sizeof(settingsMenu) / sizeof(MENU);

void settingsMenuLoop() {
	if (isSetup()) {
		DISP.setTextSize(SMALL_TEXT);
		cursorOnTop();
		if (previousProcess == PID::MAIN_MENU) cursor = 0;
		drawMenu(settingsMenu, settingsMenuSize);
	}
	menuLoop(settingsMenu, settingsMenuSize);
}
