#include "locale.h"

const Locale LANG_DE = {
	// Common menu items
	.MENU_BACK                   = "zurück",
	.MENU_SCAN                   = "scan",
	.MENU_RESCAN                 = "erneut scan.",

	// Wi-Fi menu
	.MENU_WIFI                   = "Wi-Fi",
	.MENU_WIFI_INFO              = "info",
	.MENU_WIFI_DEAUTH            = "deauth",
	.MENU_WIFI_CONNECT           = "verbinden",
	.MENU_WIFI_DISCONNECT        = "wifi trennen",
	.MENU_WIFI_AP                = "Wi-Fi AP",
	.MENU_WIFI_WEATHER           = "wetter",
	.MENU_WIFI_SYNC_TIME         = "zeit sync",

	// Bluetooth menu
	.MENU_BT                     = "Bluetooth",
	.MENU_BT_INFO                = "info",
	.MENU_BT_FIND                = "suchen",
	.MENU_BT_KEYBOARD            = "tastatur",
	.MENU_BT_AIR_MOUSE           = "air mouse",
	.MENU_BT_PRESENTER           = "präsent.",
	.MENU_BT_SHUTTER             = "auslöser",
	.MENU_BT_BAD_BLE             = "bad BLE",
	.MENU_BT_RUN_SCRIPT          = "skript",

	// NFC menu
	.MENU_NFC                    = "NFC",
	.MENU_NFC_READ_UID           = "UID lesen",
	.MENU_NFC_WRITE_UID          = "UID schreiben",

	// Settings menu
	.MENU_SETTINGS               = "einstellungen",
	.MENU_SETTINGS_TIME          = "uhrzeit",
	.MENU_SETTINGS_BRIGHTNESS    = "helligkeit",
	.MENU_SETTINGS_ROTATION      = "drehung",
	.MENU_SETTINGS_COLORS        = "farben",
	.MENU_SETTINGS_STATUS_BAR    = "statusleiste",
	.MENU_SETTINGS_STATUS_BAR_PID = "PID",
	.MENU_SETTINGS_STARTUP_SOUND = "startsound",
	.MENU_SETTINGS_FONT          = "schrift",
	.MENU_SETTINGS_KB_GYRO       = "gyro tastatur",
	.MENU_SETTINGS_LANGUAGE      = "sprache",

	// Other menu
	.MENU_OTHER                  = "sonstiges",
	.MENU_OTHER_SYSTEM           = "system",
	.MENU_OTHER_CLOCK            = "uhr",
	.MENU_OTHER_SOUND_LEVEL      = "lautstärke",
	.MENU_OTHER_FLASHLIGHT       = "taschenlampe",
	.MENU_OTHER_FLICKER          = "flackern",
	.MENU_OTHER_LEVEL_TOOL       = "wasserwaage",

	// System menu
	.MENU_SYSTEM_INFO            = "info",
	.MENU_SYSTEM_SHUTDOWN        = "ausschalten",
	.MENU_SYSTEM_BATTERY_INFO    = "akku",

	// Common text
	.TXT_PRESS_A                 = "A drücken",
	.TXT_ENABLED                 = "aktiviert",
	.TXT_DISABLED                = "deaktiviert",
	.TXT_SCANNING                = "Scannen...",
	.TXT_CONNECTING              = "Verbinden...",
	.TXT_CONNECTED               = "Verbunden",
	.TXT_DISCONNECTED            = "Getrennt",
	.TXT_DISCONNECTING           = "Trennen...",
	.TXT_WAITING_CONNECTION      = "Warten...",
	.TXT_NOT_CONNECTED           = "Nicht verbunden",
	.TXT_FAILED                  = "Fehlgeschlagen",
	.TXT_NA                      = "n/v",
	.TXT_UNNAMED                 = "unbenannt",

	// Wi-Fi text
	.TXT_WIFI_AP_ENABLED         = "WiFi AP aktiv",
	.TXT_WIFI_SSID_NOT_FOUND     = "SSID nicht gef.",
	.TXT_WIFI_TIMEOUT            = "Zeitüberschr.",
	.TXT_WIFI_WRONG_PASSWORD     = "Falsches Passwort",
	.TXT_WIFI_SYNC_FAILED        = "Sync fehlgesch.",
	.TXT_WIFI_TIME_SYNCED        = "Zeit synchronisiert",
	.TXT_WIFI_NOT_CONNECTED      = "Kein WLAN",

	// Bluetooth text
	.TXT_BT_FILE_ERROR           = "Dateifehler",
	.TXT_BT_PRESS_A_TO_RUN       = "A zum Starten",
	.TXT_BT_RUNNING              = "Läuft...",
	.TXT_BT_DONE                 = "Fertig!",

	// NFC text
	.TXT_NFC_NO_UID_STORED       = "Kein UID gespeich.",
	.TXT_NFC_READ_TAG_FIRST      = "Erst Tag lesen",
	.TXT_NFC_INVALID_UID         = "Ungült. UID",
	.TXT_NFC_UID_4BYTE_REQUIRED  = "4-Byte UID nötig",
	.TXT_NFC_WRITE_SUCCESS       = "Schreiben erfolgr.!",
	.TXT_NFC_UID_WRITTEN         = "UID geschrieben",
	.TXT_NFC_WRITE_FAILED        = "Schreiben fehlg.",
	.TXT_NFC_WRITE_ERROR         = "Kein Tag/Fehler",
	.TXT_NFC_READY_TO_WRITE      = "Bereit zum Schreib.",

	// Settings text
	.TXT_SETTINGS_LANGUAGE       = "Sprache: ",
	.TXT_SETTINGS_BRIGHTNESS     = "helligkeit: ",
	.TXT_SETTINGS_COLOR          = "farbe: ",
	.TXT_SETTINGS_FONT           = "Schrift:",
	.TXT_SETTINGS_ROTATION       = "drehung",
	.TXT_SETTINGS_SOUND          = "Sound: ",
	.TXT_SETTINGS_STATUS_BAR     = "statusleiste: ",
	.TXT_SETTINGS_PID            = "PID: ",
	.TXT_SETTINGS_KB_GYRO        = "gyro tastatur: ",
};
