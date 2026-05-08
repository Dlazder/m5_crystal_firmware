// pid ROTATION

void rotationLoop() {
  if (isSetup()) {
    String lines[] = {
      L->TXT_PRESS_A,
      "to change",
      L->TXT_SETTINGS_ROTATION
    };
    centeredPrintRows(lines, 3, MEDIUM_TEXT);
    updateTimer();
  }
  if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
    if (rotation == 1) {
      rotation = 3;
    } else rotation = 1;
    setData("rotation", rotation);
    DISP.setRotation(rotation);
    DISP.clear();
    String lines[] = {
      L->TXT_PRESS_A,
      "to change",
      L->TXT_SETTINGS_ROTATION
    };
    centeredPrintRows(lines, 3, MEDIUM_TEXT);
  }
  checkExit(PID::SETTINGS);
  if (isWebDataRequested()) {
    String res = generateFunctionElement("press A<br>to change<br>rotation", MEDIUM_TEXT, "center");
    webData = generateWebData("function", res);
  }
}
