// PID::VOLUME

void _volumeDraw() {
	static const int barHeight = 20;
	int visibleHeight = canvas.height();
	int centerY = visibleHeight / 2 - barHeight / 2;
	canvas.clear();
	drawProgressBar(volume / volumeDividor, volumeStep, centerY);
	drawHintRange();
}

void volumeLoop() {
	if (isSetup()) {
		_volumeDraw();
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbRightPressed()) && checkTimer(100)) {
		volume = ((volume / volumeDividor) % volumeStep + 1) * volumeDividor;
		DEVICE.Speaker.setVolume(volume);
		soundBeep();
		setData("volume", volume);
		Serial.printf("Volume: %d\n", volume / volumeDividor);
		_volumeDraw();
	}

	if ((isBtnPWRWasPressed() || isKbLeftPressed()) && checkTimer(100)) {
		int step = volume / volumeDividor;
		volume = (step <= 1 ? volumeStep : step - 1) * volumeDividor;
		DEVICE.Speaker.setVolume(volume);
		soundBeep();
		setData("volume", volume);
		Serial.printf("Volume: %d\n", volume / volumeDividor);
		_volumeDraw();
	}

	checkExit();

	if (isWebDataRequested()) {
		char text[50];
		sprintf(text, "volume: %d", volume / volumeDividor);
		webData = generateWebData("function", generateFunctionElement(text, MEDIUM_TEXT, "center"));
	}
}
