float filteredAngle = 0;
const float ALPHA = 0.3;

int lastAngle = 0;
bool levelToolSoundPlayed = false;

void levelToolLoop() {
	if (!hasImu) {
		if (isSetup()) centeredPrint("no gyroscope", MEDIUM_TEXT);
		checkExit();
		return;
	}

	if (isSetup()) {
		deviceSpeakerBegin();
	}

	float accX, accY, accZ;
	float angleRoll;

	DEVICE.Imu.getAccelData(&accX, &accY, &accZ);
	angleRoll = IMU_LEVEL_ROLL(accX, accY);

	// Exponential smoothing
	if (filteredAngle == 0) {
		filteredAngle = angleRoll;
	} else {
		filteredAngle = ALPHA * angleRoll + (1 - ALPHA) * filteredAngle;
	}
	
	int centerX = DISP.width() / 2;
	int centerY = DISP.height() / 2 - getStatusBarHeight();
	int lineLength = 60;
	
	float angleRad = (filteredAngle + 90) * PI / 180;
	int angle = IMU_LEVEL_ANGLE(accX, accY);
	
	int x1 = centerX - (lineLength / 2) * cos(angleRad);
	int y1 = centerY - (lineLength / 2) * sin(angleRad);
	int x2 = centerX + (lineLength / 2) * cos(angleRad);
	int y2 = centerY + (lineLength / 2) * sin(angleRad);

	canvas.fillSprite(BGCOLOR);

	// Center line
	canvas.drawLine(50, centerY, 190, centerY, TFT_DARKGREY);
	// Center point
	canvas.fillCircle(centerX, centerY, 4, FGCOLOR);
	// Horizont line
	canvas.drawWideLine(x1, y1, x2, y2, 2, FGCOLOR);
	
	// Degree number with throthling + snap to zero
	if (checkTimer(100)) {
		updateTimer();
		if (angle < 0.8) {
			lastAngle = 0;
		} else {
			lastAngle = int(angle);
		}
	}

	// turn on the led, play the sound and turn the number green at 0
	if (lastAngle == 0) {
		canvas.setTextColor(TFT_GREEN);
		DEVICE.Power.setLed(1);
		if (!levelToolSoundPlayed) {
			soundBeep();
			levelToolSoundPlayed = true;
		}
	} else {
		canvas.setTextColor(TFT_DARKGRAY);
		DEVICE.Power.setLed(0);
		levelToolSoundPlayed = false;
	}
	
	canvas.setTextSize(MEDIUM_TEXT);
	canvas.drawCenterString(String(lastAngle).c_str(), centerX, DISP.height() - getStatusBarHeight() - canvas.fontHeight() - 10);

	canvas.pushSprite(0, getStatusBarHeight());
	
	if (checkExit()) {
		DEVICE.Power.setLed(0);
		deviceSpeakerEnd();
	}
}