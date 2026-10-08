// PID::UART_TERMINAL
//
// A serial terminal over the GROVE UART (HardwareSerial2). Received bytes are
// displayed as a scrolling log; press A / enter to open the keyboard and send
// a line over the wire. Pins and baud are configured via UART → RX/TX pin / baud rate.

#include <HardwareSerial.h>

#define UART_BUF_LINES 200
#define UART_MAX_LINE 96
#define UART_FLUSH_MS 300 // commit a pending line after this many ms of no new bytes

static HardwareSerial* _uartPort = nullptr;
static bool _uartKbActive = false;

static String _uartRing[UART_BUF_LINES];
static int _uartHead = 0; // slot the next line is written into
static int _uartCount = 0; // number of valid lines in the buffer
static int _uartScroll = 0; // 0 = newest (bottom); >0 = scrolled up toward older data
static String _uartPending = ""; // partial line still being received
static int _uartLastRx = 0; // globalTimer value of the last received byte
static bool _uartDirty = true;
static int _uartVisibleCount = 1;
static File _uartLogFile;         // open log handle (invalid when logging is off)

static void _uartReset() {
	_uartHead = 0;
	_uartCount = 0;
	_uartScroll = 0;
	_uartPending = "";
	_uartDirty = true;
}

// --- file logging (SD card first, LittleFS fallback) ---

static void _uartLogOpen() {
	_uartLogFile = Storage::openUnique("/uart", ".log");
}

static void _uartLogWrite(const String& line) {
	if (_uartLogFile) _uartLogFile.println(line);
}

static void _uartLogClose() {
	if (_uartLogFile) _uartLogFile.close();
}

static void _uartCommit(const String& line) {
	_uartRing[_uartHead] = line;
	_uartHead = (_uartHead + 1) % UART_BUF_LINES;
	if (_uartCount < UART_BUF_LINES) _uartCount++;
}

// i = 0 is the oldest buffered line, i = _uartCount - 1 is the newest.
static String& _uartLine(int i) {
	int idx = _uartHead - _uartCount + i;
	while (idx < 0) idx += UART_BUF_LINES;
	return _uartRing[idx % UART_BUF_LINES];
}

// How many log lines fit on screen (below a reserved hint strip).
static void _uartLayout() {
	canvas.setTextSize(TINY_TEXT);
	int lineH = canvas.fontHeight();
	int visible = (canvas.height() - (lineH + 6)) / lineH - 1; // -1 leaves room for the live partial line
	_uartVisibleCount = (visible < 1) ? 1 : visible;
}

static void _uartDraw() {
	canvas.clear();
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(TINY_TEXT);
	int lineH = canvas.fontHeight();

	int bottom = _uartCount - 1 - _uartScroll;
	if (bottom >= _uartCount) bottom = _uartCount - 1;
	int top = bottom - _uartVisibleCount + 1;
	if (top < 0) top = 0;

	int y = 2;
	for (int i = top; i <= bottom; i++) {
		canvas.setCursor(4, y);
		canvas.print(_uartLine(i).c_str());
		y += lineH;
	}

	// Live partial line (data received without a trailing newline yet).
	if (_uartScroll == 0 && _uartPending.length() > 0) {
		canvas.setCursor(4, y);
		canvas.print(_uartPending.c_str());
	}

	_uartDirty = false;
	drawHintCustom("enter: send  ;/: scroll", "A: send  PWR: scroll");
}

static bool _uartReadStream(Stream& stream) {
	bool got = false;
	while (stream.available()) {
		char c = (char)stream.read();
		got = true;
		_uartLastRx = globalTimer;
		if (c == '\n' || c == '\r') {
			// Treat LF, CR and CRLF all as line terminators.
			if (c == '\r' && stream.peek() == '\n') stream.read(); // swallow the LF of a CRLF pair
			_uartCommit(_uartPending);
			_uartLogWrite(_uartPending);
			handleSerialCommandLine(_uartPending, stream); // run /commands, reply to the same stream
			_uartPending = "";
			if (_uartScroll > 0) _uartScroll++; // keep the view pinned while scrolled up
		} else {
			_uartPending += c;
			if (_uartPending.length() > UART_MAX_LINE)
				_uartPending = _uartPending.substring(_uartPending.length() - UART_MAX_LINE);
		}
	}
	return got;
}

static void _uartPoll() {
	bool got = false;
	if (_uartPort) got = _uartReadStream(*_uartPort); // GPIO UART
	if (_uartReadStream(Serial)) got = true;          // USB Serial

	// If a sender ends a message without a newline, commit it once the stream
	// goes quiet for UART_FLUSH_MS, so every message lands on its own line.
	if (_uartPending.length() > 0 && (globalTimer - _uartLastRx) >= UART_FLUSH_MS) {
		_uartCommit(_uartPending);
		_uartLogWrite(_uartPending);
		_uartPending = "";
		got = true;
	}

	if (got) _uartDirty = true;
}

static void _uartSend(const char* buf) {
	if (buf[0] == '\0') return;
	_uartCommit("> " + String(buf)); // local echo so the sender sees what went out
	_uartLogWrite("> " + String(buf));
	if (_uartPort) {
		_uartPort->print(buf);
		_uartPort->print('\n');
	}
	_uartScroll = 0;
	_uartDirty = true;
}

void uartTerminalLoop() {
	if (isSetup()) {
		_uartReset();
		_uartLogClose(); // ensure no stale handle from a previous session
		_uartKbActive = false;
		_uartLayout();
		_uartPort = &Serial2;
		_uartPort->end();
		_uartPort->begin(uartBaud, SERIAL_8N1, uartRxPin, uartTxPin);
		_uartDraw();
		if (uartLogEnabled) _uartLogOpen();
		Serial.printf("UART terminal: %lu baud RX=G%d TX=G%d\n",
			(unsigned long)uartBaud, uartRxPin, uartTxPin);
	}

	if (_uartKbActive) {
		keyboardLoop(
			[]() { _uartKbActive = false; kbEnd(); _uartDraw(); },
			[](const char* buf) {
				if (kbLen > 0) _uartSend(buf);
				_uartKbActive = false; kbEnd(); _uartDraw();
			},
			nullptr
		);
		return;
	}

	_uartPoll();
	if (_uartDirty) _uartDraw();

	// Open the keyboard to send a line.
	if (isBtnAWasPressed() || isKbEnterPressed()) {
		_uartKbActive = true;
		kbReset();
		drawKeyboardUi();
		return;
	}

	// Scroll toward older data (physical: up key; buttons: PWR, stops at oldest line).
	if (isKbUpPressed() || isBtnPWRWasPressed()) {
		int maxScroll = _uartCount - _uartVisibleCount;
		if (maxScroll < 0) maxScroll = 0;
		if (_uartScroll < maxScroll) _uartScroll++;
		_uartDraw();
	}
	// Scroll toward newer data (physical keyboard only).
	if (isKbDownPressed() && _uartScroll > 0) {
		_uartScroll--;
		_uartDraw();
	}

	if (checkExit()) {
		_uartLogClose();
		if (_uartPort) {
			_uartPort->end();
			_uartPort->begin(uartBaud, SERIAL_8N1, uartRxPin, uartTxPin); // restore GPIO command listening
		}
		_uartPort = nullptr;
	}
}
