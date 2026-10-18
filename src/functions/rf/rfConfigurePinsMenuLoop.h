// PID::RF_CONFIG_PINS

void rfConfigurePinsMenuLoop() {
	MENU rfConfigMenu[] = {
		{PID::RF, L->MENU_BACK, Icons::back},
		{PID::RF_CONFIG_PINS, "CS pin", Icons::settings, "rfCsPin"},
		{PID::RF_CONFIG_PINS, "SCK pin", Icons::settings, "rfSckPin"},
		{PID::RF_CONFIG_PINS, "MOSI pin", Icons::settings, "rfMosiPin"},
		{PID::RF_CONFIG_PINS, "MISO pin", Icons::settings, "rfMisoPin"},
		{PID::RF_CONFIG_PINS, "GDO0 pin", Icons::settings, "rfGdo0Pin"},
		{PID::RF_CONFIG_RESET, "reset RF pins", Icons::rotation},
	};
	int rfConfigMenuSize = sizeof(rfConfigMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::RF) cursor = 0;
		drawMenu(rfConfigMenu, rfConfigMenuSize);
	}
	menuLoop(rfConfigMenu, rfConfigMenuSize);
}
