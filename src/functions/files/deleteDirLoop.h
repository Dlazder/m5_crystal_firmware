// PID::FILE_DELETE_DIR

void deleteDirLoop() {
	static bool confirmed = false;

	if (isSetup()) {
		confirmed = false;
		String lines[] = { L->TXT_DELETE_DIR, createFileCurrentDir };
		centeredPrintRows(lines, 2, SMALL_TEXT, true);
		drawHintCustom("enter: delete", "A: continue");
	}

	if (isBtnAWasPressed() || isKbEnterPressed()) {
		if (!confirmed) {
			confirmed = true;
			String lines[] = { L->TXT_DELETE_RECURSIVE, L->TXT_DELETE_RECURSIVE_HINT };
			centeredPrintRows(lines, 2, SMALL_TEXT, true);
			drawHintCustom("enter: delete", "A: confirm");
			return;
		}

		bool ok = false;
		if (createFileCurrentDir != "/") {
			bool useLittleFS = (fileOptionsSourcePid != PID::FILE_PICKER_SD);
			ok = Storage::removeRecursive(createFileCurrentDir.c_str(), useLittleFS);
		}

		const char* result = ok ? L->TXT_SUCCESS : L->TXT_ERROR;
		centeredPrint(result, MEDIUM_TEXT);
		delay(800);
		changeProcess(fileOptionsSourcePid);
		return;
	}

	checkExit();
}
