// PID::NFC_WRITE_SECTORS

void _nfcWriteSectorsDisplayNotConnectedError() {
	Nfc::setWasConnected(false);
	connectionGuideNFC();
	Serial.println("PN532: disconnected");
	DEVICE.Power.setLed(1);
	soundError();
}

Nfc::Tag nfcWriteSectorsTag;
bool nfcWriteSectorsFileLoaded = false;
bool nfcWriteSectorsBusy = false;

void _nfcWriteSectorsDrawLoaded() {
	String lines[] = {
		String("Mifare Classic ") + nfcWriteSectorsTag.subtype,
		L->TXT_NFC_PLACE_TAG
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT);
	drawHintCustom("enter: write", "A: write");
}

void _nfcWriteSectorsProgress(uint8_t percent) {
	String lines[] = {
		L->TXT_NFC_WRITING,
		String(percent) + "%"
	};
	centeredPrintRows(lines, 2, MEDIUM_TEXT);
}

void nfcWriteSectorsLoop() {
	if (isSetup()) {
		nfcWriteSectorsFileLoaded = false;
		nfcWriteSectorsBusy = false;
		selectedFilePath = "";
		filePickerSetup(PID::NFC_WRITE);
		updateTimer();
	}

	if (fpActive) {
		if (filePickerLoop()) return;
		if (selectedFilePath == "") return;

		File f = Storage::open(selectedFilePath.c_str(), "r", !fpSelectedSd);
		bool parsed = f && Nfc::parseFlipperFile(f, nfcWriteSectorsTag);
		if (f) f.close();

		if (!parsed || nfcWriteSectorsTag.type != Nfc::TagType::MIFARE_CLASSIC) {
			centeredPrint(L->TXT_NFC_NOT_CLASSIC, MEDIUM_TEXT);
			soundError();
			delay(800);
			selectedFilePath = "";
			filePickerSetup(PID::NFC_WRITE);
			return;
		}

		if (!Nfc::begin(NFC_SDA, NFC_SCL)) {
			_nfcWriteSectorsDisplayNotConnectedError();
			return;
		}
		Nfc::setWasConnected(true);
		nfcWriteSectorsFileLoaded = true;
		_nfcWriteSectorsDrawLoaded();
		return;
	}

	if (!nfcWriteSectorsFileLoaded) return;

	if (!nfcWriteSectorsBusy && checkTimer(500) && !Nfc::isConnected()) {
		if (Nfc::wasConnected()) _nfcWriteSectorsDisplayNotConnectedError();
		return;
	}

	if (!nfcWriteSectorsBusy && (isBtnAWasPressed() || isKbEnterPressed())) {
		nfcWriteSectorsBusy = true;
		bool ok = Nfc::writeClassicSectors(nfcWriteSectorsTag, _nfcWriteSectorsProgress);
		String lines[] = {
			ok ? L->TXT_NFC_WRITE_SUCCESS : L->TXT_NFC_WRITE_FAILED,
			ok ? L->TXT_NFC_SECTORS_WRITTEN : L->TXT_NFC_WRITE_ERROR
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		if (ok) soundSuccess(); else soundError();
		delay(1200);
		nfcWriteSectorsBusy = false;
		_nfcWriteSectorsDrawLoaded();
		updateTimer();
	}

	if (checkExit()) {
		DEVICE.Power.setLed(0);
		Nfc::setWasConnected(false);
		nfcWriteSectorsFileLoaded = false;
		nfcWriteSectorsTag.release();
	}
}
