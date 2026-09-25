// PID::ERROR_SCREEN

void errorLoop() {
	if (isSetup()) {
		centeredPrint(errorMessage, SMALL_TEXT, true);
		drawHintCustom("ESC: exit", "B: exit");
		canvas.pushSprite(0, getStatusBarHeight());
	};
	if (checkExit(errorReturnPid)) {
		errorMessage = "";
	}
}
