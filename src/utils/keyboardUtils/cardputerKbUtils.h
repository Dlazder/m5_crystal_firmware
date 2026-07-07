#if HAS_PHYSICAL_KB
	#include <M5Cardputer.h>

	bool kbUpPressed = false;
	bool kbDownPressed = false;
	bool kbLeftPressed = false;
	bool kbRightPressed = false;
	bool kbPlusPressed = false;
	bool kbMinusPressed = false;
	bool kbEnterPressed = false;
	bool kbEscPressed = false;
	bool kbDelPressed = false;
	bool kbCursorUpPressed = false;
	bool kbCursorDownPressed = false;
	bool kbCursorLeftPressed = false;
	bool kbCursorRightPressed = false;
	bool kbTextMode = false;
	String kbWord = "";

	void cardputerKbUpdate() {
		M5Cardputer.update();
		if (!M5Cardputer.Keyboard.isChange()) return;
		if (!M5Cardputer.Keyboard.isPressed()) return;

		Keyboard_Class::KeysState state = M5Cardputer.Keyboard.keysState();

		if (state.del) { kbDelPressed = true; return; }
		if (state.enter) { kbEnterPressed = true; return; }

		kbWord = "";
		for (char c : state.word) {
			if (kbTextMode) {
				if (c == '`') kbEscPressed = true;
				else if (c == ';' && !state.fn) kbCursorUpPressed = true;
				else if (c == '.' && !state.fn) kbCursorDownPressed = true;
				else if (c == ',' && !state.fn) kbCursorLeftPressed = true;
				else if (c == '/' && !state.fn) kbCursorRightPressed = true;
				else kbWord += c;
			} else {
				if (c == ';') kbUpPressed = true;
				else if (c == '.') kbDownPressed = true;
				else if (c == '`') kbEscPressed = true;
				else if (c == ',') kbLeftPressed = true;
				else if (c == '/') kbRightPressed = true;
				else if (c == '=' || c == '+') kbPlusPressed = true;
				else if (c == '-') kbMinusPressed = true;
				else kbWord += c;
			}
		}
	}
#endif

void clearKbFlags() {
	#if HAS_PHYSICAL_KB
	kbUpPressed = false;
	kbDownPressed = false;
	kbLeftPressed = false;
	kbRightPressed = false;
	kbPlusPressed = false;
	kbMinusPressed = false;
	kbEnterPressed = false;
	kbEscPressed = false;
	kbDelPressed = false;
	kbCursorUpPressed = false;
	kbCursorDownPressed = false;
	kbCursorLeftPressed = false;
	kbCursorRightPressed = false;
	kbWord = "";
	#endif
}

bool isAnyKbKeyPressed() {
	#if HAS_PHYSICAL_KB
	return kbUpPressed || kbDownPressed || kbLeftPressed || kbRightPressed ||
		kbPlusPressed || kbMinusPressed || kbEnterPressed || kbEscPressed ||
	  kbDelPressed || kbCursorLeftPressed || kbCursorRightPressed || kbWord.length() > 0;
	#endif
	return false;
}

// Menu navigation — active when kbTextMode = false (;  .  ,  /)
bool isKbUpPressed() {
	#if HAS_PHYSICAL_KB
	if (kbUpPressed) { kbUpPressed = false; return true; }
	#endif
	return false;
}

bool isKbDownPressed() {
	#if HAS_PHYSICAL_KB
	if (kbDownPressed) { kbDownPressed = false; return true; }
	#endif
	return false;
}

bool isKbEnterPressed() {
	#if HAS_PHYSICAL_KB
	if (kbEnterPressed) { kbEnterPressed = false; return true; }
	#endif
	return false;
}

bool isKbEscPressed() {
	#if HAS_PHYSICAL_KB
	if (kbEscPressed) { kbEscPressed = false; return true; }
	#endif
	return false;
}

bool isKbLeftPressed() {
	#if HAS_PHYSICAL_KB
	if (kbLeftPressed) { kbLeftPressed = false; return true; }
	#endif
	return false;
}

bool isKbRightPressed() {
	#if HAS_PHYSICAL_KB
	if (kbRightPressed) { kbRightPressed = false; return true; }
	#endif
	return false;
}

bool isKbPlusPressed() {
	#if HAS_PHYSICAL_KB
	if (kbPlusPressed) { kbPlusPressed = false; return true; }
	#endif
	return false;
}

bool isKbMinusPressed() {
	#if HAS_PHYSICAL_KB
	if (kbMinusPressed) { kbMinusPressed = false; return true; }
	#endif
	return false;
}

// Text cursor movement — active when kbTextMode = true (same keys, different context)
bool isKbCursorUpPressed() {
	#if HAS_PHYSICAL_KB
	if (kbCursorUpPressed) { kbCursorUpPressed = false; return true; }
	#endif
	return false;
}

bool isKbCursorDownPressed() {
	#if HAS_PHYSICAL_KB
	if (kbCursorDownPressed) { kbCursorDownPressed = false; return true; }
	#endif
	return false;
}

bool isKbCursorLeftPressed() {
	#if HAS_PHYSICAL_KB
	if (kbCursorLeftPressed) { kbCursorLeftPressed = false; return true; }
	#endif
	return false;
}

bool isKbCursorRightPressed() {
	#if HAS_PHYSICAL_KB
	if (kbCursorRightPressed) { kbCursorRightPressed = false; return true; }
	#endif
	return false;
}
