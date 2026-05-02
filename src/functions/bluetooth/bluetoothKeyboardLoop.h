// pid PID::BT_KEYBOARD

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
void drawKeyboardUi(int row, int col, const char* inputBuf, bool capsOn, bool cursorVisible) {
	canvas.clear();
	canvas.setTextSize(SMALL_TEXT);

	int fontH = canvas.fontHeight();
	int specH = fontH + 6;
	int cellH = fontH + 1;

	// Input line
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	String display = String(inputBuf);
	if (display.length() > 14) display = display.substring(display.length() - 14);
	canvas.setCursor(4, 2);
	canvas.print(display + (cursorVisible ? "_" : " "));

	// Buffer indicator
	canvas.setTextSize(1);
	canvas.drawRightString(String(display.length()) + "/63", canvas.width() - 2, 2);
	canvas.setTextSize(SMALL_TEXT);

	// Special keys row
	int specY = 2 + fontH + 2;
	int specW = canvas.width() / kbSpecialCount;
	for (int i = 0; i < kbSpecialCount; i++) {
		int x = i * specW;
		bool sel = (row == kbSpecialRow && col == i);
		bool active = (i == 1 && capsOn);
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
		int cols = kbColCounts[r];
		int cellW = canvas.width() / cols;
		int y = startY + r * cellH;
		for (int c = 0; c < cols; c++) {
			int x = c * cellW;
			bool sel = (r + 1 == row && c == col);
			if (sel) {
				canvas.fillRect(x, y, cellW, cellH, FGCOLOR);
				canvas.setTextColor(BGCOLOR, FGCOLOR);
			} else {
				canvas.setTextColor(FGCOLOR, BGCOLOR);
			}
			char ch = capsOn ? kbShifted[r][c] : kbNormal[r][c];
			canvas.setCursor(x + (cellW - canvas.textWidth(String(ch))) / 2, y + 1);
			canvas.print(ch);
		}
	}

	canvas.pushSprite(0, getStatusBarHeight());
}

#define KB_GYRO_MAX 0.8f
#define KB_GYRO_HYSTERESIS 0.08f

