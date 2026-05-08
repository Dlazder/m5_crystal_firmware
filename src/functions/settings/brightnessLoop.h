// pid 5

void brightnessLoop() {
  if (isSetup()) {
    String lines[] = {
      String(L->TXT_SETTINGS_BRIGHTNESS) + String(brightness / brightnessDividor),
      L->TXT_PRESS_A
    };
    centeredPrintRows(lines, 2, MEDIUM_TEXT);
    updateTimer();
  }

  if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
    brightness += brightnessDividor;
    if (brightness >= brightnessMax || brightness / brightnessDividor == 0) brightness = brightnessMin;
    setData("brightness", brightness);
    DISP.setBrightness(brightness);

    Serial.printf("Brightness: %d\n", brightness);

    String lines[] = {
      String(L->TXT_SETTINGS_BRIGHTNESS) + String(brightness / brightnessDividor),
      L->TXT_PRESS_A
    };
    centeredPrintRows(lines, 2, MEDIUM_TEXT);
  }
  checkExit();

  if (isWebDataRequested()) {
    char text[50];
    sprintf(text, "brightness: %d", brightness / brightnessDividor);
    webData = generateWebData("function", generateFunctionElement(text, MEDIUM_TEXT, "center"));
  }
}
