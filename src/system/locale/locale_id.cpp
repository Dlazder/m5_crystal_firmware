#include "locale.h"

const Locale LANG_ID = {
    // Common menu items
    .MENU_BACK                    = "kembali",
    .MENU_SCAN                    = "pindai",
    .MENU_RESCAN                  = "pindai ulang",

    // Wi-Fi menu
    .MENU_WIFI                    = "Wi-Fi",
    .MENU_WIFI_INFO               = "info",
    .MENU_WIFI_DEAUTH             = "deauth",
    .MENU_WIFI_CONNECT            = "hubungkan",
    .MENU_WIFI_DISCONNECT         = "putuskan wifi",
    .MENU_WIFI_AP                 = "Wi-Fi AP",
    .MENU_WIFI_WEATHER            = "cuaca",
    .MENU_WIFI_SYNC_TIME          = "sinkron waktu",
    .MENU_WIFI_WPA_BF             = "wpa bruteforce",
    .MENU_WIFI_WPA_BF_ALL         = "wpa bf semua net",
    .MENU_WIFI_DEAUTH_ALL         = "deauth semua",
    .MENU_WIFI_EVIL_PORTAL       = "evil portal",
    .MENU_WIFI_EVIL_TWIN         = "evil twin",
    .MENU_WIFI_HANDSHAKE         = "tangkap handshake",
    .MENU_WIFI_PMKID             = "tangkap PMKID",
    .MENU_WIFI_CHANNEL_ANALYZER  = "analisis saluran",
    .MENU_SETTINGS_WIFI_SSID      = "Wi-Fi SSID",

    // Bluetooth menu
    .MENU_BT                      = "Bluetooth",
    .MENU_BT_INFO                 = "info",
    .MENU_BT_FIND                 = "cari",
    .MENU_BT_KEYBOARD             = "keyboard",
    .MENU_BT_AIR_MOUSE            = "air mouse",
    .MENU_BT_MOUSE_JIGGLER        = "mouse jiggler",
    .MENU_BT_PRESENTER            = "presenter",
    .MENU_BT_SHUTTER              = "rana",
    .MENU_BT_BAD_BLE              = "bad BLE",
    .MENU_BT_SNIFFER              = "sniffer BLE",

    // Files menu
    .MENU_FILES                   = "berkas",
    .MENU_FILES_SDCARD            = "SD card",
    .MENU_FILES_DELETE            = "hapus",
    .MENU_FILES_INFO              = "info",
    .MENU_FILES_RENAME            = "ganti nama",
    .MENU_FILES_VIEW              = "lihat",
    .MENU_FILES_EDIT              = "edit",
    .MENU_FILES_LITTLEFS                = "littleFS",

    // NFC menu
    .MENU_NFC                     = "NFC",
    .MENU_NFC_READ_UID            = "baca UID",
    .MENU_NFC_WRITE_UID           = "tulis UID",
    .MENU_NFC_ADVANCED_INFO       = "info lanjutan",

    // IR menu
    .MENU_IR                      = "IR",
    .MENU_IR_READ                 = "baca sinyal",
    .MENU_IR_SEND                 = "kirim sinyal",
    .MENU_IR_TV_B_GONE            = "TV-B-Gone",

    // IR text
    .TXT_IR_WAITING               = "Menunggu...",
    .TXT_IR_PRESS_A_SAVE          = "A untuk simpan",
    .TXT_IR_PRESS_A_SEND          = "A untuk kirim",
    .TXT_IR_SAVED                 = "Tersimpan",
    .TXT_IR_SAVE_ERROR            = "Gagal simpan",
    .TXT_IR_PARSE_ERROR           = "Error file",
    .TXT_IR_TVB_RUNNING           = "Mengirim...",
    .TXT_IR_TVB_DONE              = "Selesai!",

    // Settings menu
    .MENU_SETTINGS                = "pengaturan",
    .MENU_SETTINGS_TIME           = "waktu",
    .MENU_SETTINGS_BRIGHTNESS     = "kecerahan",
    .MENU_SETTINGS_VOLUME         = "volume",
    .MENU_SETTINGS_ROTATION       = "rotasi",
    .MENU_SETTINGS_COLORS         = "warna",
    .MENU_SETTINGS_STATUS_BAR     = "bilah status",
    .MENU_SETTINGS_STATUS_BAR_PID = "PID",
    .MENU_SETTINGS_STARTUP_SOUND  = "suara nyala",
    .MENU_SETTINGS_FONT           = "font",
    .MENU_SETTINGS_KB_GYRO        = "giroskop",
    .MENU_SETTINGS_LANGUAGE       = "bahasa",
    .MENU_SETTINGS_DIMMING        = "redupkan",
    .MENU_SETTINGS_WEBSERVER_FS   = "web server FS",

    // Other menu
    .MENU_OTHER                   = "lainnya",
    .MENU_OTHER_SYSTEM            = "sistem",
    .MENU_OTHER_CLOCK             = "jam",
    .MENU_OTHER_SOUND_LEVEL       = "level suara",
    .MENU_OTHER_FLASHLIGHT        = "senter",
    .MENU_OTHER_FLICKER           = "kedip",
    .MENU_OTHER_LEVEL_TOOL        = "waterpass",

    // USB menu
    .MENU_USB                     = "USB",
    .MENU_USB_BAD_USB             = "bad USB",
    .MENU_USB_MOUSE_JIGGLER       = "mouse jiggler",
    .MENU_USB_MOUSE               = "air mouse",
    .MENU_USB_KEYBOARD            = "keyboard",
    .MENU_USB_STORAGE             = "penyimpanan",

    // System menu
    .MENU_SYSTEM_INFO             = "info",
    .MENU_SYSTEM_SHUTDOWN         = "matikan",
    .MENU_SYSTEM_BATTERY_INFO     = "baterai",

    // Common text
    .TXT_PRESS_A                  = "tekan A",
    .TXT_LEFT                     = "kiri",
    .TXT_RIGHT                    = "kanan",
    .TXT_SELECT                   = "pilih",
    .TXT_ADJUST                   = "atur",
    .TXT_SWITCH                   = "ganti",
    .TXT_ENABLED                  = "aktif",
    .TXT_DISABLED                 = "nonaktif",
    .TXT_ON                       = "ON",
    .TXT_OFF                      = "OFF",
    .TXT_SCANNING                 = "Memindai...",
    .TXT_CONNECTING               = "Menghubungkan...",
    .TXT_CONNECTED                = "Terhubung",
    .TXT_DISCONNECTED             = "Terputus",
    .TXT_DISCONNECTING            = "Memutus...",
    .TXT_WAITING_CONNECTION       = "Menunggu koneksi",
    .TXT_WORKING                  = "Bekerja",
    .TXT_NOT_CONNECTED            = "Tidak terhubung",
    .TXT_FAILED                   = "Gagal",
    .TXT_SUCCESS                  = "Berhasil",
    .TXT_ERROR                    = "Error",
    .TXT_NA                       = "t/a",
    .TXT_UNNAMED                  = "tanpa nama",

    // Wi-Fi text
    .TXT_WIFI_AP_ENABLED          = "WiFi AP aktif",
    .TXT_WIFI_SSID_NOT_FOUND      = "Jaringan tidak ada",
    .TXT_WIFI_TIMEOUT             = "Waktu habis",
    .TXT_WIFI_WRONG_PASSWORD      = "Kata sandi salah",
    .TXT_WIFI_SYNC_FAILED         = "Sync gagal",
    .TXT_WIFI_TIME_SYNCED         = "Waktu disinkronkan",
    .TXT_WIFI_NOT_CONNECTED       = "Tidak ada WiFi",
    .TXT_WIFI_BF_LOADED           = "Dimuat: %d",
    .TXT_WIFI_BF_DONE             = "Dicoba: %d",
    .TXT_WIFI_BF_ALL_NETS         = "Jaringan: %d",
    .TXT_WIFI_BF_ALL_CRACKED      = "Berhasil: %d/%d",
    .TXT_WIFI_NO_NETWORKS         = "Tidak ada jaringan",
    .TXT_WIFI_EVIL_PORTAL_RUNNING     = "Evil Portal aktif",
    .TXT_WIFI_EVIL_PORTAL_CRED_CAPTURED = "Creds ditangkap!",
    .TXT_WIFI_EVIL_TWIN_RUNNING     = "Evil Twin aktif",
    .TXT_SETTINGS_WIFI_SSID      = "Wi-Fi SSID: ",
    
    .TXT_WIFI_HANDSHAKE_PACKETS   = "Handshake: ",
    .TXT_WIFI_HANDSHAKE_SAVED     = "Disimpan ke SD",
    .TXT_WIFI_NO_SCAN_DATA        = "Pindai jaringan dulu",

    // Bluetooth text
    .TXT_BT_FILE_ERROR            = "Error file",
    .TXT_BT_PRESS_A_TO_RUN        = "Tekan A untuk jalan",
    .TXT_BT_RUNNING               = "Berjalan...",
    .TXT_BT_DONE                  = "Selesai!",

    // USB text
    .TXT_USB_HID_UNSOPPORTED      = "USB HID tidak didukung",
    .TXT_USB_STORAGE_NO_SD        = "Tidak ada kartu SD",
    .TXT_USB_STORAGE_ACTIVE       = "Penyimpanan USB aktif",
    .TXT_USB_STORAGE_HINT         = "Keluarkan untuk keluar",

    // NFC text
    .TXT_NFC_NO_UID_STORED        = "Tidak ada UID",
    .TXT_NFC_READ_TAG_FIRST       = "Baca tag dulu",
    .TXT_NFC_INVALID_UID          = "UID tidak valid",
    .TXT_NFC_UID_4BYTE_REQUIRED   = "UID 4 byte diperlukan",
    .TXT_NFC_WRITE_SUCCESS        = "Penulisan berhasil!",
    .TXT_NFC_UID_WRITTEN          = "UID ditulis ke tag",
    .TXT_NFC_WRITE_FAILED         = "Penulisan gagal!",
    .TXT_NFC_WRITE_ERROR          = "Tidak ada tag/error",
    .TXT_NFC_READY_TO_WRITE       = "Siap menulis...",

    // Settings text
    .TXT_SETTINGS_LANGUAGE        = "Bahasa: ",
    .TXT_SETTINGS_BRIGHTNESS      = "kecerahan: ",
    .TXT_SETTINGS_VOLUME          = "volume: ",
    .TXT_SETTINGS_COLOR           = "warna: ",
    .TXT_SETTINGS_FONT            = "Font:",
    .TXT_SETTINGS_ROTATION        = "rotasi",
    .TXT_SETTINGS_SOUND           = "Suara: ",
    .TXT_SETTINGS_STATUS_BAR      = "bilah status: ",
    .TXT_SETTINGS_PID             = "PID: ",
    .TXT_SETTINGS_KB_GYRO         = "Giroskop: ",
    .TXT_SETTINGS_WEBSERVER_FS    = "WebServer FS: ",
};
