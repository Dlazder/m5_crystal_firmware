#pragma once
// Registry of all settings managed by the settings service.
// Included from settings.h (after the Setting type), once per translation unit.
//
// To add a setting: add one line here, then put its key in a menu item:
//   {PID::..., "label", nullptr, "<key>"}
// No loop file, PID or load/reset code needed. Side effects of a change go into
// an onChange function defined below and passed in the table.

// Side effects (run after the value is set, also by load() with settingsLoading set).
void recreateCanvas(); // utils/displayUtils/recreateCanvas.h

static void onRotationChanged(int v) {
	DISP.setRotation(v ? 3 : 1);
	DISP.clear();
}

static void onStatusBarChanged(int) {
	if (settingsLoading) return; // setup() creates the canvas after loading
	recreateCanvas();
	DISP.clear();
}

void soundBeep(); // utils/soundUtils.h

static void onBrightnessChanged(int v) {
	DISP.setBrightness(brightnessToHw(v));
}

static void onVolumeChanged(int v) {
	DEVICE.Speaker.setVolume(volumeToHw(v));
	if (!settingsLoading) soundBeep();
}

// Index-based settings: the shown name comes from fmt, the effect from onChange.
// locale: L-> texts are rebuilt by the menu on its next pass, so the language is applied on confirm (not live).
static String fmtFont(int i)     { return fontNames[i]; }
static String fmtColor(int i)    { return colorsEntry[i]; }
static String fmtLanguage(int i) { return localeNames[i]; }

static String fmtDimTimeout(int i) {
	switch (dimTimeouts[i]) {
		case 0: return L->TXT_DISABLED;
		case 30000: return "30s";
		case 60000: return "1m";
		case 180000: return "3m";
		case 300000: return "5m";
		default: return L->TXT_NA;
	}
}

static void onFontChanged(int v)     { applyFont(v); }
static void onColorChanged(int v)    { FGCOLOR = colors[v]; }
static void onLanguageChanged(int v) { setLocale(locales[v]); }

static void onDimTimeoutChanged(int v) {
	screenDimTimeout = dimTimeouts[v];
	if (screenIsDimmed) {
		DISP.setBrightness(brightnessToHw(brightness));
		screenIsDimmed = false;
	}
	updateTimer(&dimmingPreviousTimer);
}

static String fmtNfcModule(int i) { return i == NFC_MODULE_RC522 ? "RC522" : "PN532"; }

// Allowed UART baud rates (the stored value is the rate itself, not an index).
static const int uartBauds[] = {
	1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
};
static constexpr uint8_t uartBaudsCount = sizeof(uartBauds) / sizeof(uartBauds[0]);

// Timezone offsets in seconds (the stored value), shown as "UTC+3:00".
static const int timezoneOffsets[] = {
	-43200,  // UTC-12
	-39600,  // UTC-11
	-36000,  // UTC-10
	-32400,  // UTC-09
	-28800,  // UTC-08
	-25200,  // UTC-07
	-21600,  // UTC-06
	-18000,  // UTC-05
	-14400,  // UTC-04
	-10800,  // UTC-03
	 -7200,  // UTC-02
	 -3600,  // UTC-01
	      0,  // UTC+00
	   3600,  // UTC+01
	   7200,  // UTC+02
	  10800,  // UTC+03
	  14400,  // UTC+04
	  18000,  // UTC+05
	  19800,  // UTC+05:30
	  21600,  // UTC+06
	  25200,  // UTC+07
	  28800,  // UTC+08
	  32400,  // UTC+09
	  36000,  // UTC+10
	  39600,  // UTC+11
	  43200,  // UTC+12
	  46800,  // UTC+13
};
static constexpr uint8_t timezoneOffsetsCount = sizeof(timezoneOffsets) / sizeof(timezoneOffsets[0]);

static String fmtUtcOffset(int offsetSeconds) {
	int totalMin = offsetSeconds / 60;
	int h = totalMin / 60;
	int m = abs(totalMin) % 60;
	char buf[12];
	if (m == 0) {
		snprintf(buf, sizeof(buf), "UTC%+d:00", h);
	} else {
		snprintf(buf, sizeof(buf), "UTC%+d:%02d", h, m);
	}
	return String(buf);
}

