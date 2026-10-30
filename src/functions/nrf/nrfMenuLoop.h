// PID::NRF

void nrfMenuLoop() {
	MENU nrfMenu[] = {
		{PID::MAIN_MENU,			L->MENU_BACK, Icons::back},
		{PID::NRF_SPECTRUM,		L->MENU_RF_SPECTRUM, Icons::nrf},
		{PID::NRF_CONFIG_PINS, L->MENU_RF_CONFIG_PINS, Icons::settings},
	};
	int nrfMenuSize = sizeof(nrfMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(nrfMenu, nrfMenuSize);
	}
	menuLoop(nrfMenu, nrfMenuSize);
}
