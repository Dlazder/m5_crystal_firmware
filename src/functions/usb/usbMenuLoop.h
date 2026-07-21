// PID:USB_MENU

void usbMenuLoop() {
	MENU usbMenu[] = {
		{PID::MAIN_MENU, L->MENU_BACK, Icons::back},
		{PID::BAD_USB, L->MENU_USB_BAD_USB},
		{PID::USB_MOUSE_JIGGLER, L->MENU_USB_MOUSE_JIGGLER},
		{PID::USB_MOUSE, L->MENU_USB_MOUSE},
		{PID::USB_KEYBOARD, L->MENU_USB_KEYBOARD},
		{PID::USB_STORAGE, L->MENU_USB_STORAGE}
	};
	int usbMenuSize = sizeof(usbMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(usbMenu, usbMenuSize);
	}
	menuLoop(usbMenu, usbMenuSize);
}