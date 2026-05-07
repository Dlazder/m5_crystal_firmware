// pid 23

// Modify this array with your target UID
uint8_t DEFAULT_KEY[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint8_t calculateBCC(uint8_t *uid, uint8_t len) {
	uint8_t bcc = 0;
	for (uint8_t i = 0; i < len; i++) {
		bcc ^= uid[i];
	}
	return bcc;
}

void prepareBlock0(uint8_t *newUID, uint8_t *newBlock0, uint8_t uidLength) {
	// Copy UID
	memcpy(newBlock0, newUID, uidLength);

	// If UID 4 bytes - adding BCC
	if (uidLength == 4) {
		newBlock0[4] = calculateBCC(newUID, 4);
		newBlock0[5] = 0x08;  // SAK for MIFARE Classic 1K
		newBlock0[6] = 0x04;  // ATQA least significant byte
		newBlock0[7] = 0x00;  // ATQA most significant byte
	}
	// For 7-byte UID (MIFARE Ultralight) structure is different
	// But for CUID usually 4 bytes are used

	// Other bytes filled with zeros (or copied from the original tag)
	for (int i = uidLength + 1; i < 16; i++) {
		newBlock0[i] = 0x00;
	}
}

bool isMifareClassic(uint8_t *uid, uint8_t uidLength) {
	// MIFARE Classic usually have 4 bytes UID
	return (uidLength == 4);
}

bool writeNewUID(uint8_t *newUID, uint8_t newUIDLength) {
	if (!hasValidUID) {
		Serial.println("No UID stored for writing");
		return false;
	}
	uint8_t uid[10];
	uint8_t uidLength;

	// Tag search
	uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500);
	if (!success) {
		Serial.println("No tag found for writing");
		return false;
	}

	Serial.print("Found tag with UID: ");
	for (int i = 0; i < uidLength; i++) {
		Serial.print(uid[i], HEX);
		Serial.print(" ");
	}
	Serial.println();

	// Checking, if the operation is supported
	if (!isMifareClassic(uid, uidLength)) {
		Serial.println("Tag is not MIFARE Classic (CUID required)");
		return false;
	}

	// Authentication for block 0
	Serial.println("Authenticating for block 0...");
	success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 0, 0, DEFAULT_KEY);
	if (!success) {
		Serial.println("Authentication failed! Make sure this is a CUID tag");
		return false;
	}

	// Preparing new data for block 0
	uint8_t newBlock0[16];
	prepareBlock0(newUID, newBlock0, newUIDLength);

	Serial.print("Writing new block 0: ");
	for (int i = 0; i < 16; i++) {
		Serial.print(newBlock0[i], HEX);
		Serial.print(" ");
	}
	Serial.println();

	// Writing the new block 0
	success = nfc.mifareclassic_WriteDataBlock(0, newBlock0);
	if (!success) {
		Serial.println("Write failed! Block 0 write error");
		return false;
	}

	Serial.println("Block 0 written successfully");

	// Small delay for completion of writing
	delay(200);

	// Verification: re-read the tag
	success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500);
	if (success) {
		Serial.print("Verification - New UID: ");
		for (int i = 0; i < uidLength; i++) {
				Serial.print(uid[i], HEX);
				Serial.print(" ");
		}
		Serial.println();

		// Checking if the new UID matches the expected one
		bool uidMatches = true;
		for (int i = 0; i < newUIDLength && i < uidLength; i++) {
			if (uid[i] != newUID[i]) {
				uidMatches = false;
				break;
			}
		}

		if (uidMatches) {
			Serial.println("UID verification successful!");
			return true;
		} else {
			Serial.println("UID verification failed!");
			return false;
		}
	}

	return false;
}

void nfcWriteLoop() {
	if (isSetup()) {
		String lines[] = {
			"PN532: disconnected",
			L->TXT_CONNECTING,
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);

		// Check the stored UID
		if (!hasValidUID) {
			Serial.println("No UID stored for writing");
			String lines[] = {
				L->TXT_NFC_NO_UID_STORED,
				L->TXT_NFC_READ_TAG_FIRST
			};
			centeredPrintRows(lines, 2, MEDIUM_TEXT);
			return;
		}

		if (lastReadUIDLength != 4) {
			Serial.println("Stored UID is not a valid CUID (4 bytes required)");
			String lines[] = {
				L->TXT_NFC_INVALID_UID,
				L->TXT_NFC_UID_4BYTE_REQUIRED
			};
			centeredPrintRows(lines, 2, MEDIUM_TEXT);
			return;
		}

		if (isPN532Connected()) {
			nfc.begin();
			uint32_t versiondata = nfc.getFirmwareVersion();
			if (versiondata) {
				String lines[] = {
					"PN532: connected",
					L->TXT_NFC_READY_TO_WRITE
				};
				centeredPrintRows(lines, 2, MEDIUM_TEXT);

				Serial.println("PN532: connected");
				Serial.printf("PN532 firmware version: %lu\n", versiondata);
				Serial.println("Waiting for tag to write...");

			} else displayNotConnectedError();
		} else displayNotConnectedError();
	}

	if (checkTimer(2000) && hasValidUID && lastReadUIDLength == 4) {
		// Module connection check
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

				String uidString = uidToString(lastReadUID, lastReadUIDLength);
				String lines[] = {
					"PN532: connected",
					"UID: " + uidString,
					L->TXT_NFC_READY_TO_WRITE
				};
				centeredPrintRows(lines, 3, MEDIUM_TEXT);
				Serial.println("PN532: connected");
			}
		}

		// Scanning and writing
		uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
		uint8_t uidLength;

		uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

		if (success) {
			char uidStr[32];
			sprintf(uidStr, "Found UID: %02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3]);
			Serial.println("Tag detected, attempting to write...");

			bool writeSuccess = writeNewUID(lastReadUID, lastReadUIDLength);

			if (writeSuccess) {
				clearScreenWithSymbols();
				String lines[] = {
					L->TXT_NFC_WRITE_SUCCESS,
					L->TXT_NFC_UID_WRITTEN
				};
				centeredPrintRows(lines, 2, MEDIUM_TEXT);

				Serial.println("Write successful!");
				DEVICE.Speaker.tone(2000, 200);
			} else {
				clearScreenWithSymbols();
				String lines[] = {
					L->TXT_NFC_WRITE_FAILED,
					L->TXT_NFC_WRITE_ERROR
				};
				centeredPrintRows(lines, 2, MEDIUM_TEXT);
				Serial.println("Write failed!");
				DEVICE.Speaker.tone(1000, 200);
			}

			updateTimer();
		}
	}

	if (checkExit()) {
		DEVICE.Power.setLed(0);
		nfcModuleWasConnected = false;
	};
}
