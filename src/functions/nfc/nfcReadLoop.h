// PID::NFC_READ

Nfc::Tag nfcReadTag;
bool nfcReadHasTag = false;

void _nfcReadDisplayNotConnectedError() {
	Nfc::setWasConnected(false);
	connectionGuideNFC();
	Serial.println("PN532: disconnected");
	DEVICE.Power.setLed(1);
	soundError();
}

void _nfcReadDrawWaiting() {
	String lines[] = {
		"PN532: connected",
		L->TXT_NFC_PLACE_TAG
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT);
}

void _nfcReadDrawTag() {
	const Nfc::Tag& t = nfcReadTag;
	String lines[4];
	int n = 0;

	switch (t.type) {
		case Nfc::TagType::MIFARE_CLASSIC:
			lines[n++] = String("Mifare Classic ") + t.subtype;
			break;
		case Nfc::TagType::ULTRALIGHT:
			lines[n++] = t.subtype;
			break;
		default:
			lines[n++] = "ISO14443-3A";
	}
	lines[n++] = "UID: " + Nfc::uidToString(t.uid, t.uidLen);
	if (t.type == Nfc::TagType::MIFARE_CLASSIC) {
		lines[n++] = String(L->TXT_NFC_SECTORS) + ": " + t.sectorsRead + "/" + t.sectorsTotal;
	} else if (t.type == Nfc::TagType::ULTRALIGHT) {
		lines[n++] = String(L->TXT_NFC_PAGES) + ": " + t.pagesRead + "/" + t.pagesTotal;
	}
	centeredPrintRows(lines, n, SMALL_TEXT);
	drawHintCustom("enter: save", "A: save");
}

void _nfcReadProgress(uint8_t percent) {
	String lines[] = {
		L->TXT_NFC_READING,
		String(percent) + "%"
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT);
}

void nfcReadLoop() {
	if (isSetup()) {
		nfcReadHasTag = false;
		nfcReadTag.release();
		connectionGuideNFC();

		if (Nfc::begin(NFC_SDA, NFC_SCL)) {
			Nfc::setWasConnected(true);
			_nfcReadDrawWaiting();
		} else {
			_nfcReadDisplayNotConnectedError();
		}
	}

	// A tag is on screen: A / Enter saves it, then scanning resumes
	if (nfcReadHasTag) {
		if (isBtnAWasPressed() || isKbEnterPressed()) {
			String path;
			bool useLittleFS = false;
			if (Nfc::save(nfcReadTag, path, useLittleFS)) {
				String lines[] = {
					useLittleFS ? L->TXT_SAVED_LFS : L->TXT_SAVED_SD,
					path
				};
				centeredPrintRows(lines, 2, SMALL_TEXT);
				soundSuccess();
			} else {
				centeredPrint(L->TXT_NFC_SAVE_ERROR, MEDIUM_TEXT);
				soundError();
			}
			delay(1200);
			nfcReadHasTag = false;
			nfcReadTag.release();
			_nfcReadDrawWaiting();
			updateTimer();
		}
	} else if (checkTimer(500)) {
		if (!Nfc::isConnected()) {
			if (Nfc::wasConnected()) _nfcReadDisplayNotConnectedError();
		} else {
			if (!Nfc::wasConnected() && Nfc::begin(NFC_SDA, NFC_SCL)) {
				Nfc::setWasConnected(true);
				DEVICE.Power.setLed(0);
				soundSuccess();
				_nfcReadDrawWaiting();
			}
			if (Nfc::wasConnected() && Nfc::readTag(nfcReadTag, 100, _nfcReadProgress)) {
				nfcReadHasTag = true;

				// Keep the write screen working: it writes a stored 4-byte UID.
				Nfc::setLastUID(nfcReadTag.uid, nfcReadTag.uidLen);

				_nfcReadDrawTag();
				soundBeep();
				Serial.printf("NFC: tag %s, %s\n", Nfc::uidToString(nfcReadTag.uid, nfcReadTag.uidLen).c_str(), nfcReadTag.subtype);
			}
		}
	}

	if (checkExit()) {
		DEVICE.Power.setLed(0);
		Nfc::setWasConnected(false);
		nfcReadHasTag = false;
		nfcReadTag.release();
	}
}
