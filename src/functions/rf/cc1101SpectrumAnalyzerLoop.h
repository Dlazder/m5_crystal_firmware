// PID::RF_SPECTRUM

void cc1101SpectrumAnalyzerLoop() {
	static const int MAX_STEPS = 240;
	static const int COL_W = 3; // pixels per frequency step
	static const uint32_t FRAME_BUDGET_US = 70000; // max time spent scanning per loop()
	static const uint32_t SETTLE_US = 1100; // RX autocalibration (~0.72 ms) + RSSI settle
	static const int SCALE_LO = -110; // dBm at the bottom of the graph
	static const int SCALE_HI = -40; // dBm at the top

	struct Band {
		uint32_t startHz;
		uint32_t endHz;
		const char* name;
	};
	static const Band BANDS[] = {
		{ 300000000, 348000000, "315 MHz" },
		{ 387000000, 464000000, "433 MHz" },
		{ 779000000, 928000000, "868/915" },
		{ 433050000, 434790000, "433 zoom" },
	};
	static const int BAND_COUNT = sizeof(BANDS) / sizeof(BANDS[0]);

	static bool available = false;
	static bool sweeping = true; // still advancing through the current band
	static bool haveSweep = false; // at least one full sweep is on screen
	static int bandIndex = 1; // default to the 433 MHz band
	static int16_t spectrum[MAX_STEPS]; // dBm per step, being filled by the running sweep
	static int16_t shown[MAX_STEPS]; // last completed sweep, what the screen draws
	static int steps = 0;
	static int sweepPos = 0;
	static int peakDbm = -130, noiseDbm = -130;
	static int graphTop = 24, graphBottom = 120, graphH = 96, graphW = 240;

	if (isSetup()) {
		available = rfRadioOpen();
		if (available) {
			steps = canvas.width() / COL_W;
			if (steps > MAX_STEPS) steps = MAX_STEPS;
			if (steps < 32) steps = 32;
			graphW = steps * COL_W;

			graphTop = 24; // room for the title band
			graphH = (canvas.height() - 14 - graphTop) * 2 / 3; // two thirds of the free space
			graphBottom = graphTop + graphH;

			sweeping = true;
			sweepPos = 0;
			haveSweep = false;
		}
	}

	if (!available) {
		centeredPrint("CC1101 not found", MEDIUM_TEXT);
		if (checkExit()) rfRadioClose();
		return;
	}

	// Sweep (non-blocking)
	if (sweeping) {
		const Band& band = BANDS[bandIndex];

		if (sweepPos == 0) { // start of a fresh sweep
			ELECHOUSE_cc1101.setSidle();
			ELECHOUSE_cc1101.setRxBW(203.0f);
		}

		// Scan as many steps as fit the time budget (normally a whole sweep), so
		// the screen refreshes once per full pass instead of once per few steps.
		uint32_t t0 = micros();
		while (sweepPos < steps && (micros() - t0) < FRAME_BUDGET_US) {
			int i = sweepPos;
			uint32_t f = band.startHz +
				(uint64_t)(band.endHz - band.startHz) * i / (steps - 1);
			// IDLE → retune → RX; the chip autocalibrates on entering RX.
			ELECHOUSE_cc1101.SetRx(f / 1e6f);
			delayMicroseconds(SETTLE_US);
			int dbm = constrain(ELECHOUSE_cc1101.getRssi(), SCALE_LO, SCALE_HI);
			// Average with the previous pass: single RSSI reads jitter by a few dB.
			spectrum[i] = (int16_t)(haveSweep ? (shown[i] + dbm) / 2 : dbm);
			sweepPos++;
		}

		if (sweepPos >= steps) { // sweep complete → publish stats, start the next one
			sweepPos = 0; // keep sweeping so the trace stays live
			haveSweep = true;
			memcpy(shown, spectrum, steps * sizeof(int16_t)); // swap in the finished sweep
			peakDbm = -140;
			noiseDbm = -40;
			for (int i = 0; i < steps; i++) {
				if (shown[i] > peakDbm) peakDbm = shown[i];
				if (shown[i] < noiseDbm) noiseDbm = shown[i];
			}
		}
	}

	// Draw
	canvas.clear();
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(SMALL_TEXT);

	const Band& band = BANDS[bandIndex];
	char title[48];
	snprintf(title, sizeof(title), "%s peak:%d flr:%d", band.name, peakDbm, noiseDbm);
	canvas.setCursor(2, 2);
	canvas.print(title);

	// Fixed scale: a quiet band stays a flat floor instead of being stretched into
	// noise, and real signals stand out.
	for (int i = 0; i < steps; i++) {
		// Columns already re-measured by the running sweep are shown fresh.
		if (i >= sweepPos && !haveSweep) break;
		int16_t v = (i < sweepPos) ? spectrum[i] : shown[i];
		int y = map(v, SCALE_LO, SCALE_HI, graphBottom, graphTop);
		canvas.fillRect(i * COL_W, y, COL_W, graphBottom - y, FGCOLOR);
	}

	// Frequency labels at the extremes
	canvas.setTextSize(TINY_TEXT);
	char lo[16], hi[16];
	snprintf(lo, sizeof(lo), "%.0fM", band.startHz / 1e6);
	snprintf(hi, sizeof(hi), "%.0fM", band.endHz / 1e6);
	canvas.setCursor(2, graphBottom + 2);
	canvas.print(lo);
	canvas.setCursor(graphW - canvas.textWidth(hi) - 2, graphBottom + 2);
	canvas.print(hi);

	drawHintSwitch();

	canvas.pushSprite(0, getStatusBarHeight());

	if (isBtnAWasPressed()) {
		bandIndex = (bandIndex + 1) % BAND_COUNT;
		sweepPos = 0;
		haveSweep = false; // old trace belongs to the previous band
		sweeping = true; // re-sweep the newly selected band
	}

	if (checkExit()) {
		rfRadioClose();
	}
}
