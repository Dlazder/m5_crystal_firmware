// pid PID::IR

void irMenuLoop() {
	MENU irMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK},
		{PID::IR_READ, L->MENU_IR_READ},
		{PID::IR_SEND, L->MENU_IR_SEND},
		{PID::IR_TV_B_GONE, L->MENU_IR_TV_B_GONE},
	};
	int irMenuSize = sizeof(irMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(irMenu, irMenuSize);
	}
	menuLoop(irMenu, irMenuSize);
}
