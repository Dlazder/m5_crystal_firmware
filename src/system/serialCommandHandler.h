// Executes a single command line, writing any response to `out`.
// Lines that don't start with '/' are ignored, so raw UART traffic passes through untouched.
void handleSerialCommandLine(String cmd, Print& out) {
	// Check the raw line before trimming: indented output (e.g. the help text)
	// starts with a space, so it is never re-read as a command when UART RX is
	// looped back to TX.
	if (!cmd.startsWith("/")) return;

	cmd.trim();

	// Split off the command word from its (optional) argument, e.g.
	// "/goto WIFI_SCAN" -> command="/goto", arg="WIFI_SCAN".
	int spaceIdx = cmd.indexOf(' ');
	String command = (spaceIdx == -1) ? cmd : cmd.substring(0, spaceIdx);
	String arg = (spaceIdx == -1) ? "" : cmd.substring(spaceIdx + 1);

	if (command == "/screenshot") {
		screenshotCommand(out);
	} else if (command == "/info") {
		infoCommand(out);
	} else if (command == "/help") {
		helpCommand(out);
	} else if (command == "/shutdown") {
		shutdownCommand(out);
	} else if (command == "/goto") {
		gotoCommand(arg, out);
	} else if (command == "/up") {
		upCommand(out);
	} else if (command == "/down") {
		downCommand(out);
	} else if (command == "/select") {
		selectCommand(out);
	} else if (command == "/back") {
		backCommand(out);
	} else {
		out.println("Command not found!");
	}
}

// Per-port line accumulator. Never waits for data: it consumes whatever is
// already in the RX buffer and runs a command once the line is complete.
#define SERIAL_CMD_IDLE_MS 100

struct SerialLineReader {
	static const size_t MAX_LEN = 128;
	char buf[MAX_LEN];
	size_t len = 0;
	bool discard = false;
	uint32_t lastRx = 0;
};

static SerialLineReader serialUsbReader;
static SerialLineReader serialUartReader;

static SerialLineReader& _serialReaderFor(Stream& port) {
	return (&port == &Serial2) ? serialUartReader : serialUsbReader;
}

// Runs the buffered line (if it is a command) and resets the reader.
static void _serialFlushLine(SerialLineReader& r, Stream& port) {
	if (!r.discard && r.len > 0) {
		r.buf[r.len] = '\0';
		handleSerialCommandLine(String(r.buf), port);
	}
	r.len = 0;
	r.discard = false;
}

// Reads pending bytes from `port` and hands complete lines to handleSerialCommandLine.
void handleSerialCommands(Stream& port) {
	SerialLineReader& r = _serialReaderFor(port);

	while (port.available()) {
		char c = (char)port.read();
		r.lastRx = millis();

		// CRLF: the '\r' ends the line, the '\n' then arrives on an empty reader and is a no-op.
		if (c == '\n' || c == '\r') {
			_serialFlushLine(r, port);
			continue;
		}
		if (r.discard) continue;

		// Commands start with '/': anything else (UART traffic, line noise) is
		// skipped right away instead of being buffered.
		if (r.len == 0 && c != '/') {
			r.discard = true;
			continue;
		}
		if (r.len >= SerialLineReader::MAX_LEN - 1) {
			r.len = 0;
			r.discard = true;
			continue;
		}
		r.buf[r.len++] = c;
	}

	// No line ending from the sender: run what we have once the port goes quiet.
	if ((r.len > 0 || r.discard) && millis() - r.lastRx >= SERIAL_CMD_IDLE_MS) {
		_serialFlushLine(r, port);
	}
}
