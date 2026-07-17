// PID::NFC_ADVANCED_INFO

uint8_t cardInfoUid[10] = {0};
uint8_t cardInfoUidLen = 0;
String cardInfoType = "";
String cardInfoUidStr = "";
String cardInfoSak = "";
String cardInfoAtqa = "";
String cardInfoSize = "";
String cardInfoMemDump = "";
bool cardInfoFound = false;

// Reuse uidToString from nfcReadLoop.h (defined there globally).

String bytesToHex(const uint8_t *data, uint8_t len) {
	String s = "";
	for (uint8_t i = 0; i < len; i++) {
		if (data[i] < 0x10) s += "0";
		s += String(data[i], HEX);
		if (i < len - 1) s += " ";
	}
	s.toUpperCase();
	return s;
}

String mifareSizeString(uint8_t sak) {
	switch (sak) {
		case 0x08: return "1KB (16 sectors)";
		case 0x09: return "320B (5 sectors)";
		case 0x18: return "4KB (40 sectors)";
		default:   return "";
	}
}

String ntagSizeString(uint8_t ccByte2) {
	// Capability Container byte 2 encodes memory size for NTAG
	switch (ccByte2) {
		case 0x0E: return "NTAG213 (180B)";
		case 0x11: return "NTAG215 (540B)";
		case 0x13: return "NTAG216 (924B)";
		case 0x03: return "Ultralight (64B)";
		case 0x05: return "Ultralight C (192B)";
		default:   return "";
	}
}

void determineCardType() {
	cardInfoType = "Unknown";
	cardInfoSak = "n/a";
	cardInfoAtqa = "n/a";
	cardInfoSize = "";
	cardInfoMemDump = "";

	if (cardInfoUidLen == 0) return;

	uint8_t defaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	// Try MIFARE Classic: authenticate + read block 0
	uint8_t block0[16];
	if (nfc.mifareclassic_AuthenticateBlock(cardInfoUid, cardInfoUidLen, 0, 0, defaultKey)) {
		if (nfc.mifareclassic_ReadDataBlock(0, block0)) {
			uint8_t sak = block0[5];
			uint16_t atqa = block0[6] | (block0[7] << 8);

			char sakStr[8];
			sprintf(sakStr, "0x%02X", sak);
			cardInfoSak = String(sakStr);

			char atqaStr[10];
			sprintf(atqaStr, "0x%04X", atqa);
			cardInfoAtqa = String(atqaStr);

			switch (sak) {
				case 0x08: cardInfoType = "MIFARE Classic 1K"; break;
				case 0x09: cardInfoType = "MIFARE Classic Mini"; break;
				case 0x18: cardInfoType = "MIFARE Classic 4K"; break;
				default:   cardInfoType = "MIFARE Classic";
			}
			cardInfoSize = mifareSizeString(sak);

			// Show first 8 bytes of block 0 as memory preview
			cardInfoMemDump = "Blk0: " + bytesToHex(block0, 8);
			return;
		}
	}

	// Not MIFARE Classic — try Ultralight / NTAG
	uint8_t page0[4], page3[4];
	if (nfc.mifareultralight_ReadPage(0, page0)) {
		cardInfoType = "MIFARE Ultralight/NTAG";
		cardInfoSak = "0x00";
		nfc.mifareultralight_ReadPage(3, page3);

		// Page 3 bytes 2-3 = Capability Container (CC)
		uint8_t ccByte2 = page3[2];
		String ntagStr = ntagSizeString(ccByte2);
		if (ntagStr.length() > 0) {
			cardInfoSize = ntagStr;
			cardInfoType = ntagStr.substring(0, ntagStr.indexOf('('));
		}

		// ATQA from CC convention
		uint16_t atqa = page3[2] | ((uint16_t)page3[3] << 8);
		char atqaStr[10];
		sprintf(atqaStr, "0x%04X", atqa);
		cardInfoAtqa = String(atqaStr);

		cardInfoMemDump = "Pg0: " + bytesToHex(page0, 4);
		return;
	}

	// Fallback: just UID info
	if (cardInfoUidLen == 4) {
		cardInfoType = "ISO 14443A (4B UID)";
	} else if (cardInfoUidLen == 7) {
		cardInfoType = "ISO 14443A (7B UID)";
	}
	cardInfoSak = "n/a";
	cardInfoAtqa = "n/a";
}


