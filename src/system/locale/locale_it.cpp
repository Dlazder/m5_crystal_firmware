#include "locale.h"

const Locale LANG_IT = {
    // Common menu items
    .MENU_BACK                    = "indietro",
    .MENU_SCAN                    = "scansione",
    .MENU_RESCAN                  = "riscansione",

    // Wi-Fi menu
    .MENU_WIFI                    = "Wi-Fi",
    .MENU_WIFI_INFO               = "info",
    .MENU_WIFI_DEAUTH             = "deauth",
    .MENU_WIFI_CONNECT            = "connetti",
    .MENU_WIFI_DISCONNECT         = "disconnetti wifi",
    .MENU_WIFI_AP                 = "Wi-Fi AP",
    .MENU_WIFI_WEATHER            = "meteo",
    .MENU_WIFI_SYNC_TIME          = "sincronizza ora",

    // Bluetooth menu
    .MENU_BT                      = "Bluetooth",
    .MENU_BT_INFO                 = "info",
    .MENU_BT_FIND                 = "trova",
    .MENU_BT_KEYBOARD             = "tastiera",
    .MENU_BT_AIR_MOUSE            = "air mouse",
    .MENU_BT_PRESENTER            = "presentatore",
    .MENU_BT_SHUTTER              = "otturatore",
    .MENU_BT_BAD_BLE              = "bad BLE",

    // Files menu
    .MENU_FILES                   = "file",
    .MENU_FILES_SDCARD            = "SD card",
    .MENU_FILES_DELETE            = "elimina",
    .MENU_FILES_INFO              = "info",
    .MENU_FILES_RENAME            = "rinomina",
    .MENU_FILES_VIEW              = "visualizza",
    .MENU_FILES_LITTLEFS                = "littleFS",

    // NFC menu
    .MENU_NFC                     = "NFC",
    .MENU_NFC_READ_UID            = "leggi UID",
    .MENU_NFC_WRITE_UID           = "scrivi UID",

    // IR menu
    .MENU_IR                      = "IR",
    .MENU_IR_READ                 = "leggi segnale",
    .MENU_IR_SEND                 = "invia segnale",
    .MENU_IR_TV_B_GONE            = "TV-B-Gone",

    // IR text
    .TXT_IR_WAITING               = "In attesa...",
    .TXT_IR_PRESS_A_SAVE          = "A per salvare",
    .TXT_IR_PRESS_A_SEND          = "A per inviare",
    .TXT_IR_SAVED                 = "Salvato",
    .TXT_IR_SAVE_ERROR            = "Errore salvataggio",
    .TXT_IR_PARSE_ERROR           = "Errore file",
    .TXT_IR_TVB_RUNNING           = "Invio...",
    .TXT_IR_TVB_DONE              = "Fatto!",

    // Settings menu
    .MENU_SETTINGS                = "impostazioni",
    .MENU_SETTINGS_TIME           = "ora",
    .MENU_SETTINGS_BRIGHTNESS     = "luminosita",
    .MENU_SETTINGS_VOLUME         = "volume",
    .MENU_SETTINGS_ROTATION       = "rotazione",
    .MENU_SETTINGS_COLORS         = "colori",
    .MENU_SETTINGS_STATUS_BAR     = "barra stato",
    .MENU_SETTINGS_STATUS_BAR_PID = "PID",
    .MENU_SETTINGS_STARTUP_SOUND  = "suono avvio",
    .MENU_SETTINGS_FONT           = "carattere",
    .MENU_SETTINGS_KB_GYRO        = "giroscopio",
    .MENU_SETTINGS_LANGUAGE       = "lingua",
    .MENU_SETTINGS_DIMMING        = "oscura",

    // Other menu
    .MENU_OTHER                   = "altro",
    .MENU_OTHER_SYSTEM            = "sistema",
    .MENU_OTHER_CLOCK             = "orologio",
    .MENU_OTHER_SOUND_LEVEL       = "livello audio",
    .MENU_OTHER_FLASHLIGHT        = "torcia",
    .MENU_OTHER_FLICKER           = "sfarfallio",
    .MENU_OTHER_LEVEL_TOOL        = "livella",

    // System menu
    .MENU_SYSTEM_INFO             = "info",
    .MENU_SYSTEM_SHUTDOWN         = "spegni",
    .MENU_SYSTEM_BATTERY_INFO     = "batteria",

    // Common text
    .TXT_PRESS_A                  = "premi A",
    .TXT_ENABLED                  = "attivo",
    .TXT_DISABLED                 = "disattivo",
    .TXT_SCANNING                 = "Scansione...",
    .TXT_CONNECTING               = "Connessione...",
    .TXT_CONNECTED                = "Connesso",
    .TXT_DISCONNECTED             = "Disconnesso",
    .TXT_DISCONNECTING            = "Disconnessione...",
    .TXT_WAITING_CONNECTION       = "Attesa connessione",
    .TXT_NOT_CONNECTED            = "Non connesso",
    .TXT_FAILED                   = "Errore",
    .TXT_SUCCESS                  = "Successo",
    .TXT_ERROR                    = "Errore",
    .TXT_NA                       = "n/d",
    .TXT_UNNAMED                  = "senza nome",

    // Wi-Fi text
    .TXT_WIFI_AP_ENABLED          = "WiFi AP attivo",
    .TXT_WIFI_SSID_NOT_FOUND      = "Rete non trovata",
    .TXT_WIFI_TIMEOUT             = "Tempo scaduto",
    .TXT_WIFI_WRONG_PASSWORD      = "Password errata",
    .TXT_WIFI_SYNC_FAILED         = "Sync fallita",
    .TXT_WIFI_TIME_SYNCED         = "Ora sincronizzata",
    .TXT_WIFI_NOT_CONNECTED       = "No WiFi",

    // Bluetooth text
    .TXT_BT_FILE_ERROR            = "Errore file",
    .TXT_BT_PRESS_A_TO_RUN        = "Premi A per eseguire",
    .TXT_BT_RUNNING               = "Esecuzione...",
    .TXT_BT_DONE                  = "Fatto!",

    // NFC text
    .TXT_NFC_NO_UID_STORED        = "Nessun UID salvato",
    .TXT_NFC_READ_TAG_FIRST       = "Leggi un tag prima",
    .TXT_NFC_INVALID_UID          = "UID non valido",
    .TXT_NFC_UID_4BYTE_REQUIRED   = "UID 4 byte richiesto",
    .TXT_NFC_WRITE_SUCCESS        = "Scrittura riuscita!",
    .TXT_NFC_UID_WRITTEN          = "UID scritto sul tag",
    .TXT_NFC_WRITE_FAILED         = "Scrittura fallita!",
    .TXT_NFC_WRITE_ERROR          = "Nessun tag o errore",
    .TXT_NFC_READY_TO_WRITE       = "Pronto a scrivere...",

    // Settings text
    .TXT_SETTINGS_LANGUAGE        = "Lingua: ",
    .TXT_SETTINGS_BRIGHTNESS      = "luminosita: ",
    .TXT_SETTINGS_VOLUME          = "volume: ",
    .TXT_SETTINGS_COLOR           = "colore: ",
    .TXT_SETTINGS_FONT            = "Carattere:",
    .TXT_SETTINGS_ROTATION        = "rotazione",
    .TXT_SETTINGS_SOUND           = "Suono: ",
    .TXT_SETTINGS_STATUS_BAR      = "barra stato: ",
    .TXT_SETTINGS_PID             = "PID: ",
    .TXT_SETTINGS_KB_GYRO         = "Giroscopio: ",
};
