// BLE service — the composite keyboard/mouse HID transport (used by the bad BLE,
// keyboard, mouse, mouse jiggler, presenter and shutter screens) plus the
// currently selected/discovered device's info (set by the scanner, read by the
// info/finder screens).
//
// Mirrors the Wifi service: one connect/handle-connection API instead of each
// screen touching bleKeyboard/bleConnected directly, and one SelectedDevice
// struct instead of loose globals.

#pragma once
#include <Arduino.h>
#include <BleCombo.h>

namespace Ble {

// Composite HID transport

extern BleComboKeyboard keyboard;
extern BleComboMouse mouse;

// True once the composite HID has been initialized (begin() called at least
// once). Checked before keyboard.isConnected() — that call is unsafe pre-init.
extern bool compositeBegan;

// True while a central is connected (updated by handleConnection()).
extern bool connected;

// Starts the composite HID the first time it's called (idempotent) and shows
// the "waiting for connection" screen. Frees the SD card first — Bluedroid
// needs a large contiguous block of internal RAM, and on PSRAM-less boards
// that's the difference between init succeeding and heap exhaustion.
void begin();

// Weak UI hook fired by begin() after starting the HID transport; the no-op
// default lives in ble.cpp so this translation unit stays UI-free. The
// firmware's UI layer provides a STRONG override (src/utils/bleUtils.h) that
// shows the "waiting for connection" screen. Do not call this directly —
// use begin().
void reportBeginWaiting();

// Call every frame from a screen that uses the BLE HID transport. Fires
// onConnect/onDisconnect exactly once on each transition and updates `connected`.
void handleConnection(void (*onConnect)(), void (*onDisconnect)());

// Currently selected / discovered device

// Set by selectedBluetoothMenuLoop from a scan result; read by the info and
// finder screens.
struct SelectedDevice {
	String name;
	String address;
	int rssi = 0;
	String manufacturer;
	String manufacturerRaw;
	int8_t txPower = -128;
	uint8_t advFlags = 0xFF;
	String serviceUUIDs;
	String appearance;
};
extern SelectedDevice selected;

// Looks up a BLE SIG company identifier (the first two bytes of AD type
// 0xFF manufacturer data) against a short list of common vendors.
// Returns e.g. "0x004C" (hex, unknown) rather than an empty string, so
// callers can always display the result directly.
String manufacturerName(uint16_t companyId);

} // namespace Ble
