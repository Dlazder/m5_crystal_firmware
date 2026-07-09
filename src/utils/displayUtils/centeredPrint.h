/**
 * print text on center of the screen
 * @param textSize [SMALL_TEXT; MEDIUM_TEXT; BIG_TEXT; HUGE_TEXT] - (1; 2; 3; 4)
 */
void centeredPrint(String text, float textSize, bool manual = false) {
  if (text.length() == 0) return;

  canvas.clear();
  canvas.setTextColor(FGCOLOR, BGCOLOR);
  canvas.setTextSize(textSize);

  int textWidth = canvas.textWidth(text.c_str());
  int textHeight = canvas.fontHeight();
  int x = (canvas.width() - textWidth) / 2;
  int y = (canvas.height() - textHeight) / 2;

  // don't allow text to have x < 0 (+ a little margin)
  if (x < 5) x = 5;

  canvas.setCursor(x, y);
  canvas.println(text.c_str());
  if (!manual) canvas.pushSprite(0, getStatusBarHeight());
}
