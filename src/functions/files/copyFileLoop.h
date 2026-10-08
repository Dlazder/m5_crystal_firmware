// PID::FILE_COPY

void copyFileLoop() {
	if (isSetup()) {
		// Stage the file into the clipboard (no actual I/O yet).
		clipboardHasFile = true;
		clipboardPath = selectedFilePath;
		clipboardIsLittleFS = (selectedFileSourcePid != PID::FILE_PICKER_SD);

		centeredPrint(L->TXT_SUCCESS, MEDIUM_TEXT);
		delay(800);
		changeProcess(PID::SELECTED_FILE_MENU);
		return;
	}

	checkExit();
}
