// PID::FILE_FOLDER_INFO

void folderInfoLoop() {
	if (isSetup()) {
		bool useLittleFS = (fileOptionsSourcePid != PID::FILE_PICKER_SD);

		String* names = nullptr;
		bool* isDir = nullptr;
		int count = Storage::list(createFileCurrentDir, useLittleFS, names, isDir);
		int fileCount = 0;
		int dirCount = 0;
		if (count > 0) {
			for (int i = 0; i < count; i++) {
				if (isDir[i]) dirCount++;
				else fileCount++;
			}
		}
		delete[] names;
		delete[] isDir;

		char buf[48];
		snprintf(buf, sizeof(buf), L->TXT_FOLDER_COUNTS, fileCount, dirCount);

		String lines[] = {
			L->MENU_FILES_FOLDER_INFO,
			createFileCurrentDir,
			String(buf)
		};
		centeredPrintRows(lines, 3, SMALL_TEXT);
	}
	checkExit(PID::FILE_OPTIONS);
}
