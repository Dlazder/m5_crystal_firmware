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
#endif

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
