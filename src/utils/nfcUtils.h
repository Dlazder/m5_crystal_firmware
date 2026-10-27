// Glue between the NFC settings and the NFC service: the NFC screens start the reader
// selected in NFC > module, with the pins set in NFC > configure pins, through these.

/// Starts the selected reader (PN532 or RC522). Returns true when it answers.
inline bool nfcBegin() {
	if (nfcModule == NFC_MODULE_RC522) {
		return Nfc::begin(Nfc::Rc522Pins{rc522SsPin, rc522SckPin, rc522MosiPin, rc522MisoPin, rc522RstPin});
	}
	return Nfc::begin(pn532SdaPin, pn532SclPin);
}

/// Wiring table of the selected reader.
inline void nfcConnectionGuide() {
	if (nfcModule == NFC_MODULE_RC522) connectionGuideRC522();
	else connectionGuideNFC();
}