void nfcAdvancedInfoLoop() {
	if (isSetup()) {
		String lines[] = {
			"PN532: disconnected",
			L->TXT_CONNECTING,
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);

		Wire.end();
		Wire.begin(NFC_SDA, NFC_SCL);
		if (isPN532Connected()) {
			nfc.begin();
			uint32_t versiondata = nfc.getFirmwareVersion();
			if (versiondata) {
				String lines[] = {
					"PN532: connected",
					"Advanced Card Info",
					"Place card on reader"
				};
				centeredPrintRows(lines, 3, SMALL_TEXT);
				Serial.println("PN532: connected (advanced info mode)");
			} else {
				displayNotConnectedError();
			}
		} else {
			displayNotConnectedError();
		}
	}

	if (checkTimer(2000)) {
		if (!isPN532Connected()) {
			if (nfcModuleWasConnected) displayNotConnectedError();
			return;
		}

		nfc.begin();
		uint32_t versionData = nfc.getFirmwareVersion();

		if (!versionData) {
			if (nfcModuleWasConnected) displayNotConnectedError();
			return;
		} else {
			if (!nfcModuleWasConnected) {
				nfcModuleWasConnected = true;
				DEVICE.Power.setLed(0);
				soundSuccess();
				String lines[] = {
					"PN532: connected",
					"Advanced Card Info",
					"Place card on reader"
				};
				centeredPrintRows(lines, 3, SMALL_TEXT);
				Serial.println("PN532: connected (advanced info mode)");
			}
		}

		// Scan for card
		uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		uint8_t uidLength;
		bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

		if (success) {
			updateTimer();

			// Store UID info
			memset(cardInfoUid, 0, sizeof(cardInfoUid));
			memcpy(cardInfoUid, uid, uidLength);
			cardInfoUidLen = uidLength;
			cardInfoUidStr = uidToString(uid, uidLength);

			Serial.print("Card detected \x97 UID: ");
			Serial.println(cardInfoUidStr);

			// Probe card type and read data
			determineCardType();

			Serial.print("Type: ");
			Serial.println(cardInfoType);
			Serial.print("SAK: ");
			Serial.println(cardInfoSak);
			Serial.print("ATQA: ");
			Serial.println(cardInfoAtqa);
			if (cardInfoMemDump.length() > 0) {
				Serial.print("Data: ");
				Serial.println(cardInfoMemDump);
			}

			cardInfoFound = true;

			// Build display lines
			String lines[5];
			int lineCount = 0;
			lines[lineCount++] = cardInfoType;
			lines[lineCount++] = "UID: " + cardInfoUidStr;
			lines[lineCount++] = "SAK: " + cardInfoSak + "  ATQA: " + cardInfoAtqa;
			if (cardInfoSize.length() > 0) {
				lines[lineCount++] = "Size: " + cardInfoSize;
			}
			if (cardInfoMemDump.length() > 0) {
				lines[lineCount++] = cardInfoMemDump;
			}

			centeredPrintRows(lines, lineCount, TINY_TEXT);

			soundBeep();
		}
		// Nothing else — card info stays on screen after card is removed.
	}

	if (checkExit()) {
		DEVICE.Power.setLed(0);
		nfcModuleWasConnected = false;
		cardInfoFound = false;
		memset(cardInfoUid, 0, sizeof(cardInfoUid));
		cardInfoUidLen = 0;
	};
}
