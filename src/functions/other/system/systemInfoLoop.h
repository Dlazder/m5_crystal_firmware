// pid 34

void systemInfoLoop() {

	if (isSetup()) {
		int W = canvas.width();
		int H = canvas.height();

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

		// Text occupies the right side.
		canvas.setTextSize(MEDIUM_TEXT);
		String lines[] = {
			"Crystal",
			"firmware",
			"v" + String(FIRMWARE_VERSION),
		};
		int numLines = sizeof(lines) / sizeof(lines[0]);
		int fontHeight = canvas.fontHeight();
		int textBlockH = numLines * fontHeight;
		int textStartY = (H - textBlockH) / 2;
		// Offset text from the logo by the same gap the logo has from the edge.
		int textAreaX = logoRight + edgeGap;

		int totalChars = 0;
		for (int i = 0; i < numLines; i++) totalChars += lines[i].length();

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
				int len = lines[i].length();
				int take = remaining < len ? remaining : len;
				if (take > 0) {
					canvas.setCursor(textAreaX, textStartY + i * fontHeight);
					canvas.print(lines[i].substring(0, take).c_str());
				}
				remaining -= take;
				if (remaining <= 0) break;
			}

			canvas.pushSprite(0, getStatusBarHeight());
			delay(70);
		}
	}

	checkExit();
}
