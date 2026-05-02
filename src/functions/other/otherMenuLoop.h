// pid PID::OTHER

MENU otherMenu[] = {
	{PID::MAIN_MENU, "back"},
	{PID::SYSTEM, "system"},
	{PID::CLOCK, "clock"},
	{PID::SOUND_LEVEL, "sound level"},
	{PID::FLASHLIGHT_MENU, "flashlight"},
	{PID::LEVEL_TOOL, "level tool"},
	{PID::BATTERY, "battery info"},
};
int otherMenuSize = sizeof(otherMenu) / sizeof(MENU);

void otherMenuLoop() {
	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(otherMenu, otherMenuSize);
	}
	menuLoop(otherMenu, otherMenuSize);
}
