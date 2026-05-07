
/**
 * print spaces on screen to clear screen
 * (will be removed in future probably)
 */
void clearScreenWithSymbols() {
  DISP.setTextColor(FGCOLOR, BGCOLOR);
  DISP.setTextSize(MEDIUM_TEXT);
  for (int i = 0; i < 9; i++) {
    DISP.println("                    ");
  }
}

/**
 * print text on center of the screen
 * @param textSize [SMALL_TEXT; MEDIUM_TEXT; BIG_TEXT; HUGE_TEXT] - (1; 2; 3; 4)
 * @param newLine if we printing second text row
 */
void centeredPrint(String text, int textSize, bool newLine = false) {
  if (text.length() == 0) return;

  canvas.clear();
  canvas.setTextColor(FGCOLOR, BGCOLOR);
  canvas.setTextSize(textSize);
  
  int textWidth = canvas.textWidth(text.c_str());
  int textHeight = canvas.fontHeight();
  int offsetX = (canvas.width() - textWidth) / 2;
  int offsetY = (canvas.height() - textHeight) / 2 - (getStatusBarHeight() / 2);
  
  canvas.setCursor(offsetX, offsetY);
  canvas.println(text.c_str());
  canvas.pushSprite(0, getStatusBarHeight());
}

/**
 * Places the cursor at the top of the accessible area
 * (depending on the presence of the status bar)
 */
void cursorOnTop() {
  if (statusBar) {
    DISP.setCursor(0, getStatusBarOffset());
  } else {
    DISP.setCursor(0, 0);
  }
}