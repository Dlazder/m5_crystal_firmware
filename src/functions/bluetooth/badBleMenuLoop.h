// pid PID::BAD_BLE_MENU

MENU badBleMenu[] = {
  {PID::BLUETOOTH, "back"},
  {PID::BAD_BLE, "run script"}
};
int badBleMenuSize = sizeof(badBleMenu) / sizeof(MENU);

void badBleMenuLoop() {
  if (isSetup()) {
    lfsSelectedFile = "";
    cursor = 0;
    cursorOnTop();
    drawMenu(badBleMenu, badBleMenuSize);
  }

  menuLoop(badBleMenu, badBleMenuSize);
}
