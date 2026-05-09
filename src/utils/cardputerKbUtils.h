#ifdef CARDPUTER
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
	#ifdef CARDPUTER
	if (kbUpPressed) { kbUpPressed = false; return true; }
	#endif
	return false;
}

bool isKbDownPressed() {
	#ifdef CARDPUTER
	if (kbDownPressed) { kbDownPressed = false; return true; }
	#endif
	return false;
}

bool isKbEnterPressed() {
	#ifdef CARDPUTER
	if (kbEnterPressed) { kbEnterPressed = false; return true; }
	#endif
	return false;
}

bool isKbEscPressed() {
	#ifdef CARDPUTER
	if (kbEscPressed) { kbEscPressed = false; return true; }
	#endif
	return false;
}

bool isKbLeftPressed() {
	#ifdef CARDPUTER
	if (kbLeftPressed) { kbLeftPressed = false; return true; }
	#endif
	return false;
}

bool isKbRightPressed() {
	#ifdef CARDPUTER
	if (kbRightPressed) { kbRightPressed = false; return true; }
	#endif
	return false;
}

bool isKbPlusPressed() {
	#ifdef CARDPUTER
	if (kbPlusPressed) { kbPlusPressed = false; return true; }
	#endif
	return false;
}

bool isKbMinusPressed() {
	#ifdef CARDPUTER
	if (kbMinusPressed) { kbMinusPressed = false; return true; }
	#endif
	return false;
}
