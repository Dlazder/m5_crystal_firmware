// PID::FILE_DELETE

void deleteFileLoop() {
	if (isSetup()) {
		String name = selectedFilePath.substring(selectedFilePath.lastIndexOf('/') + 1);
		String lines[] = { "delete file?", name};
		centeredPrintRows(lines, 2, SMALL_TEXT, true);
		drawHintCustom("enter: delete", "A: delete");
	}

	if (isBtnAWasPressed() || isKbEnterPressed()) {
		bool ok = false;
		if (selectedFileSourcePid == PID::FILE_PICKER_SD) {
			#if HAS_SD
				ok = SD.remove(selectedFilePath.c_str());
			#endif
		} else {
			ok = LittleFS.remove(selectedFilePath.c_str());
		}

		const char* result = ok ? L->TXT_SUCCESS : L->TXT_ERROR;
		centeredPrint(result, MEDIUM_TEXT);
		delay(800);
		selectedFilePath = "";
		changeProcess(selectedFileSourcePid);
		return;
	}

	checkExit();
}
