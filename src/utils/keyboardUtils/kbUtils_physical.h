// Keyboard implementation for devices with a physical keyboard (e.g. Cardputer).
// Included by kbUtils.h — do not include directly.

void drawKeyboardUi() {
	canvas.clear();
	canvas.setTextSize(MEDIUM_TEXT);

	canvas.setFont(systemFonts[currentFontIndex]);
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	String display = String(kbBuf);
	canvas.setCursor(4, 4);
	canvas.print(display + (kbCursorVisible ? "_" : " "));
	canvas.setTextSize(TINY_TEXT);
	canvas.drawRightString(String(display.length()) + "/63", canvas.width() - 2, 2);
	canvas.setTextSize(MEDIUM_TEXT);

	canvas.pushSprite(0, getStatusBarHeight());
}

bool keyboardLoop(
	void (*onExit)(),
	void (*onEnter)(const char* buf),
	void (*onChar)(char ch)
) {
	kbBlinkCounter++;
	if (kbBlinkCounter >= 100) {
		kbBlinkCounter  = 0;
		kbCursorVisible = !kbCursorVisible;
		drawKeyboardUi();
	}

	cardputerKbUpdate();

	bool changed = false;
	if (kbDelPressed) {
		kbDelPressed = false;
		if (kbLen > 0) { kbLen--; kbBuf[kbLen] = '\0'; }
		if (onChar) onChar('\b');
		changed = true;
	}
	if (kbEnterPressed) {
		kbEnterPressed = false;
		onEnter(kbBuf);
		return true;
	}
	if (kbEscPressed) {
		kbEscPressed = false;
		onExit();
		return true;
	}
	for (char c : kbWord) {
		if (kbLen < 63) { kbBuf[kbLen++] = c; kbBuf[kbLen] = '\0'; }
		if (onChar) onChar(c);
		changed = true;
	}
	kbWord = "";
	if (changed) drawKeyboardUi();

	return false;
}
