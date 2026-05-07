// pid 1

void clockLoop() {
  if (isSetup()) {
    canvas.setTextColor(FGCOLOR, BGCOLOR);
  }

  static int oldSeconds;
  auto dt = DEVICE.Rtc.getDateTime();
  char formatString[30];
  sprintf(formatString, "%02d:%02d:%02d", dt.time.hours, dt.time.minutes, dt.time.seconds);
  if (dt.time.seconds != oldSeconds) {
    oldSeconds = dt.time.seconds;
    canvas.clear();
    canvas.setTextSize(BIG_TEXT);
    canvas.drawCenterString(formatString, DISP.width() / 2, ((DISP.height() - getStatusBarHeight()) - DISP.fontHeight()) / 2);
    canvas.pushSprite(0, getStatusBarHeight());
  }
  checkExit();
  
  if (isWebDataRequested()) {
    webData = generateWebData("function", generateFunctionElement(formatString, MEDIUM_TEXT, "center"));
  }
}