// PID::NFC_READ

Nfc::Tag nfcReadTag;
bool nfcReadHasTag = false;

// The RC522 shares the SPI bus with the SD card
bool _nfcReadUsesRc522() { return Nfc::activeModule() == Nfc::Module::RC522; }

void _nfcReadDisplayNotConnectedError() {
	Nfc::setWasConnected(false);
	nfcConnectionGuide();
	Serial.printf("%s: disconnected\n", Nfc::moduleName());
	DEVICE.Power.setLed(1);
	soundError();
}

void _nfcReadDrawWaiting() {
	String lines[] = {
		String(Nfc::moduleName()) + ": connected",
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
			lines[n++] = t.subtype[0] ? t.subtype : "ISO14443-3A"; // the RC522 names the tag family by SAK
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
		nfcConnectionGuide();

		if (nfcBegin()) {
			Nfc::setWasConnected(true);
			_nfcReadDrawWaiting();
		} else {
			_nfcReadDisplayNotConnectedError();
		}
	}

	// A tag is on screen: A / Enter saves it, then scanning resumes
	if (nfcReadHasTag) {
		if (isBtnAWasPressed() || isKbEnterPressed()) {
			// The RC522 shares the SPI bus with the SD card: release it for the save, restart it after
			if (_nfcReadUsesRc522()) Nfc::end();
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
			if (_nfcReadUsesRc522() && !nfcBegin()) {
				_nfcReadDisplayNotConnectedError();
			} else {
				if (_nfcReadUsesRc522()) Nfc::setWasConnected(true);
				_nfcReadDrawWaiting();
			}
			updateTimer();
		}
	} else if (checkTimer(500)) {
		if (!Nfc::isConnected()) {
			if (Nfc::wasConnected()) _nfcReadDisplayNotConnectedError();
		} else {
			if (!Nfc::wasConnected() && nfcBegin()) {
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
		Nfc::end();
		nfcReadHasTag = false;
		nfcReadTag.release();
	}
}
