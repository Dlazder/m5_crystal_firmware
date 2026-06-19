/**
 * Renders a loaded script with the current line highlighted.
 * All lines are drawn dark grey; the line at currentLine is green and anchored
 * at a fixed row (the 4th line) so the script scrolls past it.
 * Shared by the BadBLE and BadUSB loops.
 *
 * @param lines        array of script lines
 * @param lineCount    number of lines in the array
 * @param currentLine  index of the line to highlight (the one about to run)
 */
void drawScript(String* lines, int lineCount, int currentLine) {
  if (lines == nullptr || lineCount == 0) return;

  canvas.clear();
  canvas.setTextSize(SMALL_TEXT);

  int lineHeight = canvas.fontHeight();

  const int paddingX = 5;
  const int maxChars = 20;
  const int anchorRow = 3; // current line settles on the 4th row once reached

  int total = lineCount;
  int current = min(currentLine, total - 1);

  // The row the current line occupies: it climbs from the top to anchorRow,
  // then stays fixed so the script scrolls past it (no empty rows on top).
  int currentRow = min(current, anchorRow);

  for (int i = 0; i < total; i++) {
    int y = (currentRow + (i - current)) * lineHeight;
    if (y + lineHeight <= 0 || y >= canvas.height()) continue; // off-screen

    String text = lines[i];
    if ((int)text.length() > maxChars) text = text.substring(0, maxChars);

    canvas.setTextColor(i == current ? FGCOLOR : TFT_DARKGREY, BGCOLOR);
    canvas.setCursor(paddingX, y);
    canvas.print(text.c_str());
  }

  canvas.pushSprite(0, getStatusBarOffset());
}
