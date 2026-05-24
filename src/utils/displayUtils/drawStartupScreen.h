// Returns the bottom Y coordinate of the logo after drawing.
int _drawLogo(int originY) {
	int W = canvas.width();
	int cx = W / 2;

	float scaleX = (float)(W * 33 / 100) / 18.0f;
	float scaleY = scaleX * 0.7f;
	int originX = cx - (int)(12.0f * scaleX);

	auto sx = [&](float v) { return originX + (int)(v * scaleX); };
	auto sy = [&](float v) { return originY + (int)(v * scaleY); };

	int x3 = sx(3), x7 = sx(7), x10 = sx(10), x12 = sx(12);
	int x14 = sx(14), x17 = sx(17), x21 = sx(21);
	int ty = sy(3), gy = sy(9), by = sy(21);

	struct Line { int x0, y0, x1, y1; };

	Line outline[] = {
		{ x7, ty, x17, ty },
		{ x17, ty, x21, gy },
		{ x21, gy, x12, by },
		{ x12, by, x3, gy },
		{ x3, gy, x7, ty },
	};
	Line inner[] = {
		{ x10, ty, x7, gy },
		{ x7, gy, x12, by },
		{ x12, by, x17, gy },
		{ x17, gy, x14, ty },
		{ x14, ty, x10, ty },
		{ x3, gy, x21, gy },
	};

	int numOutline = sizeof(outline) / sizeof(outline[0]);
	int numInner = sizeof(inner) / sizeof(inner[0]);

	auto drawLines = [&](Line* arr, int n, int drawn) {
		for (int i = 0; i < drawn; i++)
			canvas.drawLine(arr[i].x0, arr[i].y0, arr[i].x1, arr[i].y1, FGCOLOR);
	};

	for (int i = 0; i < numOutline; i++) {
		canvas.clear();
		drawLines(outline, numOutline, i + 1);
		canvas.pushSprite(0, 0);
		delay(70);
	}

	for (int i = 0; i < numInner; i++) {
		canvas.clear();
		drawLines(outline, numOutline, numOutline);
		drawLines(inner, numInner, i + 1);
		canvas.pushSprite(0, 0);
		delay(70);
	}

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

	int by = _drawLogo(originY);
	int textBlockH = 3 * canvas.fontHeight();
	int textStartY = by + (H - by - textBlockH) / 2;

	String labels[] = { "Crystal", "firmware", "v" + String(FIRMWARE_VERSION) };
	for (int i = 0; i < 3; i++) {
		int tw = canvas.textWidth(labels[i].c_str());
		canvas.setCursor((W - tw) / 2, textStartY + i * canvas.fontHeight());
		canvas.print(labels[i].c_str());
	}

	canvas.pushSprite(0, 0);

	statusBar = statusBarOld;
	recreateCanvas();
}
