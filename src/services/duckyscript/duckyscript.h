// Duckyscript — DuckyScript (BadUSB) interpreter for keystroke-injection payloads.
//
// Mirrors Flipper Zero's BadUsb worker: one script engine, many transports. The
// engine parses DELAY / STRING / KEY / COMBO (plus REM and `//` comments) and
// emits HID events through a transport-agnostic Sink; the firmware supplies a
// BLE sink (wrapping bleKeyboard) and a native-USB sink (wrapping usbKeyboard).
//
// The service is deliberately UI- and transport-free: no globals.h, no canvas,
// no L, no BleCombo/USB. It owns the loaded script lines and the current
// execution position, and only knows the HID codes it emits.
//
// Contract:
// * begin() TAKES OWNERSHIP of `lines` (the array returned by
//	 Storage::readLines) and frees it in end(). Do not delete[] it yourself.
// * step() is NON-BLOCKING: it advances the interpreter by at most one atomic
//	 unit per call (one character, one key, one hold-release, one delay). The
//	 caller's loop() drives it once per tick, so the UI stays responsive while
//	 a long STRING or DELAY is in flight.
//
// Keycodes are the ESP32 HID "special key" convention (0x80+, e.g. 0xB0 = Enter),
// identical across BleCombo and the native USBHID, so one code works for both
// transports. Plain ASCII (e.g. ' ') passes through unchanged. resolveKey()
// returns these raw codes directly; they are deliberately NOT named constants
// because several bare key-name macros (SHIFT, SPACE, ...) leak from the device
// and HID headers and would be expanded by the preprocessor.

#pragma once
#include <Arduino.h>

namespace Duckyscript {

// Result of one step() call. Distinguishes "a visible line changed" (redraw the
// script highlight) from "still busy, nothing to draw" so the caller does not
// pushSprite() the same frame hundreds of times per second.
enum class Progress {
	Finished,     // script is done
	LineAdvanced, // currentLine changed — redraw the highlight
	Busy,         // still mid-command (typing / hold / delay) — nothing to draw
};

// HID output sink — the transport-specific half of the interpreter. The firmware
// provides one implementation per transport (BLE, native USB) wrapping the
// corresponding keyboard object.
struct Sink {
	void (*write)(uint8_t code);
	void (*press)(uint8_t code);
	void (*releaseAll)();
};

// Load a script. Takes ownership of `lines`; frees it in end().
void begin(const String* lines, int count, const Sink& sink);

// Restart from the first line without re-loading (for "run again").
void rewind();

// Stop and free the loaded script.
void end();

int lineCount();
const String* lines();
int currentLine();

// Advance the interpreter one atomic unit. Returns false only via Finished.
Progress step();

// Resolve a DuckyScript key name to a HID code (0 = unknown).
uint8_t resolveKey(const String& name);

} // namespace Duckyscript
