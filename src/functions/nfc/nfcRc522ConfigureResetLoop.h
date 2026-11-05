// PID::NFC_RC522_RESET

void nfcRc522ConfigureResetLoop() {
	if (isSetup()) {
		String lines[] = {
			"Reset RC522 pins?",
			"SS" + String(Rc522Pins::ss) + " SCK" + String(Rc522Pins::sck) + " MOSI" + String(Rc522Pins::mosi),
			"MISO" + String(Rc522Pins::miso) + " RST" + String(Rc522Pins::rst),
		};
		centeredPrintRows(lines, 3, SMALL_TEXT, true);
		drawHintCustom("enter: reset", "A: reset");
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		Settings::resetGroup(SettingGroup::RC522_PINS);

		String lines[] = {
			"RC522 pins reset",
			"SS" + String(Rc522Pins::ss) + " SCK" + String(Rc522Pins::sck) + " MOSI" + String(Rc522Pins::mosi),
			"MISO" + String(Rc522Pins::miso) + " RST" + String(Rc522Pins::rst),
		};
		Serial.printf("RC522 pins reset: %s %s\n", lines[1].c_str(), lines[2].c_str());
		centeredPrintRows(lines, 3, SMALL_TEXT);
		soundSuccess();
	}

	checkExit();
}
