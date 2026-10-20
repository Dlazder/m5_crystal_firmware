// Maps the ESP32 BleCombo/USBHID "special key" codes (0x80+) and plain ASCII
// that Duckyscript emits (see duckyscript.h) to the standard USB HID keyboard
// usage IDs that CH9329 expects in its report, plus the handful of modifier
// keys that must set a bitmask bit instead of filling a key slot.
//
// Header-only: a small lookup table, no state.

#pragma once
#include <Arduino.h>
#include "ch9329.h"

namespace CH9329KeycodeMap {

// Returns true and fills `modBit` if `duckyCode` is a modifier (CTRL/SHIFT/ALT/GUI)
// that CH9329 reports via the modifier bitmask rather than a key slot.
inline bool isModifier(uint8_t duckyCode, uint8_t& modBit) {
	switch (duckyCode) {
		case 0x80: modBit = CH9329::MOD_LCTRL; return true;
		case 0x81: modBit = CH9329::MOD_LSHIFT; return true;
		case 0x82: modBit = CH9329::MOD_LALT; return true;
		case 0x83: modBit = CH9329::MOD_LGUI; return true;
		default: return false;
	}
}

// USB HID usage ID for every other Duckyscript code (specials + ASCII).
// 0 = unsupported (dropped silently, same as resolveKey() returning 0).
inline uint8_t toHidUsage(uint8_t duckyCode) {
	switch (duckyCode) {
		// ESP32 "special key" codes (see duckyscript.cpp's resolveKey table).
		case 0xB0: return 0x28; // ENTER
		case 0xB2: return 0x2A; // BACKSPACE
		case 0xB3: return 0x2B; // TAB
		case 0xB1: return 0x29; // ESC
		case 0xD4: return 0x4C; // DELETE
		case 0xD1: return 0x49; // INSERT
		case 0xD2: return 0x4A; // HOME
		case 0xD5: return 0x4D; // END
		case 0xD3: return 0x4B; // PAGEUP
		case 0xD6: return 0x4E; // PAGEDOWN
		case 0xDA: return 0x52; // UP
		case 0xD9: return 0x51; // DOWN
		case 0xD8: return 0x50; // LEFT
		case 0xD7: return 0x4F; // RIGHT
		case 0xC2: return 0x3A; // F1
		case 0xC3: return 0x3B; // F2
		case 0xC4: return 0x3C; // F3
		case 0xC5: return 0x3D; // F4
		case 0xC6: return 0x3E; // F5
		case 0xC7: return 0x3F; // F6
		case 0xC8: return 0x40; // F7
		case 0xC9: return 0x41; // F8
		case 0xCA: return 0x42; // F9
		case 0xCB: return 0x43; // F10
		case 0xCC: return 0x44; // F11
		case 0xCD: return 0x45; // F12
	}

	// Plain ASCII (letters/digits/punctuation), as produced by STRING and SPACE.
	// NOTE: shifted characters (uppercase, symbols) need MOD_LSHIFT set alongside
	// the base-key usage ID; the sink applies that (see ch9329KeyboardLoop.h).
	if (duckyCode >= 'a' && duckyCode <= 'z') return 0x04 + (duckyCode - 'a');
	if (duckyCode >= 'A' && duckyCode <= 'Z') return 0x04 + (duckyCode - 'A');
	if (duckyCode >= '1' && duckyCode <= '9') return 0x1E + (duckyCode - '1');
	switch (duckyCode) {
		case '0': return 0x27;
		case ' ': return 0x2C;
		case '-': case '_': return 0x2D;
		case '=': case '+': return 0x2E;
		case '[': case '{': return 0x2F;
		case ']': case '}': return 0x30;
		case '\\': case '|': return 0x31;
		case ';': case ':': return 0x33;
		case '\'': case '"': return 0x34;
		case '`': case '~': return 0x35;
		case ',': case '<': return 0x36;
		case '.': case '>': return 0x37;
		case '/': case '?': return 0x38;
	}
	return 0;
}

// True if `asciiChar` requires MOD_LSHIFT held to type it (uppercase letters and
// the shifted symbol row), as a plain US QWERTY layout maps them.
inline bool needsShift(char asciiChar) {
	if (asciiChar >= 'A' && asciiChar <= 'Z') return true;
	switch (asciiChar) {
		case '!': case '@': case '#': case '$': case '%':
		case '^': case '&': case '*': case '(': case ')':
		case '_': case '+': case '{': case '}': case '|':
		case ':': case '"': case '~': case '<': case '>': case '?':
			return true;
		default: return false;
	}
}

// Symbol characters that need shift map to a different base key than the literal
// char (e.g. '!' -> '1' + shift). Returns the unshifted char toHidUsage() should
// resolve instead, or 0 if `asciiChar` already is its own base key.
inline char shiftedSymbolBase(char asciiChar) {
	switch (asciiChar) {
		case '!': return '1';
		case '@': return '2';
		case '#': return '3';
		case '$': return '4';
		case '%': return '5';
		case '^': return '6';
		case '&': return '7';
		case '*': return '8';
		case '(': return '9';
		case ')': return '0';
		case '_': return '-';
		case '+': return '=';
		case '{': return '[';
		case '}': return ']';
		case '|': return '\\';
		case ':': return ';';
		case '"': return '\'';
		case '~': return '`';
		case '<': return ',';
		case '>': return '.';
		case '?': return '/';
		default: return 0;
	}
}

} // namespace CH9329KeycodeMap
