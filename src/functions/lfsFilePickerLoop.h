// pid 44

MENU* lfsPickerMenu = nullptr;
int lfsPickerCount = 0;

void lfsFilePickerLoop() {
  if (isSetup()) {
    lfsSelectedFile = "";
    lfsSetup(lfsPickerMenu, lfsPickerCount, previousProcess, 44);
  }

  String selected = lfsLoop(lfsPickerMenu, lfsPickerCount, previousProcess);
  if (selected != "") {
    lfsSelectedFile = selected;
    lfsClearMenu(lfsPickerMenu, lfsPickerCount);
    changeProcess(lfsReturnPid);
  }
}
