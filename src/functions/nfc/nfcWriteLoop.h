// PID::NFC_WRITE

void nfcWriteLoop() {
	MENU nfcWriteMenu[] = {
		{PID::NFC,              L->MENU_BACK, Icons::back},
		{PID::NFC_WRITE_UID,     L->MENU_NFC_WRITE_UID},
		{PID::NFC_WRITE_SECTORS, L->MENU_NFC_WRITE_SECTORS},
	};
	int nfcWriteMenuSize = sizeof(nfcWriteMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(nfcWriteMenu, nfcWriteMenuSize);
	}
	menuLoop(nfcWriteMenu, nfcWriteMenuSize);
}
