// PID::FILE_RENAME_DIR

void renameDirLoop() {
	if (isSetup()) {
		kbReset();
		drawKeyboardUi();
	}

	keyboardLoop(
		[]() {
			changeProcess(PID::FILE_OPTIONS);
		},
		[](const char* buf) {
			if (kbLen == 0) {
				changeProcess(PID::FILE_OPTIONS);
				return;
			}

			String parent = createFileCurrentDir.substring(0, createFileCurrentDir.lastIndexOf('/') + 1);
			String newPath = parent + String(buf);

			bool useLittleFS = (fileOptionsSourcePid != PID::FILE_PICKER_SD);
			bool ok = Storage::rename(createFileCurrentDir.c_str(), newPath.c_str(), useLittleFS);

			if (ok) createFileCurrentDir = newPath;

			const char* result = ok ? L->TXT_SUCCESS : L->TXT_ERROR;
			centeredPrint(result, MEDIUM_TEXT);
			delay(800);
			changeProcess(fileOptionsSourcePid);
		},
		nullptr
	);
}
