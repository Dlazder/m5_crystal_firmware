// PID::COLORS

void drawColorsUI() {
	canvas.clear();

	int count = sizeof(colors) / sizeof(colors[0]);
	int squareSize = 10;
	int gap = 4;
	int borderPad = 2;
	int totalWidth = count * squareSize + (count - 1) * gap;
	int startX = (canvas.width() - totalWidth) / 2;

	canvas.setTextSize(MEDIUM_TEXT);
	int textY = canvas.height() / 2 - canvas.fontHeight();
	int squaresY = canvas.height() / 2 + gap;

	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.drawCenterString(colorsEntry[colorIndex], canvas.width() / 2, textY);

	for (int i = 0; i < count; i++) {
		int x = startX + i * (squareSize + gap);
		canvas.fillRect(x, squaresY, squareSize, squareSize, colors[i]);
		if (i == colorIndex) {
			canvas.drawRect(x - borderPad, squaresY - borderPad, squareSize + borderPad * 2, squareSize + borderPad * 2, FGCOLOR);
		}
	}
}

void colorsLoop() {
	if (isSetup()) {
		drawColorsUI();
		drawHintRange();
		updateTimer();
	}
	int colorCount = sizeof(colors) / sizeof(colors[0]);

	if ((isBtnAWasPressed() || isKbRightPressed()) && checkTimer(100)) {
		colorIndex = (colorIndex + 1) % colorCount;
		FGCOLOR = colors[colorIndex];
		setData("colorIndex", colorIndex);
		drawColorsUI();
		drawHintRange();
	}

	if ((isBtnPWRWasPressed() || isKbLeftPressed()) && checkTimer(100)) {
		colorIndex = (colorIndex + colorCount - 1) % colorCount;
		FGCOLOR = colors[colorIndex];
		setData("colorIndex", colorIndex);
		drawColorsUI();
		drawHintRange();
	}
	checkExit();
}
