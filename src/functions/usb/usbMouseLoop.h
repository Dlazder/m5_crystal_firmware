// PID::USB_MOUSE

#ifdef ESP32S3

// Sensitivity settings (shared with the BLE air mouse)
#define USB_MOUSE_DEADZONE 0.05f
#define USB_MOUSE_SMOOTHING_FACTOR 0.3f

#define USB_MOUSE_SPEED_L1_THRESHOLD 0.15f
#define USB_MOUSE_SPEED_L2_THRESHOLD 0.35f
#define USB_MOUSE_SPEED_L3_THRESHOLD 0.55f

#define USB_MOUSE_SPEED_L1 1
#define USB_MOUSE_SPEED_L2 5
#define USB_MOUSE_SPEED_L3 10

auto usbMouseGetSpeed = [](float value) -> int {
	float absVal = abs(value);
	if (absVal < USB_MOUSE_SPEED_L1_THRESHOLD) {
		return 0;
	} else if (absVal < USB_MOUSE_SPEED_L2_THRESHOLD) {
		return USB_MOUSE_SPEED_L1;
	} else if (absVal < USB_MOUSE_SPEED_L3_THRESHOLD * 1.5f) {
		return USB_MOUSE_SPEED_L2;
	} else {
		return USB_MOUSE_SPEED_L3;
	}
};

void usbMouseLoop() {
	if (!hasImu) {
		if (isSetup()) centeredPrint("no gyroscope", MEDIUM_TEXT);
		checkExit();
		return;
	}

	static bool usbReady = false;
	static float smoothedX = 0;
	static float smoothedY = 0;

	if (isSetup()) {
		usbReady = false;
		smoothedX = 0;
		smoothedY = 0;
		if (!usbHidBegan) {
			usbMouse.begin();
			USB.begin();
			usbHidBegan = true;
		}
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
	}

	if (!usbReady) {
		if (checkTimer(1000, true)) {
			usbReady = true;
			soundSuccess();
			centeredPrint(L->TXT_CONNECTED, MEDIUM_TEXT);
		}
		checkExit();
		return;
	}

	float accX, accY, accZ;
	DEVICE.Imu.getAccelData(&accX, &accY, &accZ);

	float rawMoveX = IMU_MOUSE_X(accX, accY);
	float rawMoveY = IMU_MOUSE_Y(accX, accY);

	if (abs(rawMoveX) < USB_MOUSE_DEADZONE) rawMoveX = 0;
	if (abs(rawMoveY) < USB_MOUSE_DEADZONE) rawMoveY = 0;

	smoothedX = (smoothedX * (1.0f - USB_MOUSE_SMOOTHING_FACTOR)) + (rawMoveX * USB_MOUSE_SMOOTHING_FACTOR);
	smoothedY = (smoothedY * (1.0f - USB_MOUSE_SMOOTHING_FACTOR)) + (rawMoveY * USB_MOUSE_SMOOTHING_FACTOR);

	int speedX = usbMouseGetSpeed(smoothedX);
	int speedY = usbMouseGetSpeed(smoothedY);

	int deltaX = (smoothedX > 0) ? speedX : -speedX;
	int deltaY = (smoothedY > 0) ? speedY : -speedY;

	if (deltaX != 0 || deltaY != 0) {
		usbMouse.move(deltaX, deltaY);
	}

	if (isBtnPWRWasPressed() && checkTimer(100, true)) {
		usbMouse.click(MOUSE_RIGHT);
	}

	if (DEVICE.BtnA.isPressed()) {
		usbMouse.press(MOUSE_LEFT);
	} else {
		usbMouse.release(MOUSE_LEFT);
	}

	if (checkExit()) {
		usbReady = false;
		smoothedX = 0;
		smoothedY = 0;
	}
}

#else // !ESP32S3 — USB HID unsupported on this board

void usbMouseLoop() {
	if (isSetup()) {
		centeredPrint(L->TXT_USB_HID_UNSOPPORTED, MEDIUM_TEXT);
	}
	checkExit();
}

#endif // ESP32S3
