// pid PID::FLASHLIGHT_MENU

MENU flashLightMenu[] = {
	{PID::OTHER, "back"},
	{PID::FLASHLIGHT, "flashlight"},
	{PID::FLASHLIGHT_FLICKER, "flicker"},
	{PID::BRIGHTNESS, "brightness"},
	{PID::COLORS, "colors"}
};
int flashLightMenuSize = sizeof(flashLightMenu) / sizeof(MENU);

void flashLightMenuLoop() {
	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(flashLightMenu, flashLightMenuSize);
	}
	menuLoop(flashLightMenu, flashLightMenuSize);
}