// Total number of line segments the crystal logo is composed of.
#define CRYSTAL_SEGMENTS 11

// Non-blocking: draws the first `segments` line segments of the crystal logo at
// (originX, originY) using the given horizontal scale. Pass CRYSTAL_SEGMENTS
// (or more) to draw the whole logo. Does not clear or push the sprite; the
// caller owns the animation loop and pushSprite. Returns the bottom Y of the logo.
int _drawLogoSegment(int originX, int originY, float scaleX, int segments) {
	float scaleY = scaleX * 0.7f;

	auto sx = [&](float v) { return originX + (int)(v * scaleX); };
	auto sy = [&](float v) { return originY + (int)(v * scaleY); };

	int x3 = sx(3), x7 = sx(7), x10 = sx(10), x12 = sx(12);
	int x14 = sx(14), x17 = sx(17), x21 = sx(21);
	int ty = sy(3), gy = sy(9), by = sy(21);

	struct Line { int x0, y0, x1, y1; };

	// outline (5) followed by inner (6) = CRYSTAL_SEGMENTS
	Line lines[] = {
		{ x7, ty, x17, ty },
		{ x17, ty, x21, gy },
		{ x21, gy, x12, by },
		{ x12, by, x3, gy },
		{ x3, gy, x7, ty },
		{ x10, ty, x7, gy },
		{ x7, gy, x12, by },
		{ x12, by, x17, gy },
		{ x17, gy, x14, ty },
		{ x14, ty, x10, ty },
		{ x3, gy, x21, gy },
	};

	if (segments > CRYSTAL_SEGMENTS) segments = CRYSTAL_SEGMENTS;
	for (int i = 0; i < segments; i++)
		canvas.drawLine(lines[i].x0, lines[i].y0, lines[i].x1, lines[i].y1, FGCOLOR);

	return by;
}

void drawStartupScreen() {
	bool statusBarOld = statusBar;
	statusBar = false;
	recreateCanvas();

	int W = canvas.width();
	int H = canvas.height();

	// Logo on the left, text on the right (same layout as systemInfoLoop).
	int leftW = W * 40 / 100;
	float scaleX = (float)(leftW * 80 / 100) / 18.0f;
	float scaleY = scaleX * 0.7f;
	int crystalW = (int)(18.0f * scaleX);
	int crystalH = (int)(18.0f * scaleY);
	int originX = (leftW - crystalW) / 2;
	int originY = (H - crystalH) / 2 - (int)(3.0f * scaleY);

	int logoLeft = originX + (int)(3.0f * scaleX);
	int logoRight = originX + (int)(21.0f * scaleX);
	int edgeGap = logoLeft; // gap between screen edge and logo

	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(MEDIUM_TEXT);

	int fontHeight = canvas.fontHeight();
	String labels[] = { "Crystal", "firmware", "v" + String(FIRMWARE_VERSION) };
	int numLines = sizeof(labels) / sizeof(labels[0]);

	int textBlockH = numLines * fontHeight;
	int textStartY = (H - textBlockH) / 2;
	// Offset text from the logo by the same gap the logo has from the edge.
	int textAreaX = logoRight + edgeGap;

	int totalChars = 0;
	for (int i = 0; i < numLines; i++) totalChars += labels[i].length();

	// Animate crystal segments and text characters in parallel, finishing
	// together (neofetch style typewriter effect).
	int frames = max(CRYSTAL_SEGMENTS, totalChars);
	for (int f = 1; f <= frames; f++) {
		int segments = CRYSTAL_SEGMENTS * f / frames;
		int charsShown = totalChars * f / frames;

		canvas.clear();
		canvas.setTextColor(FGCOLOR, BGCOLOR);
		canvas.setTextSize(MEDIUM_TEXT);

		_drawLogoSegment(originX, originY, scaleX, segments);

		// Print the first `charsShown` characters across the lines,
		// left-aligned within the text area.
		int remaining = charsShown;
		for (int i = 0; i < numLines; i++) {
			int len = labels[i].length();
			int take = remaining < len ? remaining : len;
			if (take > 0) {
				canvas.setCursor(textAreaX, textStartY + i * fontHeight);
				canvas.print(labels[i].substring(0, take).c_str());
			}
			remaining -= take;
			if (remaining <= 0) break;
		}

		canvas.pushSprite(0, 0);
		delay(30);
	}

	statusBar = statusBarOld;
	recreateCanvas();
}
