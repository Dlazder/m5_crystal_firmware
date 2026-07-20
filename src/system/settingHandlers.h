/**
 * Centralized handlers for every toggle-able setting.
 *
 * Each block is responsible for:
 *   1. Updating the global variable (the runtime source of truth)
 *   2. Persisting with setData()
 *   3. Any side effects (canvas recreation, display clear, etc.)
 *
 * To add a new toggle setting:
 *   - Add its settingKey to the MENU item in settingsMenuLoop.h
 *   - Add the corresponding strcmp branch below
 */

void handleSettingToggle(const char* key) {

	if (strcmp(key, "icons") == 0) {
		iconsEnabled = !iconsEnabled;
		setData("icons", iconsEnabled);
		return;
	}

	if (strcmp(key, "webServerFs") == 0) {
		webServerFs = !webServerFs;
		setData("webServerFs", webServerFs);
		return;
	}

	if (strcmp(key, "startupSound") == 0) {
		startupSound = !startupSound;
		setData("startupSound", startupSound);
		return;
	}

	if (strcmp(key, "kbGyro") == 0) {
		kbGyroEnabled = !kbGyroEnabled;
		setData("kbGyro", kbGyroEnabled);
		return;
	}

	if (strcmp(key, "rotation") == 0) {
		rotation = !rotation;
		setData("rotation", rotation);
		DISP.setRotation(rotation ? 3 : 1);
		DISP.clear();
		return;
	}

	if (strcmp(key, "statusBar") == 0) {
		statusBar = !statusBar;
		setData("statusBar", statusBar);
		recreateCanvas();
		DISP.clear();
		return;
	}

	if (strcmp(key, "statusBarPid") == 0) {
		statusBarPid = !getData("statusBarPid", statusBarPid);
		setData("statusBarPid", statusBarPid);
	}
}
