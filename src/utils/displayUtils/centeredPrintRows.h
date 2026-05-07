void centeredPrintRows(String text[], int arraySize, float textSize) {
	canvas.clear();
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(textSize);

	int canvasHeight = canvas.height() - getStatusBarOffset();
	int textHeight = arraySize * canvas.fontHeight();
	int offsetY = (canvasHeight - textHeight) / 2;

	for (int i = 0; i < arraySize; i++) {
		int textWidth = canvas.textWidth(text[i].c_str());
		int x = (canvas.width() - textWidth) / 2;
		int y = offsetY + i * canvas.fontHeight();
		canvas.setCursor(x, y);
		canvas.print(text[i].c_str());
	}

	canvas.pushSprite(0, getStatusBarOffset());
}