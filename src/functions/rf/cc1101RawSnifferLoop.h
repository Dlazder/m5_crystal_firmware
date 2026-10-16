// PID::RF_RAW_SNIFFER

// Edge timing runs in a GPIO interrupt. On the classic ESP32 (M5StickC Plus2) IRAM is
// almost full, so the handler stays in flash there; that is safe because the sniffer
// never writes flash (LittleFS/NVS) while listening. Other boards keep it in IRAM.
#ifdef ESP32S3
	#define RF_EDGE_ISR_ATTR IRAM_ATTR
#else
	#define RF_EDGE_ISR_ATTR
#endif

static const int RF_EDGE_BUF = 512;
static volatile uint16_t rfEdgeBuf[RF_EDGE_BUF]; // duration (µs) of the state that just ended
static volatile uint16_t rfEdgeHead = 0;
static volatile uint16_t rfEdgeTail = 0;
static volatile uint32_t rfEdgeLast = 0;

static void RF_EDGE_ISR_ATTR rfEdgeIsr() {
	uint32_t now = micros();
	uint32_t dt = now - rfEdgeLast;
	rfEdgeLast = now;
	uint16_t next = (rfEdgeHead + 1) % RF_EDGE_BUF;
	if (next == rfEdgeTail) return; // buffer full, drop
	rfEdgeBuf[rfEdgeHead] = dt > 0xFFFF ? 0xFFFF : (uint16_t)dt;
	rfEdgeHead = next;
}

void cc1101RawSnifferLoop() {
	static const uint32_t PRESETS[] = {
		315000000, 433920000, 868000000, 915000000,
	};
	static const int PRESET_COUNT = sizeof(PRESETS) / sizeof(PRESETS[0]);
	static const int MAX_LINES = 20; // text lines kept on screen
	static const int MAX_FRAME = 96; // pulses kept per frame
	static const uint16_t GAP_US = 5000; // silence at least this long ends a frame
	static const uint16_t MIN_PULSE_US = 60; // shorter than this is noise
	static const int MIN_EDGES = 16; // shorter frames are noise

	static bool available = false;
	static int freqIndex = 1; // default 433.92 MHz
	static uint8_t lineBuf[MAX_LINES][64]; // rendered text lines (ring)
	static int lineStart = 0; // oldest line
	static int lineCount = 0;
	static int captureRssi = -130;
	static uint16_t frame[MAX_FRAME];
	static int frameLen = 0;
	static uint32_t totalEdges = 0;

	if (isSetup()) {
		available = rfRadioOpen();
		if (available) {
			ELECHOUSE_cc1101.SetRx(PRESETS[freqIndex] / 1e6f);

			lineStart = 0;
			lineCount = 0;
			captureRssi = -130;
			frameLen = 0;
			totalEdges = 0;

			rfEdgeHead = rfEdgeTail = 0;
			rfEdgeLast = micros();
			attachInterrupt(digitalPinToInterrupt(rfGdo0Pin), rfEdgeIsr, CHANGE);
		}
		canvas.setTextColor(FGCOLOR, BGCOLOR);
	}

	if (!available) {
		centeredPrint("CC1101 not found", MEDIUM_TEXT);
		if (checkExit()) rfRadioClose();
		return;
	}

	// Drain captured edges into frames
	while (rfEdgeTail != rfEdgeHead) {
		uint16_t dt = rfEdgeBuf[rfEdgeTail];
		rfEdgeTail = (rfEdgeTail + 1) % RF_EDGE_BUF;
		totalEdges++;

		bool frameDone = false;
		if (dt >= GAP_US) {
			frameDone = true;
		} else if (dt < MIN_PULSE_US) {
			frameLen = 0; // glitch: the frame in progress is not trustworthy
		} else {
			frame[frameLen++] = dt;
			if (frameLen == MAX_FRAME) frameDone = true;
		}

		if (frameDone) {
			if (frameLen >= MIN_EDGES) {
				captureRssi = ELECHOUSE_cc1101.getRssi();

				// Screen line: "<count>: <µs> <µs> ..." (as many as fit).
				char* line = (char*)lineBuf[lineStart];
				int off = snprintf(line, 64, "%d:", frameLen);
				for (int i = 0; i < frameLen && off < 58; i++) {
					off += snprintf(line + off, 64 - off, " %u", (unsigned)frame[i]);
				}

				// Full frame to Serial for desktop capture.
				Serial.printf("%u kHz %d:", (unsigned)(PRESETS[freqIndex] / 1000), frameLen);
				for (int i = 0; i < frameLen; i++) Serial.printf(" %u", (unsigned)frame[i]);
				Serial.println();

				lineStart = (lineStart + 1) % MAX_LINES;
				if (lineCount < MAX_LINES) lineCount++;
			}
			frameLen = 0;
		}
	}

	// Draw
	canvas.clear();
	canvas.setTextSize(SMALL_TEXT);
	char head[48];
	snprintf(head, sizeof(head), "%.2f MHz rssi:%d e:%lu", PRESETS[freqIndex] / 1e6, captureRssi, (unsigned long)totalEdges);
	canvas.setCursor(2, 2);
	canvas.print(head);

	canvas.setTextSize(TINY_TEXT);
	int fh = canvas.fontHeight();
	int y = canvas.fontHeight() + 6;
	for (int i = 0; i < lineCount; i++) {
		// Ring is oldest-first: index 0 is lineStart, newest is last written.
		int idx = (lineStart - lineCount + i + MAX_LINES * 2) % MAX_LINES;
		canvas.setCursor(2, y);
		canvas.print((const char*)lineBuf[idx]);
		y += fh;
		if (y > canvas.height() - fh) break;
	}

	if (lineCount == 0) {
		drawSpinner();
	}

	canvas.pushSprite(0, getStatusBarHeight());

	if (isBtnAWasPressed()) {
		freqIndex = (freqIndex + 1) % PRESET_COUNT;
		ELECHOUSE_cc1101.SetRx(PRESETS[freqIndex] / 1e6f);
		lineStart = 0;
		lineCount = 0;
		frameLen = 0;
	}

	if (checkExit()) {
		detachInterrupt(digitalPinToInterrupt(rfGdo0Pin));
		rfRadioClose();
	}
}
