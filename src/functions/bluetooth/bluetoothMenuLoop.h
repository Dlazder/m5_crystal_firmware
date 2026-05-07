// pid PID::BLUETOOTH

void bluetoothMenuLoop() {
	MENU bluetoothMenu[] = {
		{PID::MAIN_MENU,    L->MENU_BACK},
		{PID::BT_SCAN,      L->MENU_SCAN},
		{PID::BAD_BLE_MENU, L->MENU_BT_BAD_BLE},
		{PID::BT_KEYBOARD,  L->MENU_BT_KEYBOARD},
		{PID::BT_MOUSE,     L->MENU_BT_AIR_MOUSE},
		{PID::BT_PRESENTER, L->MENU_BT_PRESENTER},
		{PID::BT_SHUTTER,   L->MENU_BT_SHUTTER},
	};
	int bluetoothMenuSize = sizeof(bluetoothMenu) / sizeof(MENU);

	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(bluetoothMenu, bluetoothMenuSize);
	}
	menuLoop(bluetoothMenu, bluetoothMenuSize);
}
