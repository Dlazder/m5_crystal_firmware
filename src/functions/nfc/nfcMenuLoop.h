// PID::NFC

void nfcMenuLoop() {
	MENU nfcMenu[] = {
		{PID::MAIN_MENU,         L->MENU_BACK, Icons::back},
		{PID::NFC_READ,          L->MENU_NFC_READ_UID},
		{PID::NFC_WRITE,         L->MENU_NFC_WRITE},
		{PID::NFC,               "module", Icons::settings, "nfcModule"},
		{PID::NFC_CONFIG_PINS,   L->MENU_RF_CONFIG_PINS, Icons::settings},
	};
	int nfcMenuSize = sizeof(nfcMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::MAIN_MENU) cursor = 0;
		drawMenu(nfcMenu, nfcMenuSize);
	}
	menuLoop(nfcMenu, nfcMenuSize);
}
