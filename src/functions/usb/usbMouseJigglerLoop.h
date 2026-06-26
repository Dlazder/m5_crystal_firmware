// PID::USB_MOUSE_JIGGLER

#ifdef ESP32S3

#define JIGGLER_INTERVAL 5000
#define JIGGLER_STEP 5

void usbMouseJigglerLoop() {
	static bool usbReady = false;
	static bool dirRight = true;
	static int jiggleTimer = 0;

	if (isSetup()) {
		usbReady = false;
		dirRight = true;
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
			jiggleTimer = globalTimer;
			soundSuccess();
		}
		checkExit();
		return;
	}

	centeredPrint(L->TXT_WORKING, MEDIUM_TEXT, true);
	drawSpinner();
	canvas.pushSprite(0, getStatusBarHeight());

	// Nudge the cursor left/right on a private timer so it stays in place.
	if (checkTimer(JIGGLER_INTERVAL, true, &jiggleTimer)) {
		int delta = dirRight ? JIGGLER_STEP : -JIGGLER_STEP;
		usbMouse.move(delta, 0);
		dirRight = !dirRight;
	}

	if (checkExit()) {
		usbReady = false;
	}
}

#else // !ESP32S3 — USB HID unsupported on this board

void usbMouseJigglerLoop() {
	if (isSetup()) {
		centeredPrint(L->TXT_USB_HID_UNSOPPORTED, MEDIUM_TEXT);
	}
	checkExit();
}

#endif // ESP32S3
