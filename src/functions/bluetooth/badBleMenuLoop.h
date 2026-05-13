// pid PID::BAD_BLE_MENU

void badBleMenuLoop() {
  MENU badBleMenu[] = {
    {PID::BLUETOOTH,    L->MENU_BACK},
    {PID::BAD_BLE,      L->MENU_FILES_LITTLEFS},
    {PID::BAD_BLE,      L->MENU_FILES_SDCARD},
  };
  int badBleMenuSize = sizeof(badBleMenu) / sizeof(MENU);

  if (isSetup()) {
    if (previousProcess != PID::FILE_PICKER && previousProcess != PID::FILE_PICKER_SD) {
      selectedFilePath = "";
    }
    cursor = 0;
    cursorOnTop();
    drawMenu(badBleMenu, badBleMenuSize);
  }
  menuLoop(badBleMenu, badBleMenuSize);
}
