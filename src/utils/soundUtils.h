void soundStartup() {
	if (startupSound) {
		DEVICE.Speaker.tone(1500, 200);
		delay(200);
		DEVICE.Speaker.tone(2000, 200);
		delay(200);
		DEVICE.Speaker.tone(2500, 200);
		delay(200);
	}
}

void soundConnected() {
	DEVICE.Speaker.tone(2000, 100);
	delay(50);
	DEVICE.Speaker.tone(2500, 100);
}

void soundDisconnected() {
	DEVICE.Speaker.tone(2500, 100);
	delay(50);
	DEVICE.Speaker.tone(2000, 100);
}

void soundSuccess() {
	DEVICE.Speaker.tone(2000, 100);
	delay(50);
	DEVICE.Speaker.tone(2500, 100);
}

void soundError() {
	DEVICE.Speaker.tone(1000, 200);
	delay(50);
	DEVICE.Speaker.tone(700, 300);
}

void soundBeep() {
	DEVICE.Speaker.tone(2000, 80);
}
