// PID::NFC_CONFIG_PINS

void nfcConfigurePinsMenuLoop() {
	MENU nfcConfigMenu[] = {
		{PID::NFC,             L->MENU_BACK, Icons::back},
		{PID::NFC_PN532_PINS,  "PN532", Icons::settings},
		{PID::NFC_RC522_PINS,  "RC522", Icons::settings},
	};
	int nfcConfigMenuSize = sizeof(nfcConfigMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::NFC) cursor = 0;
		drawMenu(nfcConfigMenu, nfcConfigMenuSize);
	}
	menuLoop(nfcConfigMenu, nfcConfigMenuSize);
}
