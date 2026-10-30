// PID::NRF_CONFIG_PINS

void nrfConfigurePinsMenuLoop() {
	MENU nrfConfigMenu[] = {
		{PID::NRF, L->MENU_BACK, Icons::back},
		{PID::NRF_CONFIG_PINS, "CSN pin", Icons::settings, "nrfCsnPin"},
		{PID::NRF_CONFIG_PINS, "SCK pin", Icons::settings, "nrfSckPin"},
		{PID::NRF_CONFIG_PINS, "MOSI pin", Icons::settings, "nrfMosiPin"},
		{PID::NRF_CONFIG_PINS, "MISO pin", Icons::settings, "nrfMisoPin"},
		{PID::NRF_CONFIG_PINS, "CE pin", Icons::settings, "nrfCePin"},
		{PID::NRF_CONFIG_RESET, "reset NRF pins", Icons::rotation},
	};
	int nrfConfigMenuSize = sizeof(nrfConfigMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::NRF) cursor = 0;
		drawMenu(nrfConfigMenu, nrfConfigMenuSize);
	}
	menuLoop(nrfConfigMenu, nrfConfigMenuSize);
}
