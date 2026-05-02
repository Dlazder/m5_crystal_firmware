// pid ROTATION

void rotationLoop() {

  String lines[] = {
    "press A",
    "to change",
    "rotation"
  };

  if (isSetup()) {
    centeredPrintRows(lines, 3, SMALL_TEXT);
    updateTimer();
  }
  if (isBtnAWasPressed() && checkTimer(100)) {
    if (rotation == 1) {
      rotation = 3;
    } else rotation = 1;
    setData("rotation", rotation);
    DISP.setRotation(rotation);
    DISP.clear();
    centeredPrintRows(lines, 3, SMALL_TEXT);
  }
  checkExit(PID::SETTINGS);
  if (isWebDataRequested()) {
    String res = generateFunctionElement("press A<br>to change<br>rotation", SMALL_TEXT, "center");
    webData = generateWebData("function", res);
  }
}