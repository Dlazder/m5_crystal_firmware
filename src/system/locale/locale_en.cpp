#include "locale.h"

const Locale LANG_EN = {
	// Common menu items
	.MENU_BACK                   = "back",
	.MENU_SCAN                   = "scan",
	.MENU_RESCAN                 = "rescan",

	// Wi-Fi menu
	.MENU_WIFI                   = "Wi-Fi",
	.MENU_WIFI_INFO              = "info",
	.MENU_WIFI_DEAUTH            = "deauth",
	.MENU_WIFI_CONNECT           = "connect",
	.MENU_WIFI_DISCONNECT        = "disconnect wifi",
	.MENU_WIFI_AP                = "Wi-Fi AP",
	.MENU_WIFI_WEATHER           = "weather",
	.MENU_WIFI_SYNC_TIME         = "sync time",

	// Bluetooth menu
	.MENU_BT                     = "Bluetooth",
	.MENU_BT_INFO                = "info",
	.MENU_BT_FIND                = "find",
	.MENU_BT_KEYBOARD            = "keyboard",
	.MENU_BT_AIR_MOUSE           = "air mouse",
	.MENU_BT_PRESENTER           = "presenter",
	.MENU_BT_SHUTTER             = "shutter",
	.MENU_BT_BAD_BLE             = "bad BLE",

	// Files menu
	.MENU_FILES                  = "files",
	.MENU_FILES_SDCARD           = "SD card",
	.MENU_FILES_DELETE           = "delete",
	.MENU_FILES_INFO             = "info",
	.MENU_FILES_RENAME           = "rename",
	.MENU_FILES_LITTLEFS               = "littleFS",

	// NFC menu
	.MENU_NFC                    = "NFC",
	.MENU_NFC_READ_UID           = "read UID",
	.MENU_NFC_WRITE_UID          = "write UID",

	// Settings menu
	.MENU_SETTINGS               = "settings",
	.MENU_SETTINGS_TIME          = "time",
	.MENU_SETTINGS_BRIGHTNESS    = "brightness",
	.MENU_SETTINGS_ROTATION      = "rotation",
	.MENU_SETTINGS_COLORS        = "colors",
	.MENU_SETTINGS_STATUS_BAR    = "status bar",
	.MENU_SETTINGS_STATUS_BAR_PID = "PID",
	.MENU_SETTINGS_STARTUP_SOUND = "startup sound",
	.MENU_SETTINGS_FONT          = "font",
	.MENU_SETTINGS_KB_GYRO       = "keyboard gyro",
	.MENU_SETTINGS_LANGUAGE      = "language",

	// Other menu
	.MENU_OTHER                  = "other",
	.MENU_OTHER_SYSTEM           = "system",
	.MENU_OTHER_CLOCK            = "clock",
	.MENU_OTHER_SOUND_LEVEL      = "sound level",
	.MENU_OTHER_FLASHLIGHT       = "flashlight",
	.MENU_OTHER_FLICKER          = "flicker",
	.MENU_OTHER_LEVEL_TOOL       = "level tool",

	// System menu
	.MENU_SYSTEM_INFO            = "info",
	.MENU_SYSTEM_SHUTDOWN        = "shutdown",
	.MENU_SYSTEM_BATTERY_INFO    = "battery info",

	// Common text
	.TXT_PRESS_A                 = "press A",
	.TXT_ENABLED                 = "enabled",
	.TXT_DISABLED                = "disabled",
	.TXT_SCANNING                = "Scanning...",
	.TXT_CONNECTING              = "Connecting...",
	.TXT_CONNECTED               = "Connected",
	.TXT_DISCONNECTED            = "Disconnected",
	.TXT_DISCONNECTING           = "Disconnecting...",
	.TXT_WAITING_CONNECTION      = "Waiting connection",
	.TXT_NOT_CONNECTED           = "Not connected",
	.TXT_FAILED                  = "Failed",
	.TXT_SUCCESS                 = "Success",
	.TXT_ERROR                   = "Error",
	.TXT_NA                      = "n/a",
	.TXT_UNNAMED                 = "unnamed",

	// Wi-Fi text
	.TXT_WIFI_AP_ENABLED         = "WiFi AP enabled",
	.TXT_WIFI_SSID_NOT_FOUND     = "SSID not found",
	.TXT_WIFI_TIMEOUT            = "Timeout",
	.TXT_WIFI_WRONG_PASSWORD     = "Wrong password",
	.TXT_WIFI_SYNC_FAILED        = "Sync failed",
	.TXT_WIFI_TIME_SYNCED        = "Time synced",
	.TXT_WIFI_NOT_CONNECTED      = "No WiFi",

	// Bluetooth text
	.TXT_BT_FILE_ERROR           = "File error",
	.TXT_BT_PRESS_A_TO_RUN       = "Press A to run",
	.TXT_BT_RUNNING              = "Running...",
	.TXT_BT_DONE                 = "Done!",

	// NFC text
	.TXT_NFC_NO_UID_STORED       = "No UID stored",
	.TXT_NFC_READ_TAG_FIRST      = "Read a tag first",
	.TXT_NFC_INVALID_UID         = "Invalid UID",
	.TXT_NFC_UID_4BYTE_REQUIRED  = "4-byte UID required",
	.TXT_NFC_WRITE_SUCCESS       = "Write successful!",
	.TXT_NFC_UID_WRITTEN         = "UID written to tag",
	.TXT_NFC_WRITE_FAILED        = "Write failed!",
	.TXT_NFC_WRITE_ERROR         = "No tag or write error",
	.TXT_NFC_READY_TO_WRITE      = "Ready to write...",

	// Settings text
	.TXT_SETTINGS_LANGUAGE       = "Language: ",
	.TXT_SETTINGS_BRIGHTNESS     = "brightness: ",
	.TXT_SETTINGS_COLOR          = "color: ",
	.TXT_SETTINGS_FONT           = "Font:",
	.TXT_SETTINGS_ROTATION       = "rotation",
	.TXT_SETTINGS_SOUND          = "Sound: ",
	.TXT_SETTINGS_STATUS_BAR     = "status bar: ",
	.TXT_SETTINGS_PID            = "PID: ",
	.TXT_SETTINGS_KB_GYRO        = "KB gyro: ",
};
