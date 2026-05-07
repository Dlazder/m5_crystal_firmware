static constexpr int SAMPLE_RATE = 44000;
static constexpr size_t SAMPLES = 4096;
static int16_t DATA[SAMPLES];

// Calibration table (phone_dB, m5_raw + 96db)
static const float CALIB_TABLE[][2] = {
	{6.0f, 45.0f},
	{12.0f, 45.0f},
	{15.0f, 45.0f},
	{31.0f, 46.0f},
	{40.0f, 47.0f},
	{45.0f, 49.0f},
	{58.0f, 51.0f},
	{64.0f, 54.0f},
	{71.0f, 61.0f},
	{81.0f, 68.0f},
	{91.0f, 75.0f},
	{100.0f, 80.0f}
};
static constexpr int CALIB_TABLE_SIZE = 12;

static float calibrateSoundLevel(float m5_raw) {
	// Linear interpolation between calibration points
	if (m5_raw <= CALIB_TABLE[0][1]) {
		return CALIB_TABLE[0][0]; // Clamp to min
	}
	if (m5_raw >= CALIB_TABLE[CALIB_TABLE_SIZE - 1][1]) {
		return CALIB_TABLE[CALIB_TABLE_SIZE - 1][0]; // Clamp to max
	}
	
	// Find the two points to interpolate between
	for (int i = 0; i < CALIB_TABLE_SIZE - 1; i++) {
		if (m5_raw >= CALIB_TABLE[i][1] && m5_raw <= CALIB_TABLE[i + 1][1]) {
			float x1 = CALIB_TABLE[i][1];
			float y1 = CALIB_TABLE[i][0];
			float x2 = CALIB_TABLE[i + 1][1];
			float y2 = CALIB_TABLE[i + 1][0];
			// Linear interpolation: y = y1 + (x - x1) * (y2 - y1) / (x2 - x1)
			return y1 + (m5_raw - x1) * (y2 - y1) / (x2 - x1);
		}
	}
	return m5_raw;
}

float getSoundLevel() {
	DEVICE.Mic.record(DATA, SAMPLES, SAMPLE_RATE, false);
	
	// RMS calc
	double sum = 0;
	for (size_t i = 0; i < SAMPLES; i++) {
		float s = DATA[i] / 32768.0f;
		sum += s * s;
	}
	float rms = sqrt(sum / SAMPLES);
	
	// dB into SPL with table-based calibration
	float dbFS = 20 * log10(rms + 1e-12);
	float m5_raw = dbFS + 94.0f;
	float dbSPL = calibrateSoundLevel(m5_raw);
	
	return dbSPL;
}

void soundLevelLoop() {
	static int statusBarHeight = statusBar ? 30 : 0;
	static int barX = 0;
	static int barY;
	static int barW = DISP.width();
	static int barH = 20;
	

	// Graph
	static const int GRAPH_POINTS = 80;
	static float soundHistory[GRAPH_POINTS];
	static int historyIndex = 0;
	static int historySize = 0;
	static int graphH = 50;
	static int graphY = DISP.height() - statusBarHeight - graphH;
	static int frameCount = 0;

	if (isSetup()) {
		DEVICE.Mic.begin();
		canvas.setTextColor(FGCOLOR);
		canvas.setTextSize(MEDIUM_TEXT);

		statusBarHeight = statusBar ? 30 : 0;
		barY = DISP.height() / 2 - (barH / 2) - statusBarHeight;
		barY = graphY - barH;

		// Clear graph data
		memset(soundHistory, 0, sizeof(soundHistory));
		historyIndex = 0;
		historySize = 0;
	}

	if (checkTimer(50)) {
		updateTimer();
		float soundLevel = getSoundLevel();
		int fillWidth = map(constrain(soundLevel, 0.0f, 100.0f), 0, 100, 0, barW);
		

		soundHistory[historyIndex] = soundLevel;
		historyIndex = (historyIndex + 1) % GRAPH_POINTS;
		if (historySize < GRAPH_POINTS) historySize++;

		// Bar
		canvas.clear();
		int halfWidth = barW / 2;
		canvas.fillGradientRect(barX, barY, halfWidth, barH, TFT_GREEN, TFT_YELLOW, lgfx::gradient_fill_styles::HLINEAR);
		canvas.fillGradientRect(barX + halfWidth, barY, halfWidth, barH, TFT_YELLOW, TFT_RED, lgfx::gradient_fill_styles::HLINEAR);
		if (fillWidth < barW) {
			canvas.fillRect(barX + fillWidth, barY, barW - fillWidth, barH, TFT_BLACK);
		}
		canvas.drawLine(barX, barY, barW, barY, FGCOLOR);

		// Graph
		canvas.drawLine(0, graphY, DISP.width(), graphY, FGCOLOR);
		if (historySize > 1) {
			int startIndex = historyIndex - historySize;
			if (startIndex < 0) startIndex += GRAPH_POINTS;
			for (int i = 0; i < historySize - 1; i++) {
				int idx1 = (startIndex + i) % GRAPH_POINTS;
				int idx2 = (startIndex + i + 1) % GRAPH_POINTS;
				int x1 = i * 3;
				int y1 = graphY + graphH - map(soundHistory[idx1], 0, 100, 0, graphH);
				int x2 = (i + 1) * 3;
				int y2 = graphY + graphH - map(soundHistory[idx2], 0, 100, 0, graphH);
				canvas.drawLine(x1, y1, x2, y2, FGCOLOR);
			}
		}

		// Average and max values under the graph
		float avgLevel = 0.0f;
		float maxLevel = 0.0f;
		if (historySize > 0) {
			maxLevel = soundHistory[0];
			float sumLevel = 0.0f;
			for (int i = 0; i < historySize; i++) {
				float value = soundHistory[i];
				sumLevel += value;
				if (value > maxLevel) maxLevel = value;
			}
			avgLevel = sumLevel / historySize;
		}
		
		char stats[32];
		snprintf(stats, sizeof(stats), "%d  AVG:%d MAX:%d", int(soundLevel), int(avgLevel), int(maxLevel));
		canvas.drawCenterString(stats, DISP.width() / 2, barY - 16 - 10);

		canvas.pushSprite(0, statusBarHeight + 1);
	}

	if (isBtnAWasPressed()) {
		memset(soundHistory, 0, sizeof(soundHistory));
		historyIndex = 0;
		historySize = 0;
	}

	if (checkExit()) {
		DEVICE.Mic.end();
	}
}