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
	const char* MENU_WIFI_WPA_BF;
	const char* MENU_WIFI_WPA_BF_ALL;
	const char* MENU_WIFI_DEAUTH_ALL;
	const char* MENU_WIFI_EVIL_PORTAL;
	const char* MENU_WIFI_EVIL_TWIN;
	const char* MENU_WIFI_HANDSHAKE;
	const char* MENU_WIFI_PMKID;
	const char* MENU_WIFI_CHANNEL_ANALYZER;
	const char* MENU_WIFI_WEB_SERVER;
	const char* MENU_SETTINGS_WIFI_SSID;

	// Bluetooth menu
	const char* MENU_BT;
	const char* MENU_BT_INFO;
	const char* MENU_BT_FIND;
	const char* MENU_BT_KEYBOARD;
	const char* MENU_BT_AIR_MOUSE;
	const char* MENU_BT_MOUSE_JIGGLER;
	const char* MENU_BT_PRESENTER;
	const char* MENU_BT_SHUTTER;
	const char* MENU_BT_BAD_BLE;
	const char* MENU_BT_SNIFFER;

	// Files menu
	const char* MENU_FILES;
	const char* MENU_FILES_SDCARD;
	const char* MENU_FILES_DELETE;
	const char* MENU_FILES_INFO;
	const char* MENU_FILES_RENAME;
	const char* MENU_FILES_VIEW;
	const char* MENU_FILES_EDIT;
	const char* MENU_FILES_LITTLEFS;

	// NFC menu
	const char* MENU_NFC;
	const char* MENU_NFC_READ_UID;
	const char* MENU_NFC_WRITE_UID;
	const char* MENU_NFC_ADVANCED_INFO;

	// IR menu
	const char* MENU_IR;
	const char* MENU_IR_READ;
	const char* MENU_IR_SEND;
	const char* MENU_IR_TV_B_GONE;

	// IR text
	const char* TXT_IR_WAITING;
	const char* TXT_IR_PRESS_A_SAVE;
	const char* TXT_IR_PRESS_A_SEND;
	const char* TXT_IR_SAVED;
	const char* TXT_IR_SAVE_ERROR;
	const char* TXT_IR_PARSE_ERROR;
	const char* TXT_IR_TVB_RUNNING;
	const char* TXT_IR_TVB_DONE;

	// Settings menu
	const char* MENU_SETTINGS;
	const char* MENU_SETTINGS_TIME;
	const char* MENU_SETTINGS_BRIGHTNESS;
	const char* MENU_SETTINGS_VOLUME;
	const char* MENU_SETTINGS_ROTATION;
	const char* MENU_SETTINGS_COLORS;
	const char* MENU_SETTINGS_ICONS;
	const char* MENU_SETTINGS_STATUS_BAR;
	const char* MENU_SETTINGS_STATUS_BAR_PID;
	const char* MENU_SETTINGS_STARTUP_SOUND;
	const char* MENU_SETTINGS_FONT;
	const char* MENU_SETTINGS_KB_GYRO;
	const char* MENU_SETTINGS_LANGUAGE;
	const char* MENU_SETTINGS_DIMMING;
	const char* MENU_SETTINGS_WEBSERVER_FS;

	// Other menu
	const char* MENU_OTHER;
	const char* MENU_OTHER_SYSTEM;
	const char* MENU_OTHER_CLOCK;
	const char* MENU_OTHER_SOUND_LEVEL;
	const char* MENU_OTHER_FLASHLIGHT;
	const char* MENU_OTHER_FLICKER;
	const char* MENU_OTHER_LEVEL_TOOL;

	// USB menu
	const char* MENU_USB;
	const char* MENU_USB_BAD_USB;
	const char* MENU_USB_MOUSE_JIGGLER;
	const char* MENU_USB_MOUSE;
	const char* MENU_USB_KEYBOARD;
	const char* MENU_USB_STORAGE;

	// System menu
	const char* MENU_SYSTEM_INFO;
	const char* MENU_SYSTEM_SHUTDOWN;
	const char* MENU_SYSTEM_BATTERY_INFO;


	// Common text
	const char* TXT_PRESS_A;
	const char* TXT_LEFT;
	const char* TXT_RIGHT;
	const char* TXT_SELECT;
	const char* TXT_ADJUST;
	const char* TXT_SWITCH;
	const char* TXT_ENABLED;
	const char* TXT_DISABLED;
	const char* TXT_ON;
	const char* TXT_OFF;
	const char* TXT_SCANNING;
	const char* TXT_CONNECTING;
	const char* TXT_CONNECTED;
	const char* TXT_DISCONNECTED;
	const char* TXT_DISCONNECTING;
	const char* TXT_WAITING_CONNECTION;
	const char* TXT_WORKING;
	const char* TXT_NOT_CONNECTED;
	const char* TXT_FAILED;
	const char* TXT_SUCCESS;
	const char* TXT_ERROR;
	const char* TXT_NA;
	const char* TXT_UNNAMED;
	const char* TXT_SAVED_SD;
	const char* TXT_SAVED_LFS;

	// Wi-Fi text
	const char* TXT_WIFI_AP_ENABLED;
	const char* TXT_WIFI_SSID_NOT_FOUND;
	const char* TXT_WIFI_TIMEOUT;
	const char* TXT_WIFI_WRONG_PASSWORD;
	const char* TXT_WIFI_SYNC_FAILED;
	const char* TXT_WIFI_TIME_SYNCED;
	const char* TXT_WIFI_NOT_CONNECTED;
	const char* TXT_WIFI_BF_LOADED;      // printf format: "Loaded: %d"
	const char* TXT_WIFI_BF_DONE;        // printf format: "Tried: %d"
	const char* TXT_WIFI_BF_ALL_NETS;    // printf format: "Nets: %d"
	const char* TXT_WIFI_BF_ALL_CRACKED; // printf format: "Cracked: %d/%d"
	const char* TXT_WIFI_NO_NETWORKS;
	const char* TXT_WIFI_EVIL_PORTAL_RUNNING;
	const char* TXT_WIFI_EVIL_PORTAL_CRED_CAPTURED;
	const char* TXT_WIFI_EVIL_TWIN_RUNNING;
	const char* TXT_WIFI_WEB_SERVER_RUNNING;
	const char* TXT_SETTINGS_WIFI_SSID;
	const char* TXT_WIFI_HANDSHAKE_CAPTURING;
	const char* TXT_WIFI_HANDSHAKE_PACKETS;

	const char* TXT_WIFI_NO_SCAN_DATA;

	// Bluetooth text
	const char* TXT_BT_FILE_ERROR;
	const char* TXT_BT_PRESS_A_TO_RUN;
	const char* TXT_BT_RUNNING;
	const char* TXT_BT_DONE;

	// USB text
	const char* TXT_USB_HID_UNSOPPORTED;
	const char* TXT_USB_STORAGE_NO_SD;
	const char* TXT_USB_STORAGE_ACTIVE;
	const char* TXT_USB_STORAGE_HINT;

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
	const char* TXT_SETTINGS_VOLUME;
	const char* TXT_SETTINGS_COLOR;
	const char* TXT_SETTINGS_FONT;
	const char* TXT_SETTINGS_ROTATION;
	const char* TXT_SETTINGS_SOUND;
	const char* TXT_SETTINGS_STATUS_BAR;
	const char* TXT_SETTINGS_PID;
	const char* TXT_SETTINGS_KB_GYRO;
	const char* TXT_SETTINGS_WEBSERVER_FS;
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
