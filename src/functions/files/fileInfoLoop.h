// PID::FILE_INFO

String _formatFileSize(size_t bytes) {
	if (bytes < 1024) return String(bytes) + " B";
	if (bytes < 1024 * 1024) return String(bytes / 1024.0, 1) + " KB";
	return String(bytes / (1024.0 * 1024.0), 1) + " MB";
}

void fileInfoLoop() {
	if (isSetup()) {
		size_t fileSize = 0;

		if (selectedFileSourcePid == PID::FILE_PICKER_SD) {
		#if HAS_SD
			File f = SD.open(selectedFilePath.c_str());
			if (f) {
				fileSize = f.size();
				f.close();
			}
		#endif
		} else {
			File f = LittleFS.open(selectedFilePath.c_str());
			if (f) {
				fileSize = f.size();
				f.close();
			}
		}

		String name = selectedFilePath.substring(selectedFilePath.lastIndexOf('/') + 1);
		String source = (selectedFileSourcePid == PID::FILE_PICKER_SD) ? "SD card" : "LittleFS";

		String lines[] = {
			name,
			_formatFileSize(fileSize),
			source,
		};
		centeredPrintRows(lines, 3, SMALL_TEXT);
	}
	checkExit(PID::SELECTED_FILE_MENU);
}
