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

	float scaleX = (float)(W * 33 / 100) / 18.0f;
	float scaleY = scaleX * 0.7f;
	int crystalH = (int)(18.0f * scaleY);
	// center crystal in upper half, same as original formula
	int originY = (H / 2 - crystalH) / 2 - (int)(3.0f * scaleY);

	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(MEDIUM_TEXT);

	int textBlockH = 3 * canvas.fontHeight();
	int textStartY = (H / 2) + (H / 2 - textBlockH) / 2;

	int originX = W / 2 - (int)(12.0f * scaleX);

	String labels[] = { "Crystal", "firmware", "v" + String(FIRMWARE_VERSION) };
	for (int seg = 1; seg <= CRYSTAL_SEGMENTS; seg++) {
		canvas.clear();
		_drawLogoSegment(originX, originY, scaleX, seg);
		for (int i = 0; i < 3; i++) {
			int tw = canvas.textWidth(labels[i].c_str());
			canvas.setCursor((W - tw) / 2, textStartY + i * canvas.fontHeight());
			canvas.print(labels[i].c_str());
		}
		canvas.pushSprite(0, 0);
		delay(70);
	}

	statusBar = statusBarOld;
	recreateCanvas();
}
