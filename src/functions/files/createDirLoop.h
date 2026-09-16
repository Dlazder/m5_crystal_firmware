// PID::FILE_CREATE_DIR

void createDirLoop() {
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
			String path = (createFileCurrentDir == "/")
				? "/" + String(buf)
				: createFileCurrentDir + "/" + String(buf);

			bool useLittleFS = (fileOptionsSourcePid != PID::FILE_PICKER_SD);
			bool ok = Storage::mkdir(path.c_str(), useLittleFS);

			const char* result = ok ? L->TXT_SUCCESS : L->TXT_ERROR;
			centeredPrint(result, MEDIUM_TEXT);
			delay(800);
			changeProcess(fileOptionsSourcePid);
		},
		nullptr
	);
}
