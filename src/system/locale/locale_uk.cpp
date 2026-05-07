#include "locale.h"

const Locale LANG_UK = {
	// Common menu items
	.MENU_BACK                   = "назад",
	.MENU_SCAN                   = "скан",
	.MENU_RESCAN                 = "рескан",

	// Wi-Fi menu
	.MENU_WIFI                   = "Wi-Fi",
	.MENU_WIFI_INFO              = "інфо",
	.MENU_WIFI_DEAUTH            = "деаут",
	.MENU_WIFI_CONNECT           = "підключити",
	.MENU_WIFI_AP                = "Wi-Fi AP",

	// Bluetooth menu
	.MENU_BT                     = "Bluetooth",
	.MENU_BT_INFO                = "інфо",
	.MENU_BT_FIND                = "пошук",
	.MENU_BT_KEYBOARD            = "клавіатура",
	.MENU_BT_AIR_MOUSE           = "air mouse",
	.MENU_BT_PRESENTER           = "презентер",
	.MENU_BT_SHUTTER             = "затвор",
	.MENU_BT_BAD_BLE             = "bad BLE",
	.MENU_BT_RUN_SCRIPT          = "скрипт",

	// NFC menu
	.MENU_NFC                    = "NFC",
	.MENU_NFC_READ_UID           = "читати UID",
	.MENU_NFC_WRITE_UID          = "записати UID",

	// Settings menu
	.MENU_SETTINGS               = "налаштування",
	.MENU_SETTINGS_TIME          = "час",
	.MENU_SETTINGS_BRIGHTNESS    = "яскравість",
	.MENU_SETTINGS_ROTATION      = "поворот",
	.MENU_SETTINGS_COLORS        = "кольори",
	.MENU_SETTINGS_STATUS_BAR    = "стат. рядок",
	.MENU_SETTINGS_STATUS_BAR_PID = "PID",
	.MENU_SETTINGS_STARTUP_SOUND = "звук запуску",
	.MENU_SETTINGS_FONT          = "шрифт",
	.MENU_SETTINGS_KB_GYRO       = "гіро клав.",
	.MENU_SETTINGS_LANGUAGE      = "мова",

	// Other menu
	.MENU_OTHER                  = "інше",
	.MENU_OTHER_SYSTEM           = "система",
	.MENU_OTHER_CLOCK            = "годинник",
	.MENU_OTHER_SOUND_LEVEL      = "гучність",
	.MENU_OTHER_FLASHLIGHT       = "ліхтарик",
	.MENU_OTHER_FLICKER          = "мигання",
	.MENU_OTHER_LEVEL_TOOL       = "рівень",
	.MENU_OTHER_BATTERY_INFO     = "батарея",

	// System menu
	.MENU_SYSTEM_INFO            = "інфо",
	.MENU_SYSTEM_SHUTDOWN        = "вимкнути",

	// Common text
	.TXT_PRESS_A                 = "натисніть A",
	.TXT_ENABLED                 = "увімкнено",
	.TXT_DISABLED                = "вимкнено",
	.TXT_SCANNING                = "Сканування...",
	.TXT_CONNECTING              = "Підключення...",
	.TXT_CONNECTED               = "Підключено",
	.TXT_DISCONNECTED            = "Відключено",
	.TXT_DISCONNECTING           = "Відключення...",
	.TXT_WAITING_CONNECTION      = "Очікування...",
	.TXT_NOT_CONNECTED           = "Не підключено",
	.TXT_FAILED                  = "Помилка",
	.TXT_NA                      = "н/д",
	.TXT_UNNAMED                 = "без імені",

	// Wi-Fi text
	.TXT_WIFI_AP_ENABLED         = "WiFi AP увімк.",
	.TXT_WIFI_SSID_NOT_FOUND     = "SSID не знайдено",
	.TXT_WIFI_TIMEOUT            = "Таймаут",
	.TXT_WIFI_WRONG_PASSWORD     = "Невірн. пароль",

	// Bluetooth text
	.TXT_BT_FILE_ERROR           = "Помилка файлу",
	.TXT_BT_PRESS_A_TO_RUN       = "A - запустити",
	.TXT_BT_RUNNING              = "Виконання...",
	.TXT_BT_DONE                 = "Готово!",

	// NFC text
	.TXT_NFC_NO_UID_STORED       = "UID не збережено",
	.TXT_NFC_READ_TAG_FIRST      = "Спочатку читай",
	.TXT_NFC_INVALID_UID         = "Невірний UID",
	.TXT_NFC_UID_4BYTE_REQUIRED  = "Потрібен 4-байт.",
	.TXT_NFC_WRITE_SUCCESS       = "Запис успішний!",
	.TXT_NFC_UID_WRITTEN         = "UID записано",
	.TXT_NFC_WRITE_FAILED        = "Запис не вдався",
	.TXT_NFC_WRITE_ERROR         = "Нема тегу/помилка",
	.TXT_NFC_READY_TO_WRITE      = "Готовий до запису...",

	// Settings text
	.TXT_SETTINGS_LANGUAGE       = "Мова: ",
	.TXT_SETTINGS_BRIGHTNESS     = "яскравість: ",
	.TXT_SETTINGS_COLOR          = "колір: ",
	.TXT_SETTINGS_FONT           = "Шрифт:",
	.TXT_SETTINGS_ROTATION       = "поворот",
	.TXT_SETTINGS_SOUND          = "Звук: ",
	.TXT_SETTINGS_STATUS_BAR     = "стат. рядок: ",
	.TXT_SETTINGS_PID            = "PID: ",
	.TXT_SETTINGS_KB_GYRO        = "гіро клав.: ",
};
