// pid PID::IR

void irMenuLoop() {
	MENU irMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK},
		{PID::IR_READ, "read signal"},
	};
	int irMenuSize = sizeof(irMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(irMenu, irMenuSize);
	}
	menuLoop(irMenu, irMenuSize);
}
