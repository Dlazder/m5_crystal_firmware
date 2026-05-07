// pid 34

void systemInfoLoop() {

	if (isSetup()) {
		// fix: This code is almost identical to startupScreen, the coordinates are different. Remove duplication later
		String lines[] = {
			"Crystal",
			"firmware",
			"",
			"v" + String(FIRMWARE_VERSION),
		};
		int textHeight = 4 * DISP.fontHeight();
		int offsetY =  (DISP.height() - textHeight) / 2;
		DISP.setTextColor(FGCOLOR);
		DISP.setTextSize(MEDIUM_TEXT);
		DISP.setCursor(0, offsetY);

		for (int i = 0; i < 4; i++) {
			int textWidth = DISP.textWidth(lines[i].c_str());
			int paddingX = (DISP.width() - textWidth) / 2;
			int cursorY = DISP.getCursorY();
			DISP.setCursor(paddingX, cursorY);
			DISP.println(lines[i].c_str());
		}
	}

	checkExit();
}