void bluetoothKeyboardLoop() {
	static bool isBleConnected = false;
	static char inputBuf[64];
	static int inputLen = 0;
	static int cursorRow = 0;
	static int cursorCol = 0;
	static bool capsOn = false;
	static int blinkCounter = 0;
	static bool cursorVisible = true;

	if (isSetup()) {
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		inputLen = 0;
		inputBuf[0] = '\0';
		cursorRow = 0;
		cursorCol = 0;
		capsOn = false;
		isBleConnected = false;
		blinkCounter = 0;
		cursorVisible = true;
		centeredPrint("Waiting connection", SMALL_TEXT);
		updateTimer();
	}
	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			isBleConnected = true;
			drawKeyboardUi(cursorRow, cursorCol, inputBuf, capsOn, cursorVisible);
			DEVICE.Speaker.tone(2000, 200);
		}
	} else {
		if (isBleConnected) {
			isBleConnected = false;
			centeredPrint("Not connected", SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 200);
		}
		checkExit();
		return;
	}

	DEVICE.update();

	if (isBleConnected) {
		blinkCounter++;
		if (blinkCounter >= 100) {
			blinkCounter = 0;
			cursorVisible = !cursorVisible;
			drawKeyboardUi(cursorRow, cursorCol, inputBuf, capsOn, cursorVisible);
		}
	}

	// Gyro navigation
	if (kbGyroEnabled) {
		static float smoothX = 0.0f;
		static float smoothY = 0.0f;
		static float lastMappedX = 0.0f;
		static float lastMappedY = 0.0f;

		float accX, accY, accZ;
		DEVICE.Imu.getAccelData(&accX, &accY, &accZ);

		// Low-pass filter to smooth out sensor noise
		smoothX = smoothX * 0.7f + accX * 0.3f;
		smoothY = smoothY * 0.7f + accY * 0.3f;

		float tiltX = constrain(smoothY, -KB_GYRO_MAX, KB_GYRO_MAX) / KB_GYRO_MAX;
		float tiltY = constrain(smoothX, -KB_GYRO_MAX, KB_GYRO_MAX) / KB_GYRO_MAX;

		// Only update if tilt changed enough (hysteresis)
		if (abs(tiltX - lastMappedX) > KB_GYRO_HYSTERESIS || abs(tiltY - lastMappedY) > KB_GYRO_HYSTERESIS) {
			lastMappedX = tiltX;
			lastMappedY = tiltY;

			int totalRows = kbRowCount + 1;
			int newRow = (int)((tiltY + 1.0f) / 2.0f * totalRows);
			newRow = constrain(newRow, 0, totalRows - 1);

			int colCount = (newRow == kbSpecialRow) ? kbSpecialCount : kbColCounts[newRow - 1];
			int newCol = (int)((tiltX + 1.0f) / 2.0f * colCount);
			newCol = constrain(newCol, 0, colCount - 1);

			if (newRow != cursorRow || newCol != cursorCol) {
				cursorRow = newRow;
				cursorCol = newCol;
				drawKeyboardUi(cursorRow, cursorCol, inputBuf, capsOn, cursorVisible);
			}
		}
	}

	if (isBtnAWasPressed() && checkTimer(150, true)) {
		if (cursorRow == kbSpecialRow) {
			if (cursorCol == 0) {
				isBleConnected = false;
				DISP.clear();
				changeProcess(PID::BLUETOOTH);
				return;
			} else if (cursorCol == 1) {
				capsOn = !capsOn;
			} else if (cursorCol == 2) {
				bleKeyboard.write(KEY_RETURN);
			} else if (cursorCol == 3) {
				if (inputLen > 0) {
					inputLen--;
					inputBuf[inputLen] = '\0';
					bleKeyboard.write(KEY_BACKSPACE);
				}
			} else {
				bleKeyboard.write(' ');
				if (inputLen < 63) {
					inputBuf[inputLen++] = ' ';
					inputBuf[inputLen] = '\0';
				}
			}
		} else {
			char ch = capsOn ? kbShifted[cursorRow - 1][cursorCol] : kbNormal[cursorRow - 1][cursorCol];
			if (inputLen < 63) {
				bleKeyboard.write((uint8_t)ch);
				inputBuf[inputLen++] = ch;
				inputBuf[inputLen] = '\0';
			}
		}
		drawKeyboardUi(cursorRow, cursorCol, inputBuf, capsOn, cursorVisible);
	}

	if (isBtnBWasPressed() && checkTimer(150, true)) {
		if (kbGyroEnabled) {
			if (inputLen > 0) {
				inputLen--;
				inputBuf[inputLen] = '\0';
				bleKeyboard.write(KEY_BACKSPACE);
			}
		} else {
			if (cursorRow == kbSpecialRow) {
				cursorCol = (cursorCol + 1) % kbSpecialCount;
			} else {
				cursorCol = (cursorCol + 1) % kbColCounts[cursorRow - 1];
			}
		}
		drawKeyboardUi(cursorRow, cursorCol, inputBuf, capsOn, cursorVisible);
	}

	if (isBtnPWRWasPressed() && checkTimer(150, true)) {
		if (kbGyroEnabled) {
			capsOn = !capsOn;
		} else {
			cursorRow = (cursorRow + 1) % (kbRowCount + 1);
			if (cursorRow == kbSpecialRow) {
				cursorCol = min(cursorCol, kbSpecialCount - 1);
			} else {
				cursorCol = min(cursorCol, kbColCounts[cursorRow - 1] - 1);
			}
		}
		drawKeyboardUi(cursorRow, cursorCol, inputBuf, capsOn, cursorVisible);
	}

	if (!bleCompositeBegan) checkExit();
}
