/**
 * Draws a horizontal progress bar onto the canvas (without pushing).
 * @param value    Current value.
 * @param maxValue Maximum value.
 * @param y        Y position of the bar on the canvas.
 * @param padding  Horizontal padding from canvas edges in pixels.
 * @param height   Bar height in pixels.
 */
void drawProgressBar(int value, int maxValue, int y, int height = 20, int padding = 20) {
	static const int innerPadding = 2;

	int barX = padding;
	int barW = canvas.width() - padding * 2;

	canvas.drawRect(barX, y, barW, height, FGCOLOR);

	int fillX = barX + innerPadding;
	int fillY = y + innerPadding;
	int fillMaxW = barW - innerPadding * 2;
	int fillH = height - innerPadding * 2;
	int fillW = (value >= maxValue) ? fillMaxW : (int)((float)value / maxValue * fillMaxW);
	if (fillW > 0) canvas.fillRect(fillX, fillY, fillW, fillH, FGCOLOR);
}
