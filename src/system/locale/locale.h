#pragma once

struct Locale {
	// Common menu items
	const char* MENU_BACK;
	const char* MENU_SCAN;
	const char* MENU_RESCAN;

	// Wi-Fi menu
	const char* MENU_WIFI;
	const char* MENU_WIFI_INFO;
	const char* MENU_WIFI_DEAUTH;
	const char* MENU_WIFI_CONNECT;
	const char* MENU_WIFI_DISCONNECT;
	const char* MENU_WIFI_AP;
	const char* MENU_WIFI_WEATHER;
	const char* MENU_WIFI_SYNC_TIME;

	// Bluetooth menu
	const char* MENU_BT;
	const char* MENU_BT_INFO;
	const char* MENU_BT_FIND;
	const char* MENU_BT_KEYBOARD;
	const char* MENU_BT_AIR_MOUSE;
	const char* MENU_BT_PRESENTER;
	const char* MENU_BT_SHUTTER;
	const char* MENU_BT_BAD_BLE;

	// Files menu
	const char* MENU_FILES;
	const char* MENU_FILES_SDCARD;
	const char* MENU_FILES_DELETE;
	const char* MENU_FILES_INFO;
	const char* MENU_FILES_RENAME;
	const char* MENU_FILES_LITTLEFS;

	// NFC menu
	const char* MENU_NFC;
	const char* MENU_NFC_READ_UID;
	const char* MENU_NFC_WRITE_UID;

	// Settings menu
	const char* MENU_SETTINGS;
	const char* MENU_SETTINGS_TIME;
	const char* MENU_SETTINGS_BRIGHTNESS;
	const char* MENU_SETTINGS_ROTATION;
	const char* MENU_SETTINGS_COLORS;
	const char* MENU_SETTINGS_STATUS_BAR;
	const char* MENU_SETTINGS_STATUS_BAR_PID;
	const char* MENU_SETTINGS_STARTUP_SOUND;
	const char* MENU_SETTINGS_FONT;
	const char* MENU_SETTINGS_KB_GYRO;
	const char* MENU_SETTINGS_LANGUAGE;

	// Other menu
	const char* MENU_OTHER;
	const char* MENU_OTHER_SYSTEM;
	const char* MENU_OTHER_CLOCK;
	const char* MENU_OTHER_SOUND_LEVEL;
	const char* MENU_OTHER_FLASHLIGHT;
	const char* MENU_OTHER_FLICKER;
	const char* MENU_OTHER_LEVEL_TOOL;

	// System menu
	const char* MENU_SYSTEM_INFO;
	const char* MENU_SYSTEM_SHUTDOWN;
	const char* MENU_SYSTEM_BATTERY_INFO;


	// Common text
	const char* TXT_PRESS_A;
	const char* TXT_ENABLED;
	const char* TXT_DISABLED;
	const char* TXT_SCANNING;
	const char* TXT_CONNECTING;
	const char* TXT_CONNECTED;
	const char* TXT_DISCONNECTED;
	const char* TXT_DISCONNECTING;
	const char* TXT_WAITING_CONNECTION;
	const char* TXT_NOT_CONNECTED;
	const char* TXT_FAILED;
	const char* TXT_SUCCESS;
	const char* TXT_ERROR;
	const char* TXT_NA;
	const char* TXT_UNNAMED;

	// Wi-Fi text
	const char* TXT_WIFI_AP_ENABLED;
	const char* TXT_WIFI_SSID_NOT_FOUND;
	const char* TXT_WIFI_TIMEOUT;
	const char* TXT_WIFI_WRONG_PASSWORD;
	const char* TXT_WIFI_SYNC_FAILED;
	const char* TXT_WIFI_TIME_SYNCED;
	const char* TXT_WIFI_NOT_CONNECTED;

	// Bluetooth text
	const char* TXT_BT_FILE_ERROR;
	const char* TXT_BT_PRESS_A_TO_RUN;
	const char* TXT_BT_RUNNING;
	const char* TXT_BT_DONE;

	// NFC text
	const char* TXT_NFC_NO_UID_STORED;
	const char* TXT_NFC_READ_TAG_FIRST;
	const char* TXT_NFC_INVALID_UID;
	const char* TXT_NFC_UID_4BYTE_REQUIRED;
	const char* TXT_NFC_WRITE_SUCCESS;
	const char* TXT_NFC_UID_WRITTEN;
	const char* TXT_NFC_WRITE_FAILED;
	const char* TXT_NFC_WRITE_ERROR;
	const char* TXT_NFC_READY_TO_WRITE;

	// Settings text
	const char* TXT_SETTINGS_LANGUAGE;
	const char* TXT_SETTINGS_BRIGHTNESS;
	const char* TXT_SETTINGS_COLOR;
	const char* TXT_SETTINGS_FONT;
	const char* TXT_SETTINGS_ROTATION;
	const char* TXT_SETTINGS_SOUND;
	const char* TXT_SETTINGS_STATUS_BAR;
	const char* TXT_SETTINGS_PID;
	const char* TXT_SETTINGS_KB_GYRO;
};

extern const Locale LANG_EN;
extern const Locale LANG_ES;
extern const Locale LANG_IT;
extern const Locale LANG_ID;
extern const Locale LANG_RU;
extern const Locale LANG_UK;
extern const Locale LANG_DE;

inline const Locale* L = &LANG_EN;

inline void setLocale(const Locale* lang) { L = lang; }
