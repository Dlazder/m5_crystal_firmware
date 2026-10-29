// CH9329 — USB-HID-over-UART bridge chip.
//
// The CH9329 is a standalone chip that implements a real USB HID keyboard (and
// optionally mouse) and is driven over a plain UART. The host only ever sees a
// USB HID device; the ESP32 never touches USB itself, so this works even on
// boards without native USB (e.g. the classic ESP32 in M5StickC Plus2), where
// badUsbLoop()/usbKeyboardLoop() are otherwise compiled out.
//
// This header owns only the wire protocol (packet framing + the standard 8-byte
// keyboard HID report). It does not own a Duckyscript::Sink — the two keycode
// spaces differ (CH9329 uses USB HID usage IDs + a modifier bitmask, Duckyscript
// speaks ESP32 BleCombo/USBHID "special key" codes 0x80+), so the sink that
// bridges them lives in the function that uses it (see ch9329KeyboardLoop.h).
//
// Packet format (little-endian lengths), per the CH9329 datasheet:
//   0x57 0xAB | ADDR(1) | CMD(1) | LEN(1) | DATA(LEN) | SUM(1)
// SUM = (sum of all preceding bytes) & 0xFF. ADDR is always 0x00 (no daisy-chain).
//
// CMD 0x02 = CMD_SEND_KB_GENERAL_DATA: an 8-byte standard HID keyboard report
//   [0]=modifier bitmask, [1]=reserved(0), [2..7]=up to 6 simultaneous USB HID
//   usage IDs (0 = no key in that slot).
//
// CMD 0x05 = CMD_SEND_MS_REL_DATA: a 5-byte relative mouse report
//   [0]=0x01 (constant "relative" marker), [1]=button bitmask, [2]=dX (int8),
//   [3]=dY (int8), [4]=wheel (int8).

#pragma once
#include <Arduino.h>

namespace CH9329 {

// USB HID modifier bitmask bits (byte 0 of the report).
enum Modifier : uint8_t {
	MOD_LCTRL = 0x01,
	MOD_LSHIFT = 0x02,
	MOD_LALT = 0x04,
	MOD_LGUI = 0x08,
	MOD_RCTRL = 0x10,
	MOD_RSHIFT = 0x20,
	MOD_RALT = 0x40,
	MOD_RGUI = 0x80,
};

// Attach the UART the chip is wired to. Does not call port->begin(); the caller
// owns the port's lifetime (baud/pins), same convention as the UART terminal.
void begin(HardwareSerial* port);
void end();

// True once begin() was called with a non-null port.
bool isReady();

// Send one keyboard HID report: up to 6 simultaneous USB HID usage IDs in
// `keys`, OR'd `modifiers` bitmask. keyCount may be 0..6; extra slots are
// padded with 0 (no key).
void sendKeyReport(uint8_t modifiers, const uint8_t* keys, uint8_t keyCount);

// Convenience: release every key (all-zero report).
void releaseAll();

// Mouse button bitmask bits (byte 1 of the relative-mouse report).
enum MouseButton : uint8_t {
	MOUSE_LEFT_BTN = 0x01,
	MOUSE_RIGHT_BTN = 0x02,
	MOUSE_MIDDLE_BTN = 0x04,
};

// Send one relative mouse report. dx/dy/wheel are signed deltas (-127..127);
// `buttons` is an OR of MouseButton bits currently held down.
void sendMouseReport(uint8_t buttons, int8_t dx, int8_t dy, int8_t wheel = 0);

} // namespace CH9329
