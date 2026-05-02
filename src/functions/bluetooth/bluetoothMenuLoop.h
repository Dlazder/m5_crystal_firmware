// pid PID::BLUETOOTH

MENU bluetoothMenu[] = {
	{PID::MAIN_MENU, "back"},
	{PID::BT_SCAN, "scan"},
	{PID::BAD_BLE_MENU, "bad BLE"},
	{PID::BT_KEYBOARD, "keyboard"},
	{PID::BT_MOUSE, "air mouse"},
	{PID::BT_PRESENTER, "presenter"},
	{PID::BT_SHUTTER, "shutter"},
};
int bluetoothMenuSize = sizeof(bluetoothMenu) / sizeof(MENU);

void bluetoothMenuLoop() {
	if (isSetup()) {
		cursor = 0;
		cursorOnTop();
		drawMenu(bluetoothMenu, bluetoothMenuSize);
	}
	menuLoop(bluetoothMenu, bluetoothMenuSize);
}
