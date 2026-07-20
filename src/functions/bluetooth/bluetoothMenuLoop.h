// pid PID::BLUETOOTH

void bluetoothMenuLoop() {
	MENU bluetoothMenu[] = {
		{PID::MAIN_MENU,    L->MENU_BACK, Icons::back},
		{PID::BT_SCAN,      L->MENU_SCAN, Icons::find},
		{PID::BT_SNIFFER,   L->MENU_BT_SNIFFER},
		{PID::BAD_BLE,      L->MENU_BT_BAD_BLE},
		{PID::BT_KEYBOARD,  L->MENU_BT_KEYBOARD},
		{PID::BT_MOUSE,     L->MENU_BT_AIR_MOUSE},
		{PID::BT_MOUSE_JIGGLER, L->MENU_BT_MOUSE_JIGGLER},
		{PID::BT_PRESENTER, L->MENU_BT_PRESENTER},
		{PID::BT_SHUTTER,   L->MENU_BT_SHUTTER},
	};
	int bluetoothMenuSize = sizeof(bluetoothMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		drawMenu(bluetoothMenu, bluetoothMenuSize);
	}
	menuLoop(bluetoothMenu, bluetoothMenuSize);
}
