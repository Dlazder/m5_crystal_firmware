// Draws a hint string at the bottom of the canvas in gray, without clearing it.
void _drawHintText(const char* hint) {
	canvas.setTextColor(TFT_DARKGREY, BGCOLOR);
	canvas.setTextSize(1);

	int textWidth = canvas.textWidth(hint);
	int x = (canvas.width() - textWidth) / 2;
	int y = canvas.height() - canvas.fontHeight() - 2;

	canvas.setCursor(x, y);
	canvas.print(hint);
	canvas.pushSprite(0, getStatusBarHeight());

	canvas.setTextColor(FGCOLOR, BGCOLOR);
}

// Hint for clock settings: select field and adjust value
void drawHintClock() {
	#if HAS_PHYSICAL_KB
		String hint = String(L->TXT_LEFT) + "/" + String(L->TXT_RIGHT) + ": " + String(L->TXT_SELECT) + "   +/-: " + String(L->TXT_ADJUST);
		_drawHintText(hint.c_str());
	#else
		String hint = String("B: ") + String(L->TXT_SELECT) + "   A/PWR: " + String(L->TXT_ADJUST);
		_drawHintText(hint.c_str());
	#endif
}

// Hint for cycling through a range of values (brightness, color, font, etc.)
// StickC Plus2: "press A"   Cardputer: "left / right"
void drawHintRange() {
#if HAS_PHYSICAL_KB
	String hint = String(L->TXT_LEFT) + " / " + String(L->TXT_RIGHT);
	_drawHintText(hint.c_str());
#else
	_drawHintText(L->TXT_PRESS_A);
#endif
}