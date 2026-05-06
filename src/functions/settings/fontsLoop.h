// pid FONTS

void fontsLoop() {
  if (isSetup()) {
    String lines[] = {
      "(beta)",
      String(L->TXT_SETTINGS_FONT) + String(fontNames[currentFontIndex]),
      L->TXT_PRESS_A
    };
    centeredPrintRows(lines, 3, SMALL_TEXT);
    updateTimer();
  }

  if (isBtnAWasPressed() && checkTimer(100)) {
    currentFontIndex++;
    if (currentFontIndex == sizeof(systemFonts) / sizeof(systemFonts[0])) {
      currentFontIndex = 0;
    }
    Serial.printf("Current font index: %d (%s)\n", currentFontIndex, fontNames[currentFontIndex]);

    DISP.setFont(systemFonts[currentFontIndex]);

    setData("fontIndex", currentFontIndex);

    String lines[] = {
      "(beta)",
      String(L->TXT_SETTINGS_FONT) + String(fontNames[currentFontIndex]),
      L->TXT_PRESS_A
    };
    DISP.clear();
    centeredPrintRows(lines, 3, SMALL_TEXT);
  }

  checkExit(PID::SETTINGS);
}
