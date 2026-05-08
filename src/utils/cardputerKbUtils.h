#ifdef CARDPUTER
	#include <M5Cardputer.h>

	bool kbUpPressed = false;
	bool kbDownPressed = false;
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
