// PID::BT_MOUSE_JIGGLER

// Interval between jiggle movements, in milliseconds.
#define BT_JIGGLER_INTERVAL 5000
// How many pixels the cursor is nudged on each jiggle.
#define BT_JIGGLER_STEP 5

void bluetoothMouseJigglerLoop() {
	static bool dirRight = true;
	// Private timer so we don't fight the shared globalPreviousTimer that the
	// status bar, dimming and button handling all reset behind our back.
	static int jiggleTimer = 0;

	if (isSetup()) {
		dirRight = true;
		bleConnect();
	}

	bleHandleConnection(
		[]() {
			jiggleTimer = globalTimer; // start our private timer fresh
			soundSuccess();
		},
		[]() {
			centeredPrint(L->TXT_NOT_CONNECTED, MEDIUM_TEXT);
			soundError();
		}
	);

	if (bleConnected) {
		// Animated "working" status, redrawn every frame so the spinner spins.
		centeredPrint(L->TXT_WORKING, MEDIUM_TEXT, true);
		drawSpinner();
		canvas.pushSprite(0, getStatusBarHeight());

		// Nudge the cursor left/right on a private timer so it stays in place.
		if (checkTimer(BT_JIGGLER_INTERVAL, true, &jiggleTimer)) {
			int delta = dirRight ? BT_JIGGLER_STEP : -BT_JIGGLER_STEP;
			bleMouse.move(delta, 0);
			dirRight = !dirRight;
		}
	}

	if (checkExit()) {
		bleConnected = false;
		centeredPrint(L->TXT_DISCONNECTING, MEDIUM_TEXT);
	}
}
