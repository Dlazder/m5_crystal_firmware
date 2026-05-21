// pid PID::BT_MOUSE

// Sensitivity settings
#define GYRO_SENSITIVITY 1.5f
#define DEADZONE 0.05f
#define SMOOTHING_FACTOR 0.3f

#define SPEED_LEVEL1_THRESHOLD 0.15f
#define SPEED_LEVEL2_THRESHOLD 0.35f
#define SPEED_LEVEL3_THRESHOLD 0.55f

#define SPEED_LEVEL1 1
#define SPEED_LEVEL2 5
#define SPEED_LEVEL3 10

auto getSpeed = [](float value) -> int {
	float absVal = abs(value);
	if (absVal < SPEED_LEVEL1_THRESHOLD) {
		return 0;
	} else if (absVal < SPEED_LEVEL2_THRESHOLD) {
		return SPEED_LEVEL1;
	} else if (absVal < SPEED_LEVEL3_THRESHOLD * 1.5f) {
		return SPEED_LEVEL2;
	} else {
		return SPEED_LEVEL3;
	}
};

void bluetoothMouseLoop() {
	if (!hasImu) {
		if (isSetup()) centeredPrint("no gyroscope", MEDIUM_TEXT);
		checkExit();
		return;
	}
	static float smoothedX = 0;
	static float smoothedY = 0;
	static bool isBleConnected = false;

	float accX, accY, accZ;
	if (isSetup()) {
		updateTimer();
		if (!bleCompositeBegan) {
			bleKeyboard.begin();
			bleCompositeBegan = true;
		}
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
	}

	if (bleKeyboard.isConnected()) {
		if (!isBleConnected) {
			centeredPrint(L->TXT_CONNECTED, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
			isBleConnected = true;
		}

		DEVICE.Imu.getAccelData(&accX, &accY, &accZ);

		float rawMoveX = IMU_MOUSE_X(accX, accY);
		float rawMoveY = IMU_MOUSE_Y(accX, accY);

		if (abs(rawMoveX) < DEADZONE) rawMoveX = 0;
		if (abs(rawMoveY) < DEADZONE) rawMoveY = 0;

		smoothedX = (smoothedX * (1.0f - SMOOTHING_FACTOR)) + (rawMoveX * SMOOTHING_FACTOR);
		smoothedY = (smoothedY * (1.0f - SMOOTHING_FACTOR)) + (rawMoveY * SMOOTHING_FACTOR);

		float absX = abs(smoothedX);
		float absY = abs(smoothedY);

		int speedX = getSpeed(smoothedX);
		int speedY = getSpeed(smoothedY);

		int deltaX = (smoothedX > 0) ? speedX : -speedX;
		int deltaY = (smoothedY > 0) ? speedY : -speedY;

		if (deltaX != 0 || deltaY != 0) {
			bleMouse.move(deltaX, deltaY);
		}

	} else {
		if (isBleConnected) {
			isBleConnected = false;
			centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT);
			DEVICE.Speaker.tone(2000, 200);
			smoothedX = 0;
			smoothedY = 0;
		}
	}

	if (isBtnPWRWasPressed() && checkTimer(100, true)) {
		bleMouse.click(MOUSE_RIGHT);
	}

	if (DEVICE.BtnA.isPressed()) {
		bleMouse.press(MOUSE_LEFT);
	} else {
		bleMouse.release(MOUSE_LEFT);
	}

	if (checkExit()) {
		isBleConnected = false;
		smoothedX = 0;
		smoothedY = 0;
		centeredPrint(L->TXT_DISCONNECTING, MEDIUM_TEXT);
	}
}
