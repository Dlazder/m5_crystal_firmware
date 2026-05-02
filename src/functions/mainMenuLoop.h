// pid PID::MAIN_MENU

MENU mainMenu[] = {
	{PID::WIFI, "Wi-Fi"},
	{PID::BLUETOOTH, "Bluetooth"},
	{PID::NFC, "NFC"},
	{PID::SETTINGS, "settings"},
	{PID::OTHER, "other"},
};
int mainMenuSize = sizeof(mainMenu) / sizeof(MENU);

void mainMenuLoop() {
	if (isSetup()) {
		cursorOnTop();
		drawMenu(mainMenu, mainMenuSize);
	}
	menuLoop(mainMenu, mainMenuSize);
}
