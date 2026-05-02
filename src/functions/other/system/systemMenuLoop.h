// pid PID::SYSTEM

MENU systemMenu[] = {
	{PID::OTHER, "back"},
	{PID::SHUTDOWN, "shutdown"},
	{PID::SYSTEM_INFO, "info"}
};
int systemMenuSize = sizeof(systemMenu) / sizeof(MENU);


void systemMenuLoop() {
	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(systemMenu, systemMenuSize);
	}
	menuLoop(systemMenu, systemMenuSize);
}
