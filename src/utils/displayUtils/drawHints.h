// Arrow tokens that can be embedded inline in a hint string; each is replaced
// by a triangle glyph when drawn. Order matches the _drawArrowGlyph cases.
static const char* const ARROW_TOKENS[] = { "__LEFT__", "__RIGHT__", "__UP__", "__DOWN__" };

enum {
	HINT_ARROW_LEFT = 0,
	HINT_ARROW_RIGHT,
	HINT_ARROW_UP,
	HINT_ARROW_DOWN,
};

// Returns 0..3 if `s` starts with the corresponding arrow token, else -1.
int _matchArrowToken(const char* s) {
	for (int i = 0; i < 4; i++) {
		if (strncmp(s, ARROW_TOKENS[i], strlen(ARROW_TOKENS[i])) == 0) return i;
	}
	return -1;
}

// Draws a single arrow glyph centered at (cx, cy).
void _drawArrowGlyph(int cx, int cy, int dir) {
	int half = canvas.fontHeight() * 3 / 10; // ~0.3 * line height
	uint16_t c = TFT_DARKGREY;
	switch (dir) {
		case HINT_ARROW_LEFT:
			canvas.fillTriangle(cx - half, cy, cx + half, cy - half, cx + half, cy + half, c);
			break;
		case HINT_ARROW_RIGHT:
			canvas.fillTriangle(cx + half, cy, cx - half, cy - half, cx - half, cy + half, c);
			break;
		case HINT_ARROW_UP:
			canvas.fillTriangle(cx, cy - half, cx - half, cy + half, cx + half, cy + half, c);
			break;
		case HINT_ARROW_DOWN:
			canvas.fillTriangle(cx, cy + half, cx - half, cy - half, cx + half, cy - half, c);
			break;
	}
}

// Draws a centered hint line. Arrow keys are written inline as tokens and
// rendered as triangle glyphs; everything else is printed as text:
// drawHint("__LEFT__ __RIGHT__ change");
// drawHint("__LEFT__ __RIGHT__ move   __UP__ __DOWN__ select");
void drawHint(const char* hint) {
	canvas.setTextColor(TFT_DARKGREY, BGCOLOR);
	canvas.setTextSize(1);

	int fh = canvas.fontHeight();
	int arrowW = fh * 3 / 5; // arrow glyph width (2 * half); spacing comes from spaces

	// Measure total width to center the whole line.
	int totalW = 0;
	for (const char* p = hint; *p;) {
		int a = _matchArrowToken(p);
		if (a >= 0) {
			totalW += arrowW;
			p += strlen(ARROW_TOKENS[a]);
		} else {
			const char* q = p;
			while (*q && _matchArrowToken(q) < 0) q++;
			totalW += canvas.textWidth(String(p, q - p).c_str());
			p = q;
		}
	}

	int x = (canvas.width() - totalW) / 2;
	int y = canvas.height() - fh - 2;
	int cy = y + fh / 2;

	// Draw arrows and text runs left to right.
	for (const char* p = hint; *p;) {
		int a = _matchArrowToken(p);
		if (a >= 0) {
			_drawArrowGlyph(x + arrowW / 2, cy, a);
			x += arrowW;
			p += strlen(ARROW_TOKENS[a]);
		} else {
			const char* q = p;
			while (*q && _matchArrowToken(q) < 0) q++;
			String run(p, q - p);
			canvas.setCursor(x, y);
			canvas.print(run);
			x += canvas.textWidth(run.c_str());
			p = q;
		}
	}

	canvas.pushSprite(0, getStatusBarHeight());
	canvas.setTextColor(FGCOLOR, BGCOLOR);
}

void drawHintCustom(const char* hint_keyboard, const char* hint_buttons) {
	#if HAS_PHYSICAL_KB
		drawHint(hint_keyboard);
	#else
		drawHint(hint_buttons);
	#endif
}

// Hint for single-action toggle (rotation, etc.)
void drawHintSwitch() {
	#if HAS_PHYSICAL_KB
		String hint = String("enter: ") + String(L->TXT_SWITCH);
		drawHint(hint.c_str());
	#else
		String hint = String("A: ") + String(L->TXT_SWITCH);
		drawHint(hint.c_str());
	#endif
}

// Hint for cycling through a range of values (brightness, color, volume, etc.)
void drawHintRange() {
#if HAS_PHYSICAL_KB
	String hint = String("__LEFT__ __RIGHT__ ") + String(L->TXT_ADJUST);
	drawHint(hint.c_str());
#else
	drawHint(L->TXT_PRESS_A);
#endif
}
