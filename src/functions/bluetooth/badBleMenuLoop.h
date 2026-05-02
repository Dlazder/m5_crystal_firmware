// pid 42

MENU badBleMenu[] = {
  {14, "back"},
  {43, "run script"}
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
