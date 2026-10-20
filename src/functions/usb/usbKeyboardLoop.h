// PID::USB_KEYBOARD
//
// Works on any board: native USB HID on ESP32-S3 boards, or an external CH9329
// USB-HID-over-UART bridge when enabled in Settings → USB (see usbHid.h).

#include "../../services/ch9329/usbHid.h"

void usbKeyboardLoop() {
	static bool usbReady = false;

	if (isSetup()) {
		usbReady = false;
		kbReset();

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
	usbHidPoll();

	if (!usbReady) {
		if (checkTimer(1000, true)) {
			usbReady = true;
			soundSuccess();
			drawKeyboardUi();
		}
		if (checkExit()) kbEnd();
		return;
	}

	if (isKbCursorUpPressed()) usbKbWrite(KEY_UP_ARROW);
	if (isKbCursorDownPressed()) usbKbWrite(KEY_DOWN_ARROW);
	if (isKbCursorLeftPressed()) usbKbWrite(KEY_LEFT_ARROW);
	if (isKbCursorRightPressed()) usbKbWrite(KEY_RIGHT_ARROW);

	keyboardLoop(
		[]() {
			kbEnd();
			usbReady = false;
			usbHidEnd();
			DISP.clear();
			changeProcess(PID::USB);
		},
		[](const char* buf) {
			usbKbWrite(KEY_RETURN);
		},
		[](char ch) {
			// correct HID value
			if (ch == '\b') usbKbWrite(BLE_KEY_BACKSPACE);
			else usbKbWrite((uint8_t)ch);
		}
	);
}
