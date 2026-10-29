// PID::USB_MOUSE_JIGGLER
//
// Works on any board: native USB HID on ESP32-S3 boards, or an external CH9329
// USB-HID-over-UART bridge when enabled in Settings → USB (see usbHid.h).

#include "../../services/ch9329/usbHid.h"

#define JIGGLER_INTERVAL 5000
#define JIGGLER_STEP 5

void usbMouseJigglerLoop() {
	static bool usbReady = false;
	static bool dirRight = true;
	static int jiggleTimer = 0;

	if (isSetup()) {
		usbReady = false;
		dirRight = true;

		if (!usbHidActive()) {
			centeredPrint(L->TXT_USB_HID_UNSOPPORTED, MEDIUM_TEXT);
			return;
		}

		usbHidBegin();
		centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
		updateTimer();
	}

	if (!usbHidActive()) {
		checkExit();
		return;
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
		usbMouseMove(delta, 0);
		dirRight = !dirRight;
	}

	if (checkExit()) {
		usbReady = false;
		usbHidEnd();
	}
}
