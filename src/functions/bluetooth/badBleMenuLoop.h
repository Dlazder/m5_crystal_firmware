// pid PID::BAD_BLE_MENU

void badBleMenuLoop() {
  MENU badBleMenu[] = {
    {PID::BLUETOOTH, L->MENU_BACK},
    {PID::BAD_BLE,   L->MENU_LITTLEFS},
  };
  int badBleMenuSize = sizeof(badBleMenu) / sizeof(MENU);

  if (isSetup()) {
    lfsSelectedFile = "";
    cursor = 0;
    cursorOnTop();
    drawMenu(badBleMenu, badBleMenuSize);
  }
  menuLoop(badBleMenu, badBleMenuSize);
}
