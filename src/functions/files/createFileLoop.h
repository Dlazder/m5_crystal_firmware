// PID::FILE_CREATE

void createFileLoop() {
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
			bool ok = false;

			bool useLittleFS = (fileOptionsSourcePid != PID::FILE_PICKER_SD);
			File f = Storage::open(path.c_str(), "w", useLittleFS);
			ok = (bool)f;
			if (f) f.close();

			const char* result = ok ? L->TXT_SUCCESS : L->TXT_ERROR;
			centeredPrint(result, MEDIUM_TEXT);
			delay(800);
			changeProcess(fileOptionsSourcePid);
		},
		nullptr
	);
}
