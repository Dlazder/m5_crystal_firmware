// pid 22

uint8_t lastReadUID[7] = {0};  // 7 bytes - enough for all standard tags
uint8_t lastReadUIDLength = 0;
bool hasValidUID = false;

// Flag to avoid repeated connect/disconnect notifications
bool nfcModuleWasConnected = false;

bool isPN532Connected() {
	Wire.beginTransmission(0x24);
	return Wire.endTransmission() == 0;
}

void displayNotConnectedError() {
	DISP.clear();
	nfcModuleWasConnected = false;
	String lines[] = {
		"PN532: disconnected"
	};
	centeredPrintRows(lines, 1, SMALL_TEXT);
	Serial.println("PN532: disconnected");
	DEVICE.Power.setLed(1);
	DEVICE.Speaker.tone(1000, 200);
}

String uidToString(const uint8_t* uid, uint8_t uidLength) {
	String uidString = "";
	for (uint8_t i = 0; i < uidLength; i++) {
		if (uid[i] < 0x10) uidString += "0";
		uidString += String(uid[i], HEX);
		if (i < uidLength - 1) uidString += ":";
	}
	return uidString;
}


void nfcReadLoop() {
	if (isSetup()) {
		String lines[] = {
			"PN532: disconnected",
			L->TXT_CONNECTING,
		};
		centeredPrintRows(lines, 2, SMALL_TEXT);

		if (isPN532Connected()) {
			nfc.begin();
			uint32_t versiondata = nfc.getFirmwareVersion();
			if (versiondata) {
				String lines[] = {
					"PN532: connected",
					L->TXT_SCANNING
				};
				centeredPrintRows(lines, 2, SMALL_TEXT);

				Serial.println("PN532: connected");
				Serial.printf("PN532 firmware version: %lu\n", versiondata);

			} else displayNotConnectedError();
		} else displayNotConnectedError();
	}

	if (checkTimer(3000)) {
		// first module connection check
		if (!isPN532Connected()) {
			if (nfcModuleWasConnected) displayNotConnectedError();
			return;
		}

		nfc.begin();
		uint32_t versionData = nfc.getFirmwareVersion();

		// Update module status
		if (!versionData) {
			if (nfcModuleWasConnected) displayNotConnectedError();
			return;
		} else {
			if (!nfcModuleWasConnected) {
				nfcModuleWasConnected = true;
				DEVICE.Power.setLed(0);
				DEVICE.Speaker.tone(2000, 200);

				clearScreenWithSymbols();
				String lines[] = {
					"PN532: connected",
					L->TXT_SCANNING
				};
				centeredPrintRows(lines, 2, SMALL_TEXT);
				Serial.println("PN532: connected");
			}
		}

		// Scanning
		uint8_t success;
		uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
		uint8_t uidLength;

		Serial.println("Scanning...");
		success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

		if (success) {
			updateTimer();
			String uidString = uidToString(uid, uidLength);

			Serial.printf("Tag found: %s\n", uidString.c_str());
			centeredPrint(uidString.c_str(), SMALL_TEXT);
			DEVICE.Speaker.tone(2000, 100);

			// Saving UID for writing
			memset(lastReadUID, 0, sizeof(lastReadUID));
			memcpy(lastReadUID, uid, uidLength);
			lastReadUIDLength = uidLength;
			hasValidUID = true;
			Serial.println("UID stored for writing:" + uidString);
		}
	}

	if (checkExit()) {
		DEVICE.Power.setLed(0);
		nfcModuleWasConnected = false;
	};
}
