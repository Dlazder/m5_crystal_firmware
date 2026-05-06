#pragma once

struct Locale {
	#define X(key) const char* key;
	#include "locale_keys.def"
	#undef X
};

extern const Locale LANG_EN;
extern const Locale LANG_ES;

inline const Locale* L = &LANG_EN;

inline void setLocale(const Locale* lang) { L = lang; }