// Defaults of TOGGLE and RANGE settings are the variables' initial values (conf.h / globals.h).
#define TOGGLE_SETTING(key, var, onChange) \
	{key, SettingType::TOGGLE, SettingGroup::NONE, &var, sizeof(var), SETTING_DEF_INITIAL, 0, 1, nullptr, 0, nullptr, onChange}

// live: every step is applied while editing (preview), ESC restores the old value.
#define RANGE_SETTING(key, var, lo, hi, fmt, live, onChange) \
	{key, SettingType::RANGE, SettingGroup::NONE, &var, sizeof(var), SETTING_DEF_INITIAL, lo, hi, nullptr, 0, fmt, onChange, live}

// One of an explicit list of values.
#define LIST_SETTING(key, var, list, listCount, def, fmt, onChange) \
	{key, SettingType::LIST, SettingGroup::NONE, &var, sizeof(var), def, 0, 0, list, listCount, fmt, onChange}

// Pins of one group must not collide (checked by Settings::hasConflict).
#define PIN_SETTING(key, group, var, def) \
	{key, SettingType::PIN, SettingGroup::group, &var, sizeof(var), def, 0, 0, devicePins, devicePinsCount, nullptr, nullptr}

static const Setting settingsRegistry[] = {
	TOGGLE_SETTING("icons",          iconsEnabled,     nullptr),
	TOGGLE_SETTING("webServerFs",    webServerFs,      nullptr),
	TOGGLE_SETTING("startupSound",   startupSound,     nullptr),
	TOGGLE_SETTING("kbGyro",         kbGyroEnabled,    nullptr),
	TOGGLE_SETTING("rotation",       rotation,         onRotationChanged),
	TOGGLE_SETTING("statusBar",      statusBar,        onStatusBarChanged),
	TOGGLE_SETTING("statusBarPid",   statusBarPid,     nullptr),
	TOGGLE_SETTING("sbSeconds",      statusBarSeconds, nullptr),
	TOGGLE_SETTING("uartLog",        uartLogEnabled,   nullptr),
	TOGGLE_SETTING("sdMountCrystal", sdMountCrystal,   nullptr),
	TOGGLE_SETTING("showMenuWifi",   showMenuWifi,     nullptr),
	TOGGLE_SETTING("showMenuBt",     showMenuBt,       nullptr),
	TOGGLE_SETTING("showMenuNfc",    showMenuNfc,      nullptr),
	TOGGLE_SETTING("showMenuIr",     showMenuIr,       nullptr),
	TOGGLE_SETTING("showMenuUsb",    showMenuUsb,      nullptr),
	TOGGLE_SETTING("showMenuRf",     showMenuRf,       nullptr),
	TOGGLE_SETTING("showMenuNrf",    showMenuNrf,      nullptr),
	TOGGLE_SETTING("showMenuUart",   showMenuUart,     nullptr),
	TOGGLE_SETTING("showMenuFiles",  showMenuFiles,    nullptr),
	TOGGLE_SETTING("showMenuOther",  showMenuOther,    nullptr),
#ifdef IR_USE_RMT
	TOGGLE_SETTING("irRxGpio",       irRxUseGpio,      nullptr),
#endif

	RANGE_SETTING("brightness", brightness, 1, BRIGHTNESS_STEPS, nullptr, true, onBrightnessChanged),
	RANGE_SETTING("volume",     volume,     1, volumeStep,       nullptr, true, onVolumeChanged),

	RANGE_SETTING("nfcModule", nfcModule, 0, 1, fmtNfcModule, false, nullptr),

	RANGE_SETTING("fontIndex",     currentFontIndex, 0, sizeof(systemFonts) / sizeof(systemFonts[0]) - 1, fmtFont,       true,  onFontChanged),
	RANGE_SETTING("colorIndex",    colorIndex,       0, sizeof(colors) / sizeof(colors[0]) - 1,           fmtColor,      true,  onColorChanged),
	RANGE_SETTING("languageIndex", languageIndex,    0, sizeof(locales) / sizeof(locales[0]) - 1,         fmtLanguage,   false, onLanguageChanged),
	RANGE_SETTING("dimTimeout",    dimTimeoutIndex,  0, dimTimeoutsCount - 1,                             fmtDimTimeout, false, onDimTimeoutChanged),

	PIN_SETTING("rfCsPin",   RF_PINS, rfCsPin,   CC1101_CS_PIN),
	PIN_SETTING("rfSckPin",  RF_PINS, rfSckPin,  CC1101_SCK_PIN),
	PIN_SETTING("rfMosiPin", RF_PINS, rfMosiPin, CC1101_MOSI_PIN),
	PIN_SETTING("rfMisoPin", RF_PINS, rfMisoPin, CC1101_MISO_PIN),
	PIN_SETTING("rfGdo0Pin", RF_PINS, rfGdo0Pin, CC1101_GDO0_PIN),

	PIN_SETTING("nrfCsnPin",  NRF_PINS, nrfCsnPin,  NRF_CSN_PIN),
	PIN_SETTING("nrfSckPin",  NRF_PINS, nrfSckPin,  NRF_SCK_PIN),
	PIN_SETTING("nrfMosiPin", NRF_PINS, nrfMosiPin, NRF_MOSI_PIN),
	PIN_SETTING("nrfMisoPin", NRF_PINS, nrfMisoPin, NRF_MISO_PIN),
	PIN_SETTING("nrfCePin",   NRF_PINS, nrfCePin,   NRF_CE_PIN),

	PIN_SETTING("pn532SdaPin", PN532_PINS, pn532SdaPin, NFC_SDA),
	PIN_SETTING("pn532SclPin", PN532_PINS, pn532SclPin, NFC_SCL),

	PIN_SETTING("rc522SsPin",   RC522_PINS, rc522SsPin,   RC522_SS_PIN),
	PIN_SETTING("rc522SckPin",  RC522_PINS, rc522SckPin,  RC522_SCK_PIN),
	PIN_SETTING("rc522MosiPin", RC522_PINS, rc522MosiPin, RC522_MOSI_PIN),
	PIN_SETTING("rc522MisoPin", RC522_PINS, rc522MisoPin, RC522_MISO_PIN),
	PIN_SETTING("rc522RstPin",  RC522_PINS, rc522RstPin,  RC522_RST_PIN),

	PIN_SETTING("irTxPin",   IR_PINS, irTxPin,   IR_SEND_PIN),
	PIN_SETTING("irRxPin",   IR_PINS, irRxPin,   IR_RECEIVE_PIN),

	PIN_SETTING("uartRxPin", UART_PINS, uartRxPin, UART_RX_PIN),
	PIN_SETTING("uartTxPin", UART_PINS, uartTxPin, UART_TX_PIN),
	LIST_SETTING("uartBaud", uartBaud, uartBauds, uartBaudsCount, UART_BAUD_DEFAULT, nullptr, nullptr),

	// CH9329 (USB-HID-over-UART bridge, Settings → USB) uses its own
	// HardwareSerial (Serial1). Sharing the UART_PINS group with
	// uartRxPin/uartTxPin lets hasConflict() flag it if a pin ends up assigned
	// to both the UART terminal (Serial2) and CH9329 at once.
	TOGGLE_SETTING("ch9329Enabled", ch9329Enabled, nullptr),
	PIN_SETTING("ch9329RxPin", UART_PINS, ch9329RxPin, CH9329_RX_PIN),
	PIN_SETTING("ch9329TxPin", UART_PINS, ch9329TxPin, CH9329_TX_PIN),
	LIST_SETTING("ch9329Baud", ch9329Baud, uartBauds, uartBaudsCount, 9600, nullptr, nullptr),

	LIST_SETTING("timezone", timezoneOffset, timezoneOffsets, timezoneOffsetsCount, SETTING_DEF_INITIAL, fmtUtcOffset, nullptr),
};
static const int settingsCount = sizeof(settingsRegistry) / sizeof(settingsRegistry[0]);
