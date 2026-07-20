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
	.MENU_WIFI_WPA_BF            = "wpa bruteforce",
	.MENU_WIFI_WPA_BF_ALL        = "wpa bf all nets",
	.MENU_WIFI_DEAUTH_ALL        = "deauth all",
	.MENU_WIFI_EVIL_PORTAL       = "evil portal",
	.MENU_WIFI_EVIL_TWIN         = "evil twin",
	.MENU_WIFI_HANDSHAKE         = "handshake erfassen",
	.MENU_WIFI_PMKID             = "PMKID erfassen",
	.MENU_WIFI_CHANNEL_ANALYZER  = "Kanal-Analyse",
	.MENU_SETTINGS_WIFI_SSID      = "Wi-Fi SSID",

	// Bluetooth menu
	.MENU_BT                     = "Bluetooth",
	.MENU_BT_INFO                = "info",
	.MENU_BT_FIND                = "suchen",
	.MENU_BT_KEYBOARD            = "tastatur",
	.MENU_BT_AIR_MOUSE           = "air mouse",
	.MENU_BT_MOUSE_JIGGLER       = "Maus-Jiggler",
	.MENU_BT_PRESENTER           = "präsent.",
	.MENU_BT_SHUTTER             = "auslöser",
	.MENU_BT_BAD_BLE             = "bad BLE",
	.MENU_BT_SNIFFER             = "BLE sniffer",

	// Files menu
	.MENU_FILES                  = "dateien",
	.MENU_FILES_SDCARD           = "SD card",
	.MENU_FILES_DELETE           = "löschen",
	.MENU_FILES_INFO             = "info",
	.MENU_FILES_RENAME           = "umbenennen",
	.MENU_FILES_VIEW             = "ansehen",
	.MENU_FILES_EDIT             = "bearbeiten",
	.MENU_FILES_LITTLEFS               = "littleFS",

	// NFC menu
	.MENU_NFC                    = "NFC",
	.MENU_NFC_READ_UID           = "UID lesen",
	.MENU_NFC_WRITE_UID          = "UID schreiben",
	.MENU_NFC_ADVANCED_INFO    = "erweiterte Info",

	// IR menu
	.MENU_IR                     = "IR",
	.MENU_IR_READ                = "signal lesen",
	.MENU_IR_SEND                = "signal senden",
	.MENU_IR_TV_B_GONE           = "TV-B-Gone",

	// IR text
	.TXT_IR_WAITING              = "Warten...",
	.TXT_IR_PRESS_A_SAVE         = "A zum Speichern",
	.TXT_IR_PRESS_A_SEND         = "A zum Senden",
	.TXT_IR_SAVED                = "Gespeichert",
	.TXT_IR_SAVE_ERROR           = "Speicherfehler",
	.TXT_IR_PARSE_ERROR          = "Dateifehler",
	.TXT_IR_TVB_RUNNING          = "Senden...",
	.TXT_IR_TVB_DONE             = "Fertig!",

	// Settings menu
	.MENU_SETTINGS               = "einstellungen",
	.MENU_SETTINGS_TIME          = "uhrzeit",
	.MENU_SETTINGS_BRIGHTNESS    = "helligkeit",
	.MENU_SETTINGS_VOLUME        = "lautstärke",
	.MENU_SETTINGS_ROTATION      = "umkehren",
	.MENU_SETTINGS_COLORS        = "farben",
	.MENU_SETTINGS_ICONS         = "symbole",
	.MENU_SETTINGS_STATUS_BAR    = "statusleiste",
	.MENU_SETTINGS_STATUS_BAR_PID = "PID",
	.MENU_SETTINGS_STARTUP_SOUND = "startsound",
	.MENU_SETTINGS_FONT          = "schrift",
	.MENU_SETTINGS_KB_GYRO       = "gyro tastatur",
	.MENU_SETTINGS_LANGUAGE      = "sprache",
	.MENU_SETTINGS_DIMMING       = "abdunkeln",
	.MENU_SETTINGS_WEBSERVER_FS  = "WebServer FS",

	// Other menu
	.MENU_OTHER                  = "sonstiges",
	.MENU_OTHER_SYSTEM           = "system",
	.MENU_OTHER_CLOCK            = "uhr",
	.MENU_OTHER_SOUND_LEVEL      = "lautstärke",
	.MENU_OTHER_FLASHLIGHT       = "taschenlampe",
	.MENU_OTHER_FLICKER          = "flackern",
	.MENU_OTHER_LEVEL_TOOL       = "wasserwaage",

	// USB menu
	.MENU_USB                    = "USB",
	.MENU_USB_BAD_USB            = "bad USB",
	.MENU_USB_MOUSE_JIGGLER      = "Maus-Jiggler",
	.MENU_USB_MOUSE              = "Luftmaus",
	.MENU_USB_KEYBOARD           = "Tastatur",
	.MENU_USB_STORAGE            = "Speicher",

	// System menu
	.MENU_SYSTEM_INFO            = "info",
	.MENU_SYSTEM_SHUTDOWN        = "ausschalten",
	.MENU_SYSTEM_BATTERY_INFO    = "akku",

	// Common text
	.TXT_PRESS_A                 = "A drücken",
	.TXT_LEFT                    = "links",
	.TXT_RIGHT                   = "rechts",
	.TXT_SELECT                  = "wählen",
	.TXT_ADJUST                  = "anpassen",
	.TXT_SWITCH                  = "wechseln",
	.TXT_ENABLED                 = "aktiviert",
	.TXT_DISABLED                = "deaktiviert",
	.TXT_ON                      = "EIN",
	.TXT_OFF                     = "AUS",
	.TXT_SCANNING                = "Scannen...",
	.TXT_CONNECTING              = "Verbinden...",
	.TXT_CONNECTED               = "Verbunden",
	.TXT_DISCONNECTED            = "Getrennt",
	.TXT_DISCONNECTING           = "Trennen...",
	.TXT_WAITING_CONNECTION      = "Warten...",
	.TXT_WORKING                 = "Arbeitet",
	.TXT_NOT_CONNECTED           = "Nicht verbunden",
	.TXT_FAILED                  = "Fehlgeschlagen",
	.TXT_SUCCESS                 = "Erfolg",
	.TXT_ERROR                   = "Fehler",
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
	.TXT_WIFI_BF_LOADED          = "Geladen: %d",
	.TXT_WIFI_BF_DONE            = "Versucht: %d",
	.TXT_WIFI_BF_ALL_NETS        = "Netze: %d",
	.TXT_WIFI_BF_ALL_CRACKED     = "Geknackt: %d/%d",
	.TXT_WIFI_NO_NETWORKS        = "Keine Netze",
	.TXT_WIFI_EVIL_PORTAL_RUNNING     = "Evil Portal aktiv",
	.TXT_WIFI_EVIL_PORTAL_CRED_CAPTURED = "Creds erfasst!",
	.TXT_WIFI_EVIL_TWIN_RUNNING     = "Evil Twin aktiv",
	.TXT_SETTINGS_WIFI_SSID      = "Wi-Fi SSID: ",
	
	.TXT_WIFI_HANDSHAKE_PACKETS   = "Handshakes: ",
	.TXT_WIFI_HANDSHAKE_SAVED     = "Gespeichert auf SD",
	.TXT_WIFI_NO_SCAN_DATA        = "Netze zuerst scannen",
				
	// Bluetooth text
	.TXT_BT_FILE_ERROR           = "Dateifehler",
	.TXT_BT_PRESS_A_TO_RUN       = "A zum Starten",
	.TXT_BT_RUNNING              = "Läuft...",
	.TXT_BT_DONE                 = "Fertig!",

	// USB text
	.TXT_USB_HID_UNSOPPORTED     = "USB HID nicht unterstützt",
	.TXT_USB_STORAGE_NO_SD       = "Keine SD-Karte",
	.TXT_USB_STORAGE_ACTIVE      = "USB-Speicher aktiv",
	.TXT_USB_STORAGE_HINT        = "Auswerfen zum Beenden",

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
	.TXT_SETTINGS_VOLUME         = "lautstärke: ",
	.TXT_SETTINGS_COLOR          = "farbe: ",
	.TXT_SETTINGS_FONT           = "Schrift:",
	.TXT_SETTINGS_ROTATION       = "umkehren",
	.TXT_SETTINGS_SOUND          = "Sound: ",
	.TXT_SETTINGS_STATUS_BAR     = "statusleiste: ",
	.TXT_SETTINGS_PID            = "PID: ",
	.TXT_SETTINGS_KB_GYRO        = "gyro tastatur: ",
	.TXT_SETTINGS_WEBSERVER_FS   = "WebServer FS: ",
};
