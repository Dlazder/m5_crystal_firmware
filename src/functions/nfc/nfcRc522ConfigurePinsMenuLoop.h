// PID::NFC_RC522_PINS

void nfcRc522ConfigurePinsMenuLoop() {
	MENU nfcRc522ConfigMenu[] = {
		{PID::NFC_CONFIG_PINS, L->MENU_BACK, Icons::back},
		{PID::NFC_RC522_PINS, "SS (SDA) pin", Icons::settings, "rc522SsPin"},
		{PID::NFC_RC522_PINS, "SCK pin", Icons::settings, "rc522SckPin"},
		{PID::NFC_RC522_PINS, "MOSI pin", Icons::settings, "rc522MosiPin"},
		{PID::NFC_RC522_PINS, "MISO pin", Icons::settings, "rc522MisoPin"},
		{PID::NFC_RC522_PINS, "RST pin", Icons::settings, "rc522RstPin"},
		{PID::NFC_RC522_RESET, "reset RC522 pins", Icons::rotation},
	};
	int nfcRc522ConfigMenuSize = sizeof(nfcRc522ConfigMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::NFC_CONFIG_PINS) cursor = 0;
		drawMenu(nfcRc522ConfigMenu, nfcRc522ConfigMenuSize);
	}
	menuLoop(nfcRc522ConfigMenu, nfcRc522ConfigMenuSize);
}
