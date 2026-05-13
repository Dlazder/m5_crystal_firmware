// Keyboard implementation for devices with a physical keyboard (e.g. Cardputer).
// Included by kbUtils.h — do not include directly.

void drawKeyboardUi() {
	canvas.clear();
	canvas.setTextSize(MEDIUM_TEXT);
	canvas.setFont(systemFonts[currentFontIndex]);
	canvas.setTextColor(FGCOLOR, BGCOLOR);

	// Scroll window so cursor stays visible (max ~14 chars wide)
	const int kbDisplayWidth = 14;
	int scrollOffset = 0;
	if (kbCursorPos > kbDisplayWidth - 1)
		scrollOffset = kbCursorPos - (kbDisplayWidth - 1);

	String before = String(kbBuf).substring(scrollOffset, kbCursorPos);
	String after  = String(kbBuf).substring(kbCursorPos);

	canvas.setCursor(4, 4);
	canvas.print(before + (kbCursorVisible ? "|" : " ") + after);

	canvas.setTextSize(TINY_TEXT);
	canvas.drawRightString(String(kbLen) + "/63", canvas.width() - 2, 2);
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

	if (kbCursorLeftPressed) {
		kbCursorLeftPressed = false;
		if (kbCursorPos > 0) kbCursorPos--;
		changed = true;
	}
	if (kbCursorRightPressed) {
		kbCursorRightPressed = false;
		if (kbCursorPos < kbLen) kbCursorPos++;
		changed = true;
	}
	if (kbDelPressed) {
		kbDelPressed = false;
		if (kbCursorPos > 0) {
			memmove(kbBuf + kbCursorPos - 1, kbBuf + kbCursorPos, kbLen - kbCursorPos + 1);
			kbLen--;
			kbCursorPos--;
		}
		if (onChar) onChar('\b');
		changed = true;
	}
	if (kbEnterPressed) {
		kbEnterPressed = false;
		kbEnd();
		onEnter(kbBuf);
		return true;
	}
	if (kbEscPressed) {
		kbEscPressed = false;
		kbEnd();
		onExit();
		return true;
	}
	for (char c : kbWord) {
		if (kbLen < 63) {
			memmove(kbBuf + kbCursorPos + 1, kbBuf + kbCursorPos, kbLen - kbCursorPos + 1);
			kbBuf[kbCursorPos] = c;
			kbLen++;
			kbCursorPos++;
		}
		if (onChar) onChar(c);
		changed = true;
	}
	kbWord = "";
	if (changed) drawKeyboardUi();

	return false;
}
