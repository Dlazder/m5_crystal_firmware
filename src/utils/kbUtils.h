#define KB_GYRO_MAX 0.8f
#define KB_GYRO_HYSTERESIS 0.08f

const int kbRowCount = 4;
const int kbColCounts[] = { 12, 12, 12, 12 };

const char kbNormal[4][12] = {
	{ '1','2','3','4','5','6','7','8','9','0','-','=' },
	{ 'q','w','e','r','t','y','u','i','o','p','[',']' },
	{ 'a','s','d','f','g','h','j','k','l',';','\'', '`'},
	{ 'z','x','c','v','b','n','m',',','.','?','/','\\'},
};

const char kbShifted[4][12] = {
	{ '!','@','#','$','%','^','&','*','(',')','_','+' },
	{ 'Q','W','E','R','T','Y','U','I','O','P','{','}' },
	{ 'A','S','D','F','G','H','J','K','L',':','"', '~'},
	{ 'Z','X','C','V','B','N','M','<','>','~','/', '|'},
};

const int kbSpecialRow = 0;
const int kbSpecialCount = 5;
const char* kbSpecialLabels[] = { "exit", "CAP", "ENT", "DEL", "SPC" };

// Keyboard state
char kbBuf[64];
int  kbLen = 0;
int  kbRow = 0;
int  kbCol = 0;
bool kbCapsOn = false;
int  kbBlinkCounter = 0;
bool kbCursorVisible = true;

void kbReset() {
	kbBuf[0] = '\0';
	kbLen = 0;
	kbRow = 0;
	kbCol = 0;
	kbCapsOn = false;
	kbBlinkCounter = 0;
	kbCursorVisible = true;
}

void drawKeyboardUi() {
	canvas.clear();
	canvas.setTextSize(SMALL_TEXT);

	int fontH = canvas.fontHeight();
	int specH = fontH + 6;
	int cellH = fontH + 1;

	// Input line
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	String display = String(kbBuf);
	if (display.length() > 14) display = display.substring(display.length() - 14);
	canvas.setCursor(4, 2);
	canvas.print(display + (kbCursorVisible ? "_" : " "));

	// Buffer indicator
	canvas.setTextSize(1);
	canvas.drawRightString(String(display.length()) + "/63", canvas.width() - 2, 2);
	canvas.setTextSize(SMALL_TEXT);

	// Special keys row
	int specY = 2 + fontH + 2;
	int specW = canvas.width() / kbSpecialCount;
	for (int i = 0; i < kbSpecialCount; i++) {
		int x       = i * specW;
		bool sel    = (kbRow == kbSpecialRow && kbCol == i);
		bool active = (i == 1 && kbCapsOn);
		if (sel || active) {
			canvas.fillRect(x, specY, specW - 1, specH, FGCOLOR);
			canvas.setTextColor(BGCOLOR, FGCOLOR);
		} else {
			canvas.drawRect(x, specY, specW - 1, specH, FGCOLOR);
			canvas.setTextColor(FGCOLOR, BGCOLOR);
		}
		int labelW = canvas.textWidth(kbSpecialLabels[i]);
		canvas.setCursor(x + (specW - labelW) / 2, specY + 3);
		canvas.print(kbSpecialLabels[i]);
	}

	// Keyboard grid
	int startY = specY + specH + 2;
	for (int r = 0; r < kbRowCount; r++) {
		int cols  = kbColCounts[r];
		int cellW = canvas.width() / cols;
		int y     = startY + r * cellH;
		for (int c = 0; c < cols; c++) {
			int x    = c * cellW;
			bool sel = (r + 1 == kbRow && c == kbCol);
			if (sel) {
				canvas.fillRect(x, y, cellW, cellH, FGCOLOR);
				canvas.setTextColor(BGCOLOR, FGCOLOR);
			} else {
				canvas.setTextColor(FGCOLOR, BGCOLOR);
			}
			char ch = kbCapsOn ? kbShifted[r][c] : kbNormal[r][c];
			canvas.setCursor(x + (cellW - canvas.textWidth(String(ch))) / 2, y + 1);
			canvas.print(ch);
		}
	}

	canvas.pushSprite(0, getStatusBarHeight());
}

