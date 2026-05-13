// PID::FILE_CREATE

static int createFileSourcePid = PID::FILE_PICKER;

void createFileLoop() {
	if (isSetup()) {
		if (previousProcess == PID::FILE_PICKER_SD)
			createFileSourcePid = PID::FILE_PICKER_SD;
		else
			createFileSourcePid = PID::FILE_PICKER;
		kbReset();
		drawKeyboardUi();
	}

	keyboardLoop(
		[]() {
			changeProcess(createFileSourcePid);
		},
		[](const char* buf) {
			if (kbLen == 0) {
				changeProcess(createFileSourcePid);
				return;
			}
			String path = "/" + String(buf);
			bool ok = false;

			if (createFileSourcePid == PID::FILE_PICKER_SD) {
				#if HAS_SD
					File f = SD.open(path.c_str(), FILE_WRITE);
					ok = (bool)f;
					if (f) f.close();
				#endif
			} else {
				File f = LittleFS.open(path.c_str(), "w");
				ok = (bool)f;
				if (f) f.close();
			}

			String result = ok ? "created" : "error";
			centeredPrint(result, MEDIUM_TEXT);
			delay(800);
			changeProcess(createFileSourcePid);
		},
		nullptr
	);
}
