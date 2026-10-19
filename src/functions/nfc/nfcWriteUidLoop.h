// PID::NFC_WRITE_UID

void _nfcWriteUidDisplayNotConnectedError() {
	Nfc::setWasConnected(false);
	connectionGuideNFC();
	Serial.println("PN532: disconnected");
	DEVICE.Power.setLed(1);
	soundError();
}

Nfc::Tag nfcWriteUidTag;
bool nfcWriteUidFileLoaded = false;

void _nfcWriteUidDrawLoaded() {
	String lines[] = {
		"UID: " + Nfc::uidToString(nfcWriteUidTag.uid, nfcWriteUidTag.uidLen),
		L->TXT_NFC_PLACE_TAG
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT);
	drawHintCustom("enter: write", "A: write");
}

void nfcWriteUidLoop() {
	if (isSetup()) {
		nfcWriteUidFileLoaded = false;
		selectedFilePath = "";
		filePickerSetup(PID::NFC_WRITE);
		updateTimer();
	}

	if (fpActive) {
		if (filePickerLoop()) return;
		if (selectedFilePath == "") return;

		File f = Storage::open(selectedFilePath.c_str(), "r", !fpSelectedSd);
		bool parsed = f && Nfc::parseFlipperFile(f, nfcWriteUidTag);
		if (f) f.close();

		if (!parsed || nfcWriteUidTag.uidLen != 4) {
			centeredPrint(L->TXT_NFC_INVALID_UID, MEDIUM_TEXT);
			soundError();
			delay(800);
			selectedFilePath = "";
			filePickerSetup(PID::NFC_WRITE);
			return;
		}

		if (!Nfc::begin(NFC_SDA, NFC_SCL)) {
			_nfcWriteUidDisplayNotConnectedError();
			return;
		}
		Nfc::setWasConnected(true);
		nfcWriteUidFileLoaded = true;
		_nfcWriteUidDrawLoaded();
		return;
	}

	if (!nfcWriteUidFileLoaded) return;

	if (checkTimer(500) && !Nfc::isConnected()) {
		if (Nfc::wasConnected()) _nfcWriteUidDisplayNotConnectedError();
		return;
	}

	if (isBtnAWasPressed() || isKbEnterPressed()) {
		bool ok = Nfc::writeUid(nfcWriteUidTag);
		String lines[] = {
			ok ? L->TXT_NFC_WRITE_SUCCESS : L->TXT_NFC_WRITE_FAILED,
			ok ? L->TXT_NFC_UID_WRITTEN : L->TXT_NFC_WRITE_ERROR
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		if (ok) soundSuccess(); else soundError();
		delay(1200);
		_nfcWriteUidDrawLoaded();
		updateTimer();
	}

	if (checkExit()) {
		DEVICE.Power.setLed(0);
		Nfc::setWasConnected(false);
		nfcWriteUidFileLoaded = false;
		nfcWriteUidTag.release();
	}
}
