// PID::SYSTEM_INFO

void systemInfoLoop() {
	static bool animating = false;
	static bool fastAnimation = false;
	static int frame = 0;
	static int frames = 0;
	static int totalChars = 0;
	static int numLines = 0;
	static String lines[6];
	static int originX = 0;
	static int originY = 0;
	static float scaleX = 1.0f;
	static int fontHeight = 0;
	static int textStartY = 0;
	static int textAreaX = 0;
	static int frameTimer = 0;

	if (isSetup()) {
		int battery = DEVICE.Power.getBatteryLevel();

		int W = canvas.width();
		int H = canvas.height();

		int leftW = W * 40 / 100;
		scaleX = (float)(leftW * 80 / 100) / 18.0f;
		float scaleY = scaleX * 0.7f;
		int crystalW = (int)(18.0f * scaleX);
		int crystalH = (int)(18.0f * scaleY);
		originX = (leftW - crystalW) / 2;
		originY = (H - crystalH) / 2 - (int)(3.0f * scaleY);

		int logoLeft = originX + (int)(3.0f * scaleX);
		int logoRight = originX + (int)(21.0f * scaleX);
		int edgeGap = logoLeft; // gap between screen edge and logo

		// RAM across the internal heap and PSRAM (when present).
		uint32_t freeRam = ESP.getFreeHeap() + ESP.getFreePsram();
		uint32_t totalRam = ESP.getHeapSize() + ESP.getPsramSize();

		uint32_t sketchSize = ESP.getSketchSize();
		uint32_t sketchSpace = ESP.getFreeSketchSpace();
		if (sketchSpace == 0) sketchSpace = ESP.getFlashChipSize();
		uint32_t freeFlash = sketchSpace > sketchSize ? sketchSpace - sketchSize : 0;

		// Text occupies the right side.
		canvas.setTextSize(TINY_TEXT);
		String newLines[] = {
			"Crystal",
			"firmware",
			"v" + String(FIRMWARE_VERSION),
			String(battery) + "% " + String(DEVICE.Power.getBatteryVoltage()) + " V",
			"RAM " + String(freeRam / 1024U) + "/" + String(totalRam / 1024U) + " KB",
			"Flash " + String(freeFlash / 1024U) + "/" + String(sketchSpace / 1024U) + " KB",
		};
		numLines = sizeof(newLines) / sizeof(newLines[0]);
		for (int i = 0; i < numLines; i++) lines[i] = newLines[i];

		fontHeight = canvas.fontHeight();
		int textBlockH = numLines * fontHeight;
		textStartY = (H - textBlockH) / 2;
		// Offset text from the logo by the same gap the logo has from the edge.
		textAreaX = logoRight + edgeGap;

		totalChars = 0;
		for (int i = 0; i < numLines; i++) totalChars += lines[i].length();

		frames = max(CRYSTAL_SEGMENTS, totalChars);
		frame = 1;
		fastAnimation = false;
		updateTimer(&frameTimer);
		animating = true;
	}

	if (checkExit()) {
		animating = false;
		return;
	}

	// Non-blocking animation
	if (animating) {
		// Animation speed-up
		if (isBtnAWasPressed() || isKbEnterPressed()) {
			fastAnimation = true;
		}

		int frameDelay = fastAnimation ? 3 : 15;
		if (checkTimer(frameDelay, true, &frameTimer) && frame < frames) frame++;

		int segments = CRYSTAL_SEGMENTS * frame / frames;
		int charsShown = totalChars * frame / frames;

		canvas.clear();
		canvas.setTextColor(FGCOLOR, BGCOLOR);
		canvas.setTextSize(TINY_TEXT);

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

		if (frame >= frames) animating = false;
	}

	if (isWebDataRequested()) {
		int battery = DEVICE.Power.getBatteryLevel();
		char text[10];
		sprintf(text, "%d%%", battery);
		webData = generateWebData("function", generateFunctionElement(text, BIG_TEXT, "center"));
	}
}
