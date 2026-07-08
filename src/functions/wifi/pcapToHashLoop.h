// pid PID::WIFI_PCAP_TO_HASH

// Select a .pcap file (SD card), extracts EAPOL handshake, and writes
// a .hash file for FT-Crack / hashcat mode 22000.

void pcapToHashLoop() {
	static bool converted = false;
	static bool success = false;

	if (isSetup()) {
		converted = false;
		success = false;
		fpActive = false;
		selectedFilePath = "";
		filePickerSetup(PID::WIFI);
	}

	// --- File picker phase ---
	if (fpActive) {
		if (filePickerLoop()) return;
		if (selectedFilePath == "") return; // user cancelled, changeProcess already called

		// Only process .pcap files from SD
		if (!fpSelectedSd || !selectedFilePath.endsWith(".pcap")) {
			centeredPrint("Select a .pcap file\nfrom SD card", SMALL_TEXT);
			soundError();
			selectedFilePath = "";
			filePickerSetup(PID::WIFI);
			return;
		}

		canvas.clear();
		centeredPrint("Processing...", MEDIUM_TEXT);
		canvas.pushSprite(0, getStatusBarHeight());

		success = pcapToFTHash(selectedFilePath);
		converted = true;
		return;
	}

	// --- Result display phase ---
	if (converted) {
		if (success) {
			String hashPath = selectedFilePath;
			hashPath.replace(".pcap", ".hash");
			String lines[] = {
				"Hash file saved:",
				hashPath
			};
			centeredPrintRows(lines, 2, SMALL_TEXT);
			soundSuccess();
		} else {
			String lines[] = {
				"No handshake found",
				"in " + selectedFilePath
			};
			centeredPrintRows(lines, 2, SMALL_TEXT);
			soundError();
		}

		delay(1000);
		converted = false;
		selectedFilePath = "";
		fpActive = false;
		changeProcess(previousProcess);

		// if (checkExit()) {
		// 	converted = false;
		// 	selectedFilePath = "";
		// 	fpActive = false;
		// }
	}
}
