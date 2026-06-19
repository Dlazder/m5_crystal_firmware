// PID:USB_MENU

void usbMenuLoop() {
	MENU usbMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK},
		{PID::BAD_USB, L->MENU_USB_BAD_USB}
	};
	int usbMenuSize = sizeof(usbMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(usbMenu, usbMenuSize);
	}
	menuLoop(usbMenu, usbMenuSize);
}