// pid 5

void _brightnessDraw() {
  static const int barHeight = 20;
  int visibleHeight = canvas.height() - getStatusBarOffset();
  int centerY = visibleHeight / 2 - barHeight / 2;
  canvas.clear();
  drawProgressBar(brightness, BRIGHTNESS_STEPS, centerY);
  drawHintRange();
}

void brightnessLoop() {
  if (isSetup()) {
    _brightnessDraw();
    updateTimer();
  }

  if ((isBtnAWasPressed() || isKbRightPressed()) && checkTimer(100)) {
    brightness = (brightness % BRIGHTNESS_STEPS) + 1;
    setData("brightness", brightness);
    DISP.setBrightness(brightnessToHw(brightness));
    Serial.printf("Brightness: %d\n", brightness);
    _brightnessDraw();
  }

  if ((isBtnPWRWasPressed() || isKbLeftPressed()) && checkTimer(100)) {
    brightness = (brightness <= 1) ? BRIGHTNESS_STEPS : brightness - 1;
    setData("brightness", brightness);
    DISP.setBrightness(brightnessToHw(brightness));
    Serial.printf("Brightness: %d\n", brightness);
    _brightnessDraw();
  }
  checkExit();

  if (isWebDataRequested()) {
    char text[50];
    sprintf(text, "brightness: %d", brightness);
    webData = generateWebData("function", generateFunctionElement(text, MEDIUM_TEXT, "center"));
  }
}
