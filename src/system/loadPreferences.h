void loadPreferences() {
	Serial.println("Loading preferences...");

	rotation = getData("rotation", rotation);
	Serial.printf("Rotation: %d\n", rotation);
  DISP.setRotation(rotation);

	brightness = getData("brightness", brightness);
	Serial.printf("Brightness: %d\n", brightness);
	DISP.setBrightness(brightnessToHw(brightness));

	statusBar = getData("statusBar", statusBar);
	Serial.printf("Status bar: %s\n", statusBar ? "enabled" : "disabled");
  statusBarPid = getData("statusBarPid", statusBarPid);
	Serial.printf("Status bar PID: %s\n", statusBarPid ? "enabled" : "disabled");
  statusBarBattery = getData("statusBarBattery", statusBarBattery);
	Serial.printf("Status bar battery: %s\n", statusBarBattery ? "enabled" : "disabled");

	currentFontIndex = getData("fontIndex", currentFontIndex);
	Serial.printf("Font: %s\n", fontNames[currentFontIndex]);
  applyFont(currentFontIndex);

	colorIndex = getData("colorIndex", colorIndex);
  Serial.printf("Color: %s\n", colorsEntry[colorIndex]);
  FGCOLOR = colors[colorIndex];

	iconsEnabled = getData("icons", iconsEnabled);
	Serial.printf("Icons: %s\n", iconsEnabled ? "enabled" : "disabled");

	volume = getData("volume", volume);
	Serial.printf("Volume: %d\n", volume / volumeDividor);
	DEVICE.Speaker.setVolume(volume);

	startupSound = getData("startupSound", startupSound);
  Serial.printf("Startup sound: %s\n", startupSound ? "enabled" : "disabled");

	kbGyroEnabled = getData("kbGyro", kbGyroEnabled);
	Serial.printf("KB gyro: %s\n", kbGyroEnabled ? "enabled" : "disabled");

	languageIndex = getData("languageIndex", languageIndex);
	setLocale(locales[languageIndex]);
	Serial.printf("Language: %s\n", localeNames[languageIndex]);

	dimTimeoutIndex = getData("dimTimeout", dimTimeoutIndex);
	screenDimTimeout = dimTimeouts[dimTimeoutIndex];
	Serial.printf("Dim timeout index: %d\n", dimTimeoutIndex);

	webServerFs = getData("webServerFs", webServerFs);
	Serial.printf("WebServer FS: %s\n", webServerFs ? "enabled" : "disabled");

	Serial.println("Preferences loaded");
}