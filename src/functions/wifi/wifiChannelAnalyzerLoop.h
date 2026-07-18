// pid PID::WIFI_CHANNEL_ANALYZER

#define CH_ANALYZER_MAX_CH 13

void wifiChannelAnalyzerLoop() {
	if (isSetup()) {
		if (wifiCount == 0) {
			centeredPrint(L->TXT_WIFI_NO_SCAN_DATA, MEDIUM_TEXT);
			return;
		}

		// ---------- collect channel histogram ----------
		int counts[CH_ANALYZER_MAX_CH] = {0};
		int maxCount = 0;
		int bestChannel = 1;
		int bestCount = INT_MAX;

		for (int i = 0; i < wifiCount; i++) {
			int ch = WiFi.channel(i);
			if (ch < 1 || ch > CH_ANALYZER_MAX_CH) continue;
			counts[ch - 1]++;
			if (counts[ch - 1] > maxCount) {
				maxCount = counts[ch - 1];
			}
		}
		// best channel = least congested (ties → lowest channel number)
		for (int i = 0; i < CH_ANALYZER_MAX_CH; i++) {
			if (counts[i] < bestCount) {
				bestCount = counts[i];
				bestChannel = i + 1;
			}
		}

		// layout
		canvas.fillSprite(BGCOLOR);

		const int cw = canvas.width();
		const int chH = canvas.height();

		// title: "BEST CH: X total: Y"
		canvas.setTextSize(TINY_TEXT);
		canvas.setTextColor(TFT_DARKGRAY, BGCOLOR);
		String bestStr = "BEST CH: " + String(bestChannel);
		String totalStr = " 	TOTAL: " + String(wifiCount);
		int fh = canvas.fontHeight();
		canvas.setCursor(6, 4);
		canvas.print(bestStr.c_str());
		canvas.print(totalStr.c_str());

		// Chart bounds
		const int TITLE_H = fh + 8; // space below title
		const int BOTTOM_LABEL_H = 14; // room for channel numbers
		const int TOP_GAP = 8; // gap above bar-top labels
		const int MARGIN_L = 6;
		const int MARGIN_R = 6;

		int chartTop = TITLE_H + TOP_GAP;
		int chartBottom = chH - BOTTOM_LABEL_H;
		int chartHeight = chartBottom - chartTop;
		int chartWidth = cw - MARGIN_L - MARGIN_R;

		// 100 % reference line
		canvas.drawFastHLine(MARGIN_L, chartTop, chartWidth, TFT_DARKGRAY);
		// boundary above channel numbers
		canvas.drawFastHLine(MARGIN_L, chartBottom, chartWidth, FGCOLOR);
		// intermediate grid lines (25 %, 50 %, 75 %)
		for (int i = 1; i <= 3; i++) {
			int gy = chartTop + (chartHeight * i) / 4;
			canvas.drawFastHLine(MARGIN_L, gy, chartWidth, TFT_DARKGRAY);
		}

		if (chartHeight < 20 || chartWidth < 20 || maxCount == 0) {
			// fallback: no data or too small
			if (maxCount == 0) {
				canvas.setTextSize(MEDIUM_TEXT);
				canvas.setTextColor(FGCOLOR, BGCOLOR);
				canvas.drawCenterString(L->TXT_WIFI_NO_NETWORKS, cw / 2, chH / 2);
			}
			canvas.pushSprite(0, getStatusBarHeight());
			return;
		}

		// measure tiny text height once
		canvas.setTextSize(TINY_TEXT);
		int tinyFh = canvas.fontHeight();

		// draw bars
		int gap = 8;
		int barW = (chartWidth - gap * (CH_ANALYZER_MAX_CH - 1)) / CH_ANALYZER_MAX_CH;
		if (barW < 3) { barW = 3; gap = (chartWidth - barW * CH_ANALYZER_MAX_CH) / (CH_ANALYZER_MAX_CH - 1); }
		if (gap < 1) gap = 1;

		int totalW = CH_ANALYZER_MAX_CH * barW + (CH_ANALYZER_MAX_CH - 1) * gap;
		int xOffset = MARGIN_L + (chartWidth - totalW) / 2; // center the group

		for (int i = 0; i < CH_ANALYZER_MAX_CH; i++) {
			int x = xOffset + i * (barW + gap);
			int barH;
			if (maxCount > 0) {
				barH = (counts[i] * chartHeight) / maxCount;
				if (barH < 1 && counts[i] > 0) barH = 1;
			} else {
				barH = 0;
			}
			int y = chartBottom - barH;

			// bar fill
			bool isBest = (i + 1 == bestChannel);
			uint16_t barColor = isBest ? TFT_GREEN : FGCOLOR;
			canvas.fillRect(x, y, barW, barH, barColor);

			// count number above bar
			if (counts[i] > 0) {
				canvas.setTextSize(TINY_TEXT);
				canvas.setTextColor(barColor, BGCOLOR);
				String nStr = String(counts[i]);
				int nW = canvas.textWidth(nStr.c_str());
				int nX = x + (barW - nW) / 2;
				int nY = y - tinyFh - 1;
				canvas.setCursor(nX, nY);
				canvas.print(nStr.c_str());
			}

			// channel number below
			canvas.setTextSize(TINY_TEXT);
			canvas.setTextColor(FGCOLOR, BGCOLOR);
			String chStr = String(i + 1);
			int cW = canvas.textWidth(chStr.c_str());
			canvas.setCursor(x + (barW - cW) / 2, chartBottom + 2);
			canvas.print(chStr.c_str());
		}

		canvas.pushSprite(0, getStatusBarHeight());
	}

	checkExit();
}
