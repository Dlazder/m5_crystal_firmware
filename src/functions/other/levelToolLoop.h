float filteredAngle = 0;
float filteredPitch = 0;
const float ALPHA = 0.3;

int lastAngle = 0;
bool levelToolSoundPlayed = false;
int levelToolMode = 0; // 0 = single axis (line), 1 = two axes (bubble)

void levelToolLoop() {
	if (!hasImu) {
		if (isSetup()) centeredPrint("no gyroscope", MEDIUM_TEXT);
		checkExit();
		return;
	}

	if (isSetup()) {
		deviceSpeakerBegin();
	}

	// Poll exit every tick for responsive button handling
	if (checkExit()) {
		DEVICE.Power.setLed(0);
		deviceSpeakerEnd();
		return;
	}

	if ((isBtnAWasPressed() || isKbEnterPressed())) {
		levelToolMode = (levelToolMode + 1) % 2;
	}

	// 25 fps
	if (!checkTimer(40)) {
		return;
	}

	float accX, accY, accZ;
	DEVICE.Imu.getAccelData(&accX, &accY, &accZ);

	int centerX = DISP.width() / 2;
	int centerY = canvas.height() / 2;

	// MODE 0: Single-axis line level (original)
	if (levelToolMode == 0) {
		int levelCenterY = centerY - 12; // raised to make room for hint
		float angleRoll = IMU_LEVEL_ROLL(accX, accY);

		// Exponential smoothing
		if (filteredAngle == 0) {
			filteredAngle = angleRoll;
		} else {
			filteredAngle = ALPHA * angleRoll + (1 - ALPHA) * filteredAngle;
		}

		int lineLength = 60;
		float angleRad = (filteredAngle + IMU_LEVEL_DISPLAY_OFFSET) * PI / 180;
		int angle = IMU_LEVEL_ANGLE(accX, accY);

		int x1 = centerX - (lineLength / 2) * cos(angleRad);
		int y1 = levelCenterY - (lineLength / 2) * sin(angleRad);
		int x2 = centerX + (lineLength / 2) * cos(angleRad);
		int y2 = levelCenterY + (lineLength / 2) * sin(angleRad);

		canvas.fillSprite(BGCOLOR);

		// Center line
		canvas.drawLine(50, levelCenterY, 190, levelCenterY, TFT_DARKGREY);
		// Center point
		canvas.fillCircle(centerX, levelCenterY, 4, FGCOLOR);
		// Horizont line
		canvas.drawWideLine(x1, y1, x2, y2, 2, FGCOLOR);

		// Hysteresis: enter "0" at < 0.8, leave at > 2.5
		if (lastAngle == 0) { if (angle > 2.5) lastAngle = int(angle); }
		else {
			if (angle < 0.8) lastAngle = 0;
			else lastAngle = int(angle);
		}

		// turn on the led, play the sound and change the number color at 0
		if (lastAngle == 0) {
			canvas.setTextColor(FGCOLOR);
			DEVICE.Power.setLed(1);
			if (!levelToolSoundPlayed) {
				soundBeep();
				levelToolSoundPlayed = true;
				dimmerWake();
			}
		} else {
			canvas.setTextColor(TFT_DARKGRAY);
			DEVICE.Power.setLed(0);
			levelToolSoundPlayed = false;
		}

		canvas.setTextSize(MEDIUM_TEXT);
		canvas.drawCenterString(String(lastAngle).c_str(), centerX, canvas.height() - canvas.fontHeight() - 22);

	// MODE 1: Two-axis bubble level (circle + dot)
	} else {
		// Calculate both axes: roll (left-right) and pitch (forward-back)
		// Device lies flat, screen up — use accZ as gravity reference
		float roll = IMU_LEVEL_BUBBLE_ROLL(accX, accY, accZ);
		float pitch = IMU_LEVEL_BUBBLE_PITCH(accX, accY, accZ);

		// Exponential smoothing for both axes
		if (filteredAngle == 0) {
			filteredAngle = roll;
			filteredPitch = pitch;
		} else {
			filteredAngle = ALPHA * roll + (1 - ALPHA) * filteredAngle;
			filteredPitch = ALPHA * pitch + (1 - ALPHA) * filteredPitch;
		}

		int circleRadius = min(canvas.width(), canvas.height()) / 3;
		int dotRadius = 5;
		int maxOffset = circleRadius - dotRadius - 3;

		float rollRad = filteredAngle * PI / 180;
		float pitchRad = filteredPitch * PI / 180;

		// Map tilt to dot displacement (clamped to circle)
		int dotX = centerX + sin(rollRad) * maxOffset;
		int dotY = centerY - sin(pitchRad) * maxOffset;

		// Clamp dot to stay within the circle
		int dx = dotX - centerX;
		int dy = dotY - centerY;
		float dist = sqrt(dx * dx + dy * dy);
		if (dist > maxOffset) {
			dotX = centerX + dx * maxOffset / dist;
			dotY = centerY + dy * maxOffset / dist;
		}

		// Combined tilt magnitude for level detection
		int combinedAngle = int(sqrt(filteredAngle * filteredAngle + filteredPitch * filteredPitch));

		// Hysteresis for level (both axes near zero)
		if (lastAngle == 0) { if (combinedAngle > 2.5) lastAngle = combinedAngle; }
		else {
			if (combinedAngle < 0.8) lastAngle = 0;
			else lastAngle = combinedAngle;
		}

		canvas.fillSprite(BGCOLOR);

		// Inner reference circle
		canvas.drawCircle(centerX, centerY, circleRadius / 3, TFT_DARKGREY);

		// Crosshair lines (trimmed to circle)
		canvas.drawLine(centerX - circleRadius, centerY, centerX + circleRadius, centerY, TFT_DARKGREY);
		canvas.drawLine(centerX, centerY - circleRadius, centerX, centerY + circleRadius, TFT_DARKGREY);

		// Tick marks at 0° and ±max on X axis
		int tickY1 = centerY - 4;
		int tickY2 = centerY + 4;
		canvas.drawLine(centerX - circleRadius, tickY1, centerX - circleRadius, tickY2, TFT_DARKGREY);
		canvas.drawLine(centerX + circleRadius, tickY1, centerX + circleRadius, tickY2, TFT_DARKGREY);
		canvas.drawLine(centerX, tickY1, centerX, tickY2, TFT_DARKGREY);
		// Y axis ticks
		int tickX1 = centerX - 4;
		int tickX2 = centerX + 4;
		canvas.drawLine(tickX1, centerY - circleRadius, tickX2, centerY - circleRadius, TFT_DARKGREY);
		canvas.drawLine(tickX1, centerY + circleRadius, tickX2, centerY + circleRadius, TFT_DARKGREY);

		// Outer circle (drawn last — clean over crosshair edges)
		canvas.drawCircle(centerX, centerY, circleRadius, FGCOLOR);

		// Moving dot
		if (lastAngle == 0) {
			canvas.fillCircle(dotX, dotY, dotRadius, FGCOLOR);
		} else {
			canvas.fillCircle(dotX, dotY, dotRadius, TFT_DARKGRAY);
		}

		// LED and sound at level
		if (lastAngle == 0) {
			canvas.setTextColor(FGCOLOR);
			DEVICE.Power.setLed(1);
			if (!levelToolSoundPlayed) {
				soundBeep();
				levelToolSoundPlayed = true;
				dimmerWake();
			}
		} else {
			canvas.setTextColor(TFT_DARKGRAY);
			DEVICE.Power.setLed(0);
			levelToolSoundPlayed = false;
		}

		// Show both angles on two right-aligned lines
		canvas.setTextSize(SMALL_TEXT);
		String rollText = "R:" + String(int(filteredAngle));
		String pitchText = "P:" + String(int(filteredPitch));
		int textY = canvas.height() - canvas.fontHeight() - 22;
		canvas.drawRightString(rollText.c_str(), canvas.width() - 10, textY - canvas.fontHeight() - 4);
		canvas.drawRightString(pitchText.c_str(), canvas.width() - 10, textY);
	}

	drawHintSwitch();
	canvas.pushSprite(0, getStatusBarHeight());
}