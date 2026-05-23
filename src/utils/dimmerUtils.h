void dimmerUpdate() {
	bool anyInput = isBtnAWasPressed() || isBtnBWasPressed() || isBtnPWRWasPressed() || isAnyKbKeyPressed();
	if (anyInput) {
		if (screenIsDimmed) {
			DISP.setBrightness(brightnessToHw(brightness));
			screenIsDimmed = false;
		}
		updateTimer(&dimmingPreviousTimer);
	}

	if (screenDimTimeout > 0 && !screenIsDimmed) {
		if (checkTimer(screenDimTimeout, false, &dimmingPreviousTimer)) {
			DISP.setBrightness(10);
			screenIsDimmed = true;
		}
	}
}
