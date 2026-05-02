// pid PID::NFC

MENU nfcMenu[] = {
	{PID::MAIN_MENU, "back"},
	{PID::NFC_READ, "read UID"},
	{PID::NFC_WRITE, "write UID"},
};

int nfcMenuSize = sizeof(nfcMenu) / sizeof(MENU);

void nfcMenuLoop() {
	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(nfcMenu, nfcMenuSize);
	}
	menuLoop(nfcMenu, nfcMenuSize);
}
