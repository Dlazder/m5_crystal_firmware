// pid PID::DIMMING

const int dimTimeouts[] = { 0, 30000, 60000, 180000, 300000 };
const int dimTimeoutsCount = sizeof(dimTimeouts) / sizeof(dimTimeouts[0]);

int dimTimeoutIndex = defaultDimTimeoutIndex;

String _getDimLabel() {
	switch (dimTimeouts[dimTimeoutIndex]) {
		case 0: return L->TXT_DISABLED;
		case 30000: return "30s";
		case 60000: return "1m";
		case 180000: return "3m";
		case 300000: return "5m";
		default: return L->TXT_NA;
	}
}

void dimmingLoop() {
	if (isSetup()) {
		String lines[] = {
			String(L->MENU_SETTINGS_DIMMING) + String(": ") + _getDimLabel(),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		dimTimeoutIndex = (dimTimeoutIndex + 1) % dimTimeoutsCount;
		screenDimTimeout = dimTimeouts[dimTimeoutIndex];
		setData("dimTimeout", dimTimeoutIndex);

		if (screenIsDimmed) {
			DISP.setBrightness(brightnessToHw(brightness));
			screenIsDimmed = false;
		}
		updateTimer(&dimmingPreviousTimer);

		String lines[] = {
			String(L->MENU_SETTINGS_DIMMING) + String(": ") + _getDimLabel(),
			L->TXT_PRESS_A
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
	}

	checkExit();
}
