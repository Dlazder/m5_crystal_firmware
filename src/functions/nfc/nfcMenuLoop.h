// pid PID::NFC

void nfcMenuLoop() {
	MENU nfcMenu[] = {
		{PID::MAIN_MENU,         L->MENU_BACK, Icons::back},
		{PID::NFC_READ,          L->MENU_NFC_READ_UID},
		{PID::NFC_WRITE,         L->MENU_NFC_WRITE_UID},
		{PID::NFC_ADVANCED_INFO, L->MENU_NFC_ADVANCED_INFO},
	};
	int nfcMenuSize = sizeof(nfcMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(nfcMenu, nfcMenuSize);
	}
	menuLoop(nfcMenu, nfcMenuSize);
}
