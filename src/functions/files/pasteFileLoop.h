// PID::FILE_PASTE

void pasteFileLoop() {
	if (isSetup()) {
		if (!clipboardHasFile) {
			centeredPrint(L->TXT_CLIPBOARD_EMPTY, MEDIUM_TEXT);
			delay(800);
			changeProcess(fileOptionsSourcePid);
			return;
		}

		// Destination backend + directory (set when entering FILE_OPTIONS).
		bool dstLittleFS = (fileOptionsSourcePid != PID::FILE_PICKER_SD);
		String dir = createFileCurrentDir;

		// Source filename (basename), split into stem + extension for _N renaming.
		String name = clipboardPath.substring(clipboardPath.lastIndexOf('/') + 1);
		int dot = name.lastIndexOf('.');
		String stem = (dot > 0) ? name.substring(0, dot) : name;
		String ext  = (dot > 0) ? name.substring(dot) : "";

		// Build a unique destination path, appending _1, _2, ... on collision.
		String dest = (dir == "/") ? "/" + name : dir + "/" + name;
		int n = 1;
		while (Storage::exists(dest.c_str(), dstLittleFS)) {
			dest = (dir == "/") ? "/" + stem + "_" + String(n) + ext
			                    : dir + "/" + stem + "_" + String(n) + ext;
			n++;
		}

		bool ok = Storage::copy(clipboardPath.c_str(), dest.c_str(), clipboardIsLittleFS, dstLittleFS);
		if (ok) {
			// Clear the clipboard on success so a subsequent paste doesn't
			// duplicate the file again.
			clipboardHasFile = false;
			clipboardPath = "";
		}
		const char* result = ok ? L->TXT_SUCCESS : L->TXT_ERROR;
		centeredPrint(result, MEDIUM_TEXT);
		delay(800);
		changeProcess(fileOptionsSourcePid);
		return;
	}

	checkExit();
}
