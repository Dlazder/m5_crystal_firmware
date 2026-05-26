// pid ROTATION

void rotationLoop() {
  if (isSetup()) {
    centeredPrint(L->TXT_SETTINGS_ROTATION, MEDIUM_TEXT, true);
    drawHintSwitch();
    updateTimer();
  }
  if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
    if (rotation == 1) {
      rotation = 3;
    } else rotation = 1;
    setData("rotation", rotation);
    DISP.setRotation(rotation);
    DISP.clear();
    centeredPrint(L->TXT_SETTINGS_ROTATION, MEDIUM_TEXT, true);
    drawHintSwitch();
  }
  checkExit(PID::SETTINGS);
  if (isWebDataRequested()) {
    String res = generateFunctionElement("press A<br>to change<br>rotation", MEDIUM_TEXT, "center");
    webData = generateWebData("function", res);
  }
}
