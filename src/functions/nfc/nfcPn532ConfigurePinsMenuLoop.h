// PID::NFC_PN532_PINS

void nfcPn532ConfigurePinsMenuLoop() {
	MENU nfcPn532ConfigMenu[] = {
		{PID::NFC_CONFIG_PINS, L->MENU_BACK, Icons::back},
		{PID::NFC_PN532_PINS, "SDA pin", Icons::settings, "pn532SdaPin"},
		{PID::NFC_PN532_PINS, "SCL pin", Icons::settings, "pn532SclPin"},
		{PID::NFC_PN532_RESET, "reset PN532 pins", Icons::rotation},
	};
	int nfcPn532ConfigMenuSize = sizeof(nfcPn532ConfigMenu) / sizeof(MENU);

	if (isSetup()) {
		if (previousProcess == PID::NFC_CONFIG_PINS) cursor = 0;
		drawMenu(nfcPn532ConfigMenu, nfcPn532ConfigMenuSize);
	}
	menuLoop(nfcPn532ConfigMenu, nfcPn532ConfigMenuSize);
}
