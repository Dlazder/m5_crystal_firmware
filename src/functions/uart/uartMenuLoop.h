// PID::UART

void uartMenuLoop() {
	MENU uartMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK, Icons::back},
		{PID::UART_TERMINAL, L->MENU_UART_TERMINAL, Icons::terminal},
		{PID::UART_TERMINAL, L->MENU_UART_SAVE_LOGS, Icons::settings, "uartLog"},
		{PID::UART, "baud rate", Icons::settings, "uartBaud"},
		{PID::UART, "RX pin", Icons::settings, "uartRxPin"},
		{PID::UART, "TX pin", Icons::settings, "uartTxPin"},
	};
	int uartMenuSize = sizeof(uartMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(uartMenu, uartMenuSize);
	}
	menuLoop(uartMenu, uartMenuSize);
}
