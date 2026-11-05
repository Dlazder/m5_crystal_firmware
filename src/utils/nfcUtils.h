// Glue between the NFC settings and the NFC service: the NFC screens start the reader
// selected in NFC > module, with the pins set in NFC > configure pins, through these.

/// Starts the selected reader (PN532 or RC522). Returns true when it answers.
inline bool nfcBegin() {
	if (nfcModule == NFC_MODULE_RC522) {
		return Nfc::begin(Nfc::Rc522Pins{Rc522Pins::ss, Rc522Pins::sck, Rc522Pins::mosi, Rc522Pins::miso, Rc522Pins::rst});
	}
	return Nfc::begin(Pn532Pins::sda, Pn532Pins::scl);
}

/// Wiring table of the selected reader.
inline void nfcConnectionGuide() {
	if (nfcModule == NFC_MODULE_RC522) connectionGuideRC522();
	else connectionGuideNFC();
}
