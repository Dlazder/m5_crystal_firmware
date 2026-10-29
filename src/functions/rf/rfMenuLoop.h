// PID::RF

void rfMenuLoop() {
	MENU rfMenu[] = {
		{PID::MAIN_MENU,			L->MENU_BACK, Icons::back},
		{PID::RF_SPECTRUM,		L->MENU_RF_SPECTRUM, Icons::rf},
		{PID::RF_RAW_SNIFFER, L->MENU_RF_RAW_SNIFFER, Icons::rf},
		{PID::RF_CONFIG_PINS, L->MENU_RF_CONFIG_PINS, Icons::settings},
	};
	int rfMenuSize = sizeof(rfMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(rfMenu, rfMenuSize);
	}
	menuLoop(rfMenu, rfMenuSize);
}
