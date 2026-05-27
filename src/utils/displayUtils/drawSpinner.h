static int _spinnerTimer = 0;
static int _spinnerTick = 0;

/**
 * Draws a filled animated spinner onto the canvas (manual mode — no pushSprite).
 * Use after centeredPrintRows(..., true), then call canvas.pushSprite() yourself.
 */
void drawSpinner() {
	if (checkTimer(22, true, &_spinnerTimer)) {
		_spinnerTick = (_spinnerTick + 1) % 36;
	}

	const int r = 8;
	const int margin = 8;
	int cx = canvas.width() - r - margin;
	int cy = canvas.height() - r - margin;
	canvas.fillArc(cx, cy, r, r * 2 / 3, _spinnerTick * 10, _spinnerTick * 10 + 270, FGCOLOR);
}
