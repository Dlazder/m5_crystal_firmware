#pragma once
// Settings service: declarative registry of persisted settings.
//
// Each setting is described once in registry.h (NVS key, runtime variable, default,
// allowed values). The service owns loading, validated writes, reset and the
// pin-conflict check; menus only render and edit what is registered here
// (see MENU::setting in globals.h and the editing mode in menuUtils.h).
//
// Header-only: relies on getData()/setData() from storageUtils.h, so it must be
// included after it.

#include <Arduino.h>
#include <limits.h>
#include <string.h>

enum class SettingType : uint8_t {
	TOGGLE, // 0 / 1, flipped by Enter and drawn as a checkbox
	PIN,    // one of `values` (devicePins), rendered as "G<n>"
	LIST,   // one of `values` (e.g. baud rates), rendered as the number or via fmt
	RANGE,  // min..max, wraps around at the ends
};

// `def` for settings whose default is the variable's initial value (conf.h / globals.h):
// load() keeps that value when nothing is stored. Such settings cannot be reset().
#define SETTING_DEF_INITIAL INT_MIN

// Settings that must not collide with each other (checked for PIN settings only).
enum class SettingGroup : uint8_t { NONE, RF_PINS, PN532_PINS, RC522_PINS, IR_PINS, UART_PINS };

struct Setting {
	const char*    key;   // NVS key; also MENU::settingKey of the menu item bound to it
	SettingType    type;
	SettingGroup   group;
	void*          var;     // runtime source of truth
	uint8_t        varSize; // sizeof(*var): 1, 2 or 4
	int            def;
	int            min, max;       // RANGE (step is 1)
	const int*     values;         // PIN / LIST: the allowed values
	uint8_t        count;
	String (*fmt)(int);            // RANGE: text shown instead of the number (font name, "30s"...); nullptr = number
	void (*onChange)(int);         // optional side effect after a validated set()/preview()
	bool           live;           // apply every step while editing (brightness, color); ESC restores the old value
};

// True while load() runs: onChange functions skip effects that make no sense at boot
// (confirmation beep, rebuilding a canvas that setup() has not created yet).
static bool settingsLoading = false;

#include "registry.h"

namespace Settings {

static int _read(const Setting* s) {
	switch (s->varSize) {
		case 1:  return *(uint8_t*)s->var;
		case 2:  return *(uint16_t*)s->var;
		default: return *(int*)s->var;
	}
}

// Writes the variable and NVS without validation (load/reset: defaults may be
// outside `values`, e.g. a board's stock CC1101 pin that is not broken out).
static void _apply(const Setting* s, int v) {
	switch (s->varSize) {
		case 1:  *(uint8_t*)s->var = v;  break;
		case 2:  *(uint16_t*)s->var = v; break;
		default: *(int*)s->var = v;      break;
	}
}

const Setting* find(const char* key) {
	for (int i = 0; i < settingsCount; i++) {
		if (strcmp(settingsRegistry[i].key, key) == 0) return &settingsRegistry[i];
	}
	return nullptr;
}

int get(const Setting* s) {
	return _read(s);
}

// PIN and LIST both pick one of `values`.
static bool _isList(const Setting* s) {
	return s->type == SettingType::PIN || s->type == SettingType::LIST;
}

// Index of `v` in a list setting's values; -1 when absent (e.g. a stored value that is
// no longer in the board's pin list).
static int _indexOf(const Setting* s, int v) {
	for (int i = 0; i < s->count; i++) {
		if (s->values[i] == v) return i;
	}
	return -1;
}

// Next/previous allowed value (dir = +1 / -1), wrapping at the ends. A list value
// outside `values` steps to the first (forward) or last (backward) listed value.
int step(const Setting* s, int v, int dir) {
	if (s->type == SettingType::TOGGLE) return v ? 0 : 1;
	if (_isList(s)) {
		int i = _indexOf(s, v);
		i = (i < 0) ? (dir > 0 ? 0 : s->count - 1) : (i + dir + s->count) % s->count;
		return s->values[i];
	}
	v += dir;
	if (v > s->max) v = s->min;
	if (v < s->min) v = s->max;
	return v;
}

bool isValid(const Setting* s, int v) {
	if (s->type == SettingType::TOGGLE) return v == 0 || v == 1;
	if (_isList(s)) {
		for (int i = 0; i < s->count; i++) {
			if (s->values[i] == v) return true;
		}
		return false;
	}
	return v >= s->min && v <= s->max;
}

// Persists the variable's current value (confirming a live edit without firing onChange again).
void commit(const Setting* s) {
	setData(s->key, _read(s));
}

// Validated write: updates the variable and fires onChange. persist = false is the
// live preview while editing (no NVS write).
bool set(const Setting* s, int v, bool persist = true) {
	if (!isValid(s, v)) return false;
	_apply(s, v);
	if (s->onChange) s->onChange(v);
	if (persist) commit(s);
	return true;
}

void reset(const Setting* s) {
	if (s->def == SETTING_DEF_INITIAL) return;
	_apply(s, s->def);
	setData(s->key, s->def);
	if (s->onChange) s->onChange(s->def);
}

void resetGroup(SettingGroup g) {
	for (int i = 0; i < settingsCount; i++) {
		if (settingsRegistry[i].group == g) reset(&settingsRegistry[i]);
	}
}

// Loads every registered setting from NVS into its variable and applies it to the
// hardware through onChange. Call after preferences.begin().
void load() {
	// Volume used to be stored as a hardware value (level * volumeDividor). Levels are
	// 1..volumeStep, so a larger stored value is the old format: convert it once.
	int storedVolume = getData("volume", 0);
	if (storedVolume > volumeStep) setData("volume", storedVolume / volumeDividor);

	settingsLoading = true;
	for (int i = 0; i < settingsCount; i++) {
		const Setting* s = &settingsRegistry[i];
		int def = (s->def == SETTING_DEF_INITIAL) ? _read(s) : s->def;
		int v = getData(s->key, def);
		// Only RANGE is checked: a stored index outside it would read past the end of the
		// arrays it selects from. A stored pin/list value may predate the list.
		v = (s->type == SettingType::RANGE && !isValid(s, v)) ? def : v;
		_apply(s, v);
		if (s->onChange) s->onChange(v);
		Serial.printf("Setting %s = %d", s->key, v);
	}
	settingsLoading = false;
}

// True when another PIN setting of the same group currently uses `v`.
bool hasConflict(const Setting* s, int v) {
	if (s->type != SettingType::PIN || s->group == SettingGroup::NONE) return false;
	for (int i = 0; i < settingsCount; i++) {
		const Setting* o = &settingsRegistry[i];
		if (o != s && o->group == s->group && o->type == SettingType::PIN && _read(o) == v) return true;
	}
	return false;
}

String format(const Setting* s, int v) {
	if (s->type == SettingType::PIN) return "G" + String(v);
	if (s->type == SettingType::TOGGLE) return v ? "on" : "off";
	if (s->fmt) return s->fmt(v);
	return String(v);
}

} // namespace Settings
