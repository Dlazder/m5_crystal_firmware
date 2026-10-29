// PID::IR_CONFIG_RESET

void irConfigureResetLoop() {
	if (isSetup()) {
		String lines[] = {
			"Reset IR pins?",
			"TX: G" + String(irTxPin) + "  RX: G" + String(irRxPin),
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT, true);
		drawHintCustom("enter: reset", "A: reset");
		updateTimer();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && checkTimer(100)) {
		Settings::resetGroup(SettingGroup::IR_PINS);
		Serial.printf("IR pins reset: TX=%d RX=%d\n", irTxPin, irRxPin);
		
		String lines[] = {
			"IR pins reset",
			"TX: G" + String(irTxPin) + "  RX: G" + String(irRxPin),
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		soundSuccess();
	}

	checkExit();
}
