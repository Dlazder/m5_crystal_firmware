// PID:USB_MENU

void usbMenuLoop() {
	MENU usbMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK, Icons::back},
		{PID::BAD_USB, L->MENU_USB_BAD_USB, Icons::duck},
		{PID::USB_MOUSE_JIGGLER, L->MENU_USB_MOUSE_JIGGLER, Icons::mouse},
		{PID::USB_MOUSE, L->MENU_USB_MOUSE, Icons::mouse},
		{PID::USB_KEYBOARD, L->MENU_USB_KEYBOARD, Icons::keyboard},
		{PID::USB_STORAGE, L->MENU_USB_STORAGE, Icons::flashdrive},
		{PID::USB, "CH9329 enabled", Icons::settings, "ch9329Enabled"},
		{PID::USB, "CH9329 baud rate", Icons::settings, "ch9329Baud"},
		{PID::USB, "CH9329 RX pin", Icons::settings, "ch9329RxPin"},
		{PID::USB, "CH9329 TX pin", Icons::settings, "ch9329TxPin"},
	};
	int usbMenuSize = sizeof(usbMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(usbMenu, usbMenuSize);
	}
	menuLoop(usbMenu, usbMenuSize);
}