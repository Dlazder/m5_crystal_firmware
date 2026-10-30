// PID::NRF_SPECTRUM

void nrfSpectrumAnalyzerLoop() {
	static const int CHANNELS = NRF24::CHANNEL_COUNT; // 2400..2525 MHz
	static const uint32_t FRAME_BUDGET_US = 40000; // max time spent scanning per loop()
	static const int HIT_GAIN = 40; // level added when a carrier is seen on a channel
	static const int LEVEL_MAX = 100;
	static const int DECAY_EVERY = 4; // sweeps between one-step decays of every level

	static bool available = false;
	static bool paused = false;
	static uint8_t level[CHANNELS]; // activity per channel, 0..LEVEL_MAX
	static uint8_t sweepCount = 0;
	static int peakChannel = 0;
	static int graphTop = 24, graphBottom = 120, graphH = 96;
	static int colW = 1;

	if (isSetup()) {
		available = nrfRadioOpen();
		if (available) {
			paused = false;
			memset(level, 0, sizeof(level));
			sweepCount = 0;
			peakChannel = 0;

			colW = canvas.width() / CHANNELS;
			if (colW < 1) colW = 1;

			graphTop = 24; // room for the title band
			graphH = (canvas.height() - 14 - graphTop) * 2 / 3; // two thirds of the free space
			graphBottom = graphTop + graphH;
		}
	}

	if (!available) {
		String lines[] = {
			"NRF24 not found",
			"CSN" + String(nrfCsnPin) + " SCK" + String(nrfSckPin) + " MOSI" + String(nrfMosiPin),
			"MISO" + String(nrfMisoPin) + " CE" + String(nrfCePin),
		};
		centeredPrintRows(lines, 3, SMALL_TEXT);
		if (checkExit()) nrfRadioClose();
		return;
	}

	// Sweep (non-blocking): as many full passes as fit the time budget, at least one.
	if (!paused) {
		uint32_t t0 = micros();
		do {
			for (int ch = 0; ch < CHANNELS; ch++) {
				if (NRF24::carrierOnChannel(ch)) {
					level[ch] = min(LEVEL_MAX, level[ch] + HIT_GAIN);
				}
			}
			if (++sweepCount >= DECAY_EVERY) {
				sweepCount = 0;
				for (int ch = 0; ch < CHANNELS; ch++) {
					if (level[ch] > 0) level[ch]--;
				}
			}
		} while ((micros() - t0) < FRAME_BUDGET_US);

		peakChannel = 0;
		for (int ch = 1; ch < CHANNELS; ch++) {
			if (level[ch] > level[peakChannel]) peakChannel = ch;
		}
	}

	// Draw
	canvas.clear();
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(SMALL_TEXT);

	char title[48];
	if (level[peakChannel] > 0) {
		snprintf(title, sizeof(title), "2.4G peak:%d MHz%s", 2400 + peakChannel, paused ? " ||" : "");
	} else {
		snprintf(title, sizeof(title), "2.4G quiet%s", paused ? " ||" : "");
	}
	canvas.setCursor(2, 2);
	canvas.print(title);

	for (int ch = 0; ch < CHANNELS; ch++) {
		if (level[ch] == 0) continue;
		int y = map(level[ch], 0, LEVEL_MAX, graphBottom, graphTop);
		canvas.fillRect(ch * colW, y, colW, graphBottom - y, FGCOLOR);
	}

	// Frequency labels at the extremes
	canvas.setTextSize(TINY_TEXT);
	const char* lo = "2400M";
	const char* hi = "2525M";
	canvas.setCursor(2, graphBottom + 2);
	canvas.print(lo);
	canvas.setCursor(CHANNELS * colW - canvas.textWidth(hi) - 2, graphBottom + 2);
	canvas.print(hi);

	drawHintCustom("enter: pause", "A: pause");

	canvas.pushSprite(0, getStatusBarHeight());

	if (isBtnAWasPressed() || isKbEnterPressed()) {
		paused = !paused;
	}

	if (checkExit()) {
		nrfRadioClose();
	}
}
