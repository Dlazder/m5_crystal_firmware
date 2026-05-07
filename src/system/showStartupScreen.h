void showStartupScreen() {
	bool statusBarOld = statusBar;
	statusBar = false;
	String lines[] = {
    "Crystal",
    "firmware",
    "v" + String(FIRMWARE_VERSION),
  };
	centeredPrintRows(lines, 3, MEDIUM_TEXT);
	statusBar = statusBarOld;
}