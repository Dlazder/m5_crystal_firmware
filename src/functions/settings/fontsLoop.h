// pid FONTS

void fontsLoop() {
  if (isSetup()) {
    String lines[] = {
      "(beta)",
      String(L->TXT_SETTINGS_FONT) + String(fontNames[currentFontIndex]),
    };
    centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
    drawHintSwitch();
    updateTimer();
  }

  if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
    currentFontIndex++;
    if (currentFontIndex == sizeof(systemFonts) / sizeof(systemFonts[0])) {
      currentFontIndex = 0;
    }
    applyFont(currentFontIndex);
    Serial.printf("Font: %d (%s)\n", currentFontIndex, fontNames[currentFontIndex]);

    setData("fontIndex", currentFontIndex);

    String lines[] = {
      "(beta)",
      String(L->TXT_SETTINGS_FONT) + String(fontNames[currentFontIndex]),
    };
    DISP.clear();
    centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
    drawHintSwitch();
  }

  checkExit(PID::SETTINGS);
}
