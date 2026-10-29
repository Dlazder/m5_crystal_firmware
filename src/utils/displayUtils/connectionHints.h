#pragma once

/**
 * Draws a title and a two-column pinout table with an inverted-colour
 * header (FGCOLOR fill, BGCOLOR text) and normal data rows.
 *
 * @param title       top line, white
 * @param headerLeft  text for the left column header cell
 * @param headerRight text for the right column header cell
 * @param rows        rowCount × 2 array of pin-label strings
 * @param rowCount    number of data rows
 */
static void _drawPinTable(const char* title,
													const char* headerLeft, const char* headerRight,
													const char* rows[][2], int rowCount) {
	canvas.clear();
	canvas.setTextSize(TINY_TEXT);

	int fh = canvas.fontHeight();
	int cw = canvas.width();

	// ── Measure column widths from content ──
	const int PAD = 10;
	int col1W = canvas.textWidth(headerLeft);
	int col2W = canvas.textWidth(headerRight);
	for (int i = 0; i < rowCount; i++) {
			int w1 = canvas.textWidth(rows[i][0]);
			int w2 = canvas.textWidth(rows[i][1]);
			if (w1 > col1W) col1W = w1;
			if (w2 > col2W) col2W = w2;
	}
	col1W += PAD * 2;
	col2W += PAD * 2;

	int tblW  = col1W + col2W;
	int cellH = fh + 2;             // snug cell — (cellH - fh)/2 = 1px margin top & bottom
	int vyOff = 1;                  // vertical offset to centre text inside cellH
	int tblX  = (cw - tblW) / 2;
	int col1X = tblX + PAD;
	int col2X = tblX + col1W + PAD;

	// ── Title ──
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setCursor((cw - canvas.textWidth(title)) / 2, 2);
	canvas.print(title);

	int y      = 2 + fh + 4;        // first row Y (below title)
	int totalH = (rowCount + 1) * cellH; // header + data rows

	// ── Outer frame + vertical divider (continuous lines) ──
	canvas.drawFastVLine(tblX,            y, totalH, FGCOLOR);
	canvas.drawFastVLine(tblX + col1W,    y, totalH, FGCOLOR);
	canvas.drawFastVLine(tblX + tblW - 1, y, totalH, FGCOLOR);

	// ── Header row (inverted colours) ──
	canvas.fillRect(tblX + 1, y, tblW - 2, cellH, FGCOLOR);
	canvas.drawFastHLine(tblX, y + cellH - 1, tblW, FGCOLOR);
	canvas.setTextColor(BGCOLOR, FGCOLOR);
	canvas.setCursor(col1X, y + vyOff);
	canvas.print(headerLeft);
	canvas.setCursor(col2X, y + vyOff);
	canvas.print(headerRight);

	// ── Data rows ──
	for (int i = 0; i < rowCount; i++) {
			y += cellH;
			canvas.drawFastHLine(tblX, y + cellH - 1, tblW, FGCOLOR);
			canvas.setTextColor(FGCOLOR, BGCOLOR);
			canvas.setCursor(col1X, y + vyOff);
			canvas.print(rows[i][0]);
			canvas.setCursor(col2X, y + vyOff);
			canvas.print(rows[i][1]);
	}

	canvas.pushSprite(0, getStatusBarHeight());
}


void connectionGuideNFC() {
	const char* title = "Connect PN532 module";

	// Pins are configurable (NFC > configure pins > PN532), so show the current ones
	static char sda[8], scl[8];
	snprintf(sda, sizeof(sda), "G%d", pn532SdaPin);
	snprintf(scl, sizeof(scl), "G%d", pn532SclPin);

	const char* rows[][2] = {
		{"VCC", "3.3V"},
		{"GND", "GND"},
		{"SDA", sda},
		{"SCL", scl},
	};
	int rowCount = 4;

	#if CARDPUTER
		const char* hRight = "Cardputer (GROVE)";
	#elif STICKS3
		const char* hRight = "StickS3";
	#else
		const char* hRight = "StickCP2";
	#endif

	_drawPinTable(title, "PN532 (I2C)", hRight, rows, rowCount);
}

void connectionGuideRC522() {
	const char* title = "Connect RC522 module";

	// Pins are configurable (NFC > configure pins > RC522), so show the current ones
	static char ss[8], sck[8], mosi[8], miso[8], rst[8];
	snprintf(ss, sizeof(ss), "G%d", rc522SsPin);
	snprintf(sck, sizeof(sck), "G%d", rc522SckPin);
	snprintf(mosi, sizeof(mosi), "G%d", rc522MosiPin);
	snprintf(miso, sizeof(miso), "G%d", rc522MisoPin);
	snprintf(rst, sizeof(rst), "G%d", rc522RstPin);

	const char* rows[][2] = {
		{"SDA(SS)", ss},
		{"SCK", sck},
		{"MOSI", mosi},
		{"MISO", miso},
		{"RST", rst},
		{"VCC", "3.3V"},
		{"GND", "GND"},
	};
	int rowCount = 7;

	#if CARDPUTER
		const char* hRight = "Cardputer";
	#elif STICKS3
		const char* hRight = "StickS3";
	#else
		const char* hRight = "StickCP2";
	#endif

	_drawPinTable(title, "RC522 (SPI)", hRight, rows, rowCount);
}
