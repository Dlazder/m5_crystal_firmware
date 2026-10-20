// usbHid — shared by badUsbLoop/usbKeyboardLoop/usbMouseLoop/usbMouseJigglerLoop
// so each of those keeps a single code path regardless of transport: native
// USBHID (ESP32-S3 boards only) or an external CH9329
// USB-HID-over-UART bridge (any board, including classic ESP32 ones with no
// native USB device controller). Which one is used is one runtime switch,
// `ch9329Enabled` (Settings → USB), not a build-time #ifdef — the point of
// CH9329 is to make USB HID available on boards where native USB cannot do it,
// without forking the already-small per-function loop files in two.
//
// usbHidActive() tells each loop file whether a transport exists at all for the
// current board + setting, so the "unsupported" fallback stays in one place
// instead of being re-derived per function.
//
// Mouse buttons use the Arduino BLE/USB convention (MOUSE_LEFT/MOUSE_RIGHT,
// from BleCombo's headers, already included by the time this is used) and are
// translated to CH9329's own bitmask internally.

#pragma once
#include <Arduino.h>
#include "ch9329.h"
#include "ch9329Sink.h"

static HardwareSerial Ch9329Serial(1); // UART1 — free on every supported board
static bool ch9329PortOpen = false;

static void ch9329EnsurePortOpen() {
	if (ch9329PortOpen) return;
	Ch9329Serial.begin(ch9329Baud, SERIAL_8N1, ch9329RxPin, ch9329TxPin);
	CH9329::begin(&Ch9329Serial);
	ch9329PortOpen = true;
}

static void ch9329ClosePort() {
	if (!ch9329PortOpen) return;
	CH9329::end();
	Ch9329Serial.end();
	ch9329PortOpen = false;
}

// True if some USB HID transport can actually be driven right now: either this
// board has native USB (ESP32-S3) or the CH9329 bridge is enabled in Settings.
static bool usbHidActive() {
#ifdef ESP32S3
	return true;
#else
	return ch9329Enabled;
#endif
}

// Call once per "enter the loop" (isSetup()), before using any Usb* function
// below. Brings up whichever transport usbHidActive() selected.
static void usbHidBegin() {
	if (ch9329Enabled) {
		ch9329EnsurePortOpen();
	}
#ifdef ESP32S3
	else if (!usbHidBegan) {
		usbKeyboard.begin();
		usbMouse.begin();
		USB.begin();
		usbHidBegan = true;
	}
#endif
}

// Call on checkExit() and end-of-script paths to release the UART cleanly.
// Native USBHID has no equivalent teardown (USB.begin() is a one-time init for
// the whole firmware session, same as today).
static void usbHidEnd() {
	if (ch9329Enabled) ch9329ClosePort();
}

// Call once per loop() tick while usbHidActive() — drives Ch9329Sink's deferred
// key release (see ch9329Sink.h). No-op on native USBHID (it has no such delay).
static void usbHidPoll() {
	if (ch9329Enabled) Ch9329Sink::poll();
}

// --- Keyboard ---

static void usbKbWrite(uint8_t keyOrChar) {
	if (ch9329Enabled) { Ch9329Sink::doWrite(keyOrChar); return; }
#ifdef ESP32S3
	usbKeyboard.write(keyOrChar);
#endif
}

static void usbKbPress(uint8_t keyOrChar) {
	if (ch9329Enabled) { Ch9329Sink::doPress(keyOrChar); return; }
#ifdef ESP32S3
	usbKeyboard.press(keyOrChar);
#endif
}

static void usbKbReleaseAll() {
	if (ch9329Enabled) { Ch9329Sink::doReleaseAll(); return; }
#ifdef ESP32S3
	usbKeyboard.releaseAll();
#endif
}

// --- Mouse ---
// dx/dy/wheel match CH9329's signed-byte range; callers already clamp to it
// (USB_MOUSE_SPEED_L3 = 10, JIGGLER_STEP = 5).

static uint8_t ch9329HeldButtons = 0;

static void usbMouseMove(int8_t dx, int8_t dy) {
	if (ch9329Enabled) { CH9329::sendMouseReport(ch9329HeldButtons, dx, dy); return; }
#ifdef ESP32S3
	usbMouse.move(dx, dy);
#endif
}

static void usbMousePress(uint8_t button) {
	if (ch9329Enabled) {
		ch9329HeldButtons |= (button == MOUSE_RIGHT) ? CH9329::MOUSE_RIGHT_BTN
												: (button == MOUSE_MIDDLE) ? CH9329::MOUSE_MIDDLE_BTN
												: CH9329::MOUSE_LEFT_BTN;
		CH9329::sendMouseReport(ch9329HeldButtons, 0, 0);
		return;
	}
#ifdef ESP32S3
	usbMouse.press(button);
#endif
}

static void usbMouseRelease(uint8_t button) {
	if (ch9329Enabled) {
		ch9329HeldButtons &= ~((button == MOUSE_RIGHT) ? CH9329::MOUSE_RIGHT_BTN
												: (button == MOUSE_MIDDLE) ? CH9329::MOUSE_MIDDLE_BTN
												: CH9329::MOUSE_LEFT_BTN);
		CH9329::sendMouseReport(ch9329HeldButtons, 0, 0);
		return;
	}
#ifdef ESP32S3
	usbMouse.release(button);
#endif
}

static void usbMouseClick(uint8_t button) {
	usbMousePress(button);
	usbMouseRelease(button);
}