// onExit  — called when "exit" pressed
// onEnter — called when "ENT" pressed, receives kbBuf
// onChar  — called for each typed/deleted char (pass nullptr if not needed)
// returns true if the caller should return immediately (exit or ENT was pressed)
bool keyboardLoop(
	void (*onExit)(),
	void (*onEnter)(const char* buf),
	void (*onChar)(char ch)
) {
	DEVICE.update();
	kbBlinkCounter++;
	if (kbBlinkCounter >= 100) {
		kbBlinkCounter   = 0;
		kbCursorVisible  = !kbCursorVisible;
		drawKeyboardUi();
	}

	// Gyro navigation
	if (kbGyroEnabled) {
		static float smoothX     = 0.0f;
		static float smoothY     = 0.0f;
		static float lastMappedX = 0.0f;
		static float lastMappedY = 0.0f;

		float accX, accY, accZ;
		DEVICE.Imu.getAccelData(&accX, &accY, &accZ);

		smoothX = smoothX * 0.7f + accX * 0.3f;
		smoothY = smoothY * 0.7f + accY * 0.3f;

		float tiltX = constrain(smoothY, -KB_GYRO_MAX, KB_GYRO_MAX) / KB_GYRO_MAX;
		float tiltY = constrain(smoothX, -KB_GYRO_MAX, KB_GYRO_MAX) / KB_GYRO_MAX;

		if (abs(tiltX - lastMappedX) > KB_GYRO_HYSTERESIS || abs(tiltY - lastMappedY) > KB_GYRO_HYSTERESIS) {
			lastMappedX = tiltX;
			lastMappedY = tiltY;

			int totalRows = kbRowCount + 1;
			int newRow    = constrain((int)((tiltY + 1.0f) / 2.0f * totalRows), 0, totalRows - 1);
			int colCount  = (newRow == kbSpecialRow) ? kbSpecialCount : kbColCounts[newRow - 1];
			int newCol    = constrain((int)((tiltX + 1.0f) / 2.0f * colCount), 0, colCount - 1);

			if (newRow != kbRow || newCol != kbCol) {
				kbRow = newRow;
				kbCol = newCol;
				drawKeyboardUi();
			}
		}
	}

	if (isBtnAWasPressed() && checkTimer(150, true)) {
		if (kbRow == kbSpecialRow) {
			if (kbCol == 0) {
				onExit();
				return true;
			} else if (kbCol == 1) {
				kbCapsOn = !kbCapsOn;
			} else if (kbCol == 2) {
				onEnter(kbBuf);
				return true;
			} else if (kbCol == 3) {
				if (kbLen > 0) { kbLen--; kbBuf[kbLen] = '\0'; }
			} else {
				if (kbLen < 63) { kbBuf[kbLen++] = ' '; kbBuf[kbLen] = '\0'; }
				if (onChar) onChar(' ');
			}
		} else {
			char ch = kbCapsOn ? kbShifted[kbRow - 1][kbCol] : kbNormal[kbRow - 1][kbCol];
			if (kbLen < 63) { kbBuf[kbLen++] = ch; kbBuf[kbLen] = '\0'; }
			if (onChar) onChar(ch);
		}
		drawKeyboardUi();
	}

	if (isBtnBWasPressed() && checkTimer(150, true)) {
		if (kbGyroEnabled) {
			if (kbLen > 0) { kbLen--; kbBuf[kbLen] = '\0'; }
			if (onChar) onChar('\b');
		} else {
			kbCol = (kbRow == kbSpecialRow)
				? (kbCol + 1) % kbSpecialCount
				: (kbCol + 1) % kbColCounts[kbRow - 1];
		}
		drawKeyboardUi();
	}

	if (isBtnPWRWasPressed() && checkTimer(150, true)) {
		if (kbGyroEnabled) {
			kbCapsOn = !kbCapsOn;
		} else {
			kbRow = (kbRow + 1) % (kbRowCount + 1);
			kbCol = min(kbCol,
				(kbRow == kbSpecialRow) ? kbSpecialCount - 1 : kbColCounts[kbRow - 1] - 1);
		}
		drawKeyboardUi();
	}

	return false;
}
