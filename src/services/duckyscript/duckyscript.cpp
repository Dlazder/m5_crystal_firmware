#include "duckyscript.h"

namespace Duckyscript {

// Timing, in milliseconds. Same cadence as the original blocking delays.
static const unsigned long STRING_CHAR_MS = 10;
static const unsigned long COMBO_HOLD_MS = 50;

// A command that takes more than one tick to finish.
enum class Mode { Ready, Typing, ComboHold };

static const String* s_lines = nullptr;
static int s_lineCount = 0;
static int s_currentLine = 0; // next line to run (also drives the highlight)
static Sink s_sink = { nullptr, nullptr, nullptr };

// In-progress command state.
static Mode s_mode = Mode::Ready;
static unsigned long s_delayUntil = 0; // pending DELAY deadline (0 = none)
static String s_stringText;            // remaining STRING payload
static int s_stringPos = 0;            // cursor into s_stringText
static unsigned long s_stringNext = 0; // when to emit the next character
static unsigned long s_comboRelease = 0; // when to release a held COMBO

void begin(const String* lines, int count, const Sink& sink) {
	end();
	s_lines = lines;
	s_lineCount = count;
	s_sink = sink;
}

void rewind() {
	s_currentLine = 0;
	s_mode = Mode::Ready;
	s_delayUntil = 0;
	s_stringPos = 0;
	s_stringText = "";
	s_stringNext = 0;
	s_comboRelease = 0;
}

void end() {
	if (s_lines != nullptr) {
		delete[] s_lines;
		s_lines = nullptr;
	}
	s_lineCount = 0;
	s_sink = { nullptr, nullptr, nullptr };
	rewind();
}

int lineCount() { return s_lineCount; }
const String* lines() { return s_lines; }
int currentLine() { return s_currentLine; }

// DuckyScript key name -> ESP32 HID "special key" code (0x80+). Values match the
// BleCombo and native USBHID key tables exactly, so the same code drives both
// transports. Plain ASCII (SPACE -> ' ') passes through unchanged.
uint8_t resolveKey(const String& name) {
	static const struct {
		const char* name;
		uint8_t code;
	} table[] = {
		{"ENTER",     0xB0},
		{"BACKSPACE", 0xB2},
		{"TAB",       0xB3},
		{"ESC",       0xB1},
		{"DELETE",    0xD4},
		{"INSERT",    0xD1},
		{"HOME",      0xD2},
		{"END",       0xD5},
		{"PAGEUP",    0xD3},
		{"PAGEDOWN",  0xD6},
		{"UP",        0xDA},
		{"DOWN",      0xD9},
		{"LEFT",      0xD8},
		{"RIGHT",     0xD7},
		{"SPACE",     ' '},
		{"GUI",       0x83},
		{"CTRL",      0x80},
		{"SHIFT",     0x81},
		{"ALT",       0x82},
		{"F1",  0xC2},
		{"F2",  0xC3},
		{"F3",  0xC4},
		{"F4",  0xC5},
		{"F5",  0xC6},
		{"F6",  0xC7},
		{"F7",  0xC8},
		{"F8",  0xC9},
		{"F9",  0xCA},
		{"F10", 0xCB},
		{"F11", 0xCC},
		{"F12", 0xCD},
	};
	for (const auto& entry : table) {
		if (name == entry.name) return entry.code;
	}
	return 0;
}

Progress step() {
	if (s_lines == nullptr) return Progress::Finished;

	// Drain a pending DELAY before touching any further state (a trailing DELAY
	// must still elapse before the script reports Finished).
	if (s_delayUntil) {
		if (millis() < s_delayUntil) return Progress::Busy;
		s_delayUntil = 0;
	}

	// Drain an in-progress STRING, one character per STRING_CHAR_MS.
	if (s_mode == Mode::Typing) {
		if (millis() < s_stringNext) return Progress::Busy;
		s_sink.write((uint8_t)s_stringText[s_stringPos++]);
		if (s_stringPos < (int)s_stringText.length()) {
			s_stringNext = millis() + STRING_CHAR_MS;
			return Progress::Busy;
		}
		s_mode = Mode::Ready;
		return Progress::Busy; // line finished; next call parses the next line
	}

	// Drain an in-progress COMBO, holding for COMBO_HOLD_MS before releasing.
	if (s_mode == Mode::ComboHold) {
		if (millis() < s_comboRelease) return Progress::Busy;
		s_sink.releaseAll();
		s_mode = Mode::Ready;
		return Progress::Busy;
	}

	// Nothing in progress — advance to the next line, or report finished.
	if (s_currentLine >= s_lineCount) return Progress::Finished;

	// Parse + start the next line. currentLine is advanced up-front so the
	// highlight keeps its "next line to run" semantics (as before).
	String line = s_lines[s_currentLine++];

	if (line.length() == 0 || line.startsWith("REM") || line.startsWith("//"))
		return Progress::LineAdvanced;

	if (line.startsWith("DELAY ")) {
		s_delayUntil = millis() + (unsigned long)line.substring(6).toInt();
	} else if (line.startsWith("STRING ")) {
		s_stringText = line.substring(7);
		s_stringPos = 0;
		s_mode = Mode::Typing;
		s_stringNext = millis(); // first character emits on the next tick
	} else if (line.startsWith("KEY ")) {
		String keyName = line.substring(4);
		keyName.trim();
		uint8_t k = resolveKey(keyName);
		if (k) s_sink.write(k);
	} else if (line.startsWith("COMBO ")) {
		String args = line.substring(6);
		args.trim();
		while (args.length() > 0) {
			int sp = args.indexOf(' ');
			String token = (sp == -1) ? args : args.substring(0, sp);
			token.trim();
			uint8_t k = resolveKey(token);
			if (!k && token.length() == 1) k = (uint8_t)token[0];
			if (k) s_sink.press(k);
			args = (sp == -1) ? "" : args.substring(sp + 1);
		}
		s_mode = Mode::ComboHold;
		s_comboRelease = millis() + COMBO_HOLD_MS;
	}

	return Progress::LineAdvanced;
}

}
