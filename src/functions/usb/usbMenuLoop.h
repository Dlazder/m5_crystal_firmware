// PID:USB_MENU

void usbMenuLoop() {
	MENU usbMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK, Icons::back},
		{PID::BAD_USB, L->MENU_USB_BAD_USB, Icons::duck},
		{PID::USB_MOUSE_JIGGLER, L->MENU_USB_MOUSE_JIGGLER, Icons::mouse},
		{PID::USB_MOUSE, L->MENU_USB_MOUSE, Icons::mouse},
		{PID::USB_KEYBOARD, L->MENU_USB_KEYBOARD, Icons::keyboard},
		{PID::USB_STORAGE, L->MENU_USB_STORAGE, Icons::flashdrive},
	};
	int usbMenuSize = sizeof(usbMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(usbMenu, usbMenuSize);
	}
	menuLoop(usbMenu, usbMenuSize);
}