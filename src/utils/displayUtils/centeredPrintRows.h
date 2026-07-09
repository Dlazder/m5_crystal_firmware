/**
 * Draws centered rows of text onto the canvas.
 * @param text      Array of strings to display.
 * @param arraySize Number of elements in the array.
 * @param textSize  Text size [TINY_TEXT; SMALL_TEXT; MEDIUM_TEXT; BIG_TEXT; HUGE_TEXT].
 * @param manual    If true, skips pushSprite — use when a drawHint* call follows to avoid double push.
 */
void centeredPrintRows(String text[], int arraySize, float textSize, bool manual = false) {
	canvas.clear();
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(textSize);

	int textHeight = arraySize * canvas.fontHeight();
	int offsetY = (canvas.height() - textHeight) / 2;

	for (int i = 0; i < arraySize; i++) {
		int textWidth = canvas.textWidth(text[i].c_str());
		int x = (canvas.width() - textWidth) / 2;
		// don't allow text to have x < 0 (+ a little margin)
		if (x < 5) x = 5;
		int y = offsetY + i * canvas.fontHeight();
		canvas.setCursor(x, y);
		canvas.print(text[i].c_str());
	}

	if (!manual) canvas.pushSprite(0, getStatusBarHeight());
}