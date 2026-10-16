// PID::RF_CONFIG_PINS

void rfConfigurePinsMenuLoop() {
	MENU rfConfigMenu[] = {
		{PID::RF, L->MENU_BACK, Icons::back},
		{PID::RF_CONFIG_CS, "CS pin"},
		{PID::RF_CONFIG_SCK, "SCK pin"},
		{PID::RF_CONFIG_MOSI, "MOSI pin"},
		{PID::RF_CONFIG_MISO, "MISO pin"},
		{PID::RF_CONFIG_GDO0, "GDO0 pin"},
		{PID::RF_CONFIG_RESET, "reset RF pins"},
	};
	int rfConfigMenuSize = sizeof(rfConfigMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::RF) cursor = 0;
		drawMenu(rfConfigMenu, rfConfigMenuSize);
	}
	menuLoop(rfConfigMenu, rfConfigMenuSize);
}
