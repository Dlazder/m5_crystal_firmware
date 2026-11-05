// PID::MAIN_MENU

void mainMenuLoop() {
	MENU mainMenuAll[] = {
		{PID::WIFI, L->MENU_WIFI, Icons::wifi},
		{PID::BLUETOOTH, L->MENU_BT, Icons::bluetooth},
		{PID::NFC, L->MENU_NFC, Icons::nfc},
		{PID::IR, L->MENU_IR, Icons::ir},
		{PID::USB, L->MENU_USB, Icons::usb},
		{PID::RF, "RF", Icons::rf},
		{PID::NRF, "NRF", Icons::nrf},
		{PID::UART, L->MENU_UART, Icons::terminal},
		{PID::SETTINGS, L->MENU_SETTINGS, Icons::settings},
		{PID::FILES_MENU, L->MENU_FILES, Icons::folder},
		{PID::OTHER, L->MENU_OTHER, Icons::other},
	};
	int mainMenuAllSize = sizeof(mainMenuAll) / sizeof(MENU);

	// Sections hidden via settings > UI > customize menu.
	MENU mainMenu[mainMenuAllSize];
	int mainMenuSize = 0;
	for (int i = 0; i < mainMenuAllSize; i++) {
		int pid = mainMenuAll[i].command;
		if (pid == PID::WIFI && !showMenuWifi) continue;
		if (pid == PID::BLUETOOTH && !showMenuBt) continue;
		if (pid == PID::NFC && !showMenuNfc) continue;
		if (pid == PID::IR && !showMenuIr) continue;
		if (pid == PID::USB && !showMenuUsb) continue;
		if (pid == PID::RF && !showMenuRf) continue;
		if (pid == PID::NRF && !showMenuNrf) continue;
		if (pid == PID::UART && !showMenuUart) continue;
		if (pid == PID::FILES_MENU && !showMenuFiles) continue;
		if (pid == PID::OTHER && !showMenuOther) continue;
		mainMenu[mainMenuSize++] = mainMenuAll[i];
	}

	if (isSetup()) {
		cursor = 0;
		drawMenu(mainMenu, mainMenuSize);
	}
	menuLoop(mainMenu, mainMenuSize);
}
