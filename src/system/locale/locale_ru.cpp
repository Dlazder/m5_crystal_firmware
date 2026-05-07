#include "locale.h"

const Locale LANG_RU = {
	// Common menu items
	.MENU_BACK                   = "назад",
	.MENU_SCAN                   = "сканировать",
	.MENU_RESCAN                 = "сканировать",

	// Wi-Fi menu
	.MENU_WIFI                   = "Wi-Fi",
	.MENU_WIFI_INFO              = "инфо",
	.MENU_WIFI_DEAUTH            = "деаутентификация",
	.MENU_WIFI_CONNECT           = "подключить",
	.MENU_WIFI_DISCONNECT        = "отключить wifi",
	.MENU_WIFI_AP                = "Wi-Fi AP",
	.MENU_WIFI_WEATHER           = "погода",

	// Bluetooth menu
	.MENU_BT                     = "Bluetooth",
	.MENU_BT_INFO                = "инфо",
	.MENU_BT_FIND                = "поиск",
	.MENU_BT_KEYBOARD            = "клавиатура",
	.MENU_BT_AIR_MOUSE           = "air mouse",
	.MENU_BT_PRESENTER           = "презентер",
	.MENU_BT_SHUTTER             = "затвор",
	.MENU_BT_BAD_BLE             = "bad BLE",
	.MENU_BT_RUN_SCRIPT          = "запуск",

	// NFC menu
	.MENU_NFC                    = "NFC",
	.MENU_NFC_READ_UID           = "читать UID",
	.MENU_NFC_WRITE_UID          = "записать UID",

	// Settings menu
	.MENU_SETTINGS               = "настройки",
	.MENU_SETTINGS_TIME          = "время",
	.MENU_SETTINGS_BRIGHTNESS    = "яркость",
	.MENU_SETTINGS_ROTATION      = "поворот",
	.MENU_SETTINGS_COLORS        = "цвета",
	.MENU_SETTINGS_STATUS_BAR    = "статус бар",
	.MENU_SETTINGS_STATUS_BAR_PID = "PID",
	.MENU_SETTINGS_STARTUP_SOUND = "звук запуска",
	.MENU_SETTINGS_FONT          = "шрифт",
	.MENU_SETTINGS_KB_GYRO       = "гиро клав.",
	.MENU_SETTINGS_LANGUAGE      = "язык",

	// Other menu
	.MENU_OTHER                  = "другое",
	.MENU_OTHER_SYSTEM           = "система",
	.MENU_OTHER_CLOCK            = "часы",
	.MENU_OTHER_SOUND_LEVEL      = "громкость",
	.MENU_OTHER_FLASHLIGHT       = "фонарик",
	.MENU_OTHER_FLICKER          = "мигание",
	.MENU_OTHER_LEVEL_TOOL       = "уровень",

	// System menu
	.MENU_SYSTEM_INFO            = "инфо",
	.MENU_SYSTEM_SHUTDOWN        = "выключить",
	.MENU_SYSTEM_BATTERY_INFO    = "батарея",

	// Common text
	.TXT_PRESS_A                 = "нажмите A",
	.TXT_ENABLED                 = "включено",
	.TXT_DISABLED                = "выключено",
	.TXT_SCANNING                = "Сканирование...",
	.TXT_CONNECTING              = "Подключение...",
	.TXT_CONNECTED               = "Подключено",
	.TXT_DISCONNECTED            = "Отключено",
	.TXT_DISCONNECTING           = "Отключение...",
	.TXT_WAITING_CONNECTION      = "Ожидание...",
	.TXT_NOT_CONNECTED           = "Не подключено",
	.TXT_FAILED                  = "Ошибка",
	.TXT_NA                      = "н/д",
	.TXT_UNNAMED                 = "без имени",

	// Wi-Fi text
	.TXT_WIFI_AP_ENABLED         = "WiFi AP вкл.",
	.TXT_WIFI_SSID_NOT_FOUND     = "SSID не найден",
	.TXT_WIFI_TIMEOUT            = "Таймаут",
	.TXT_WIFI_WRONG_PASSWORD     = "Неверн. пароль",

	// Bluetooth text
	.TXT_BT_FILE_ERROR           = "Ошибка файла",
	.TXT_BT_PRESS_A_TO_RUN       = "A - запустить",
	.TXT_BT_RUNNING              = "Выполнение...",
	.TXT_BT_DONE                 = "Готово!",

	// NFC text
	.TXT_NFC_NO_UID_STORED       = "UID не сохранён",
	.TXT_NFC_READ_TAG_FIRST      = "Сначала прочитай тег",
	.TXT_NFC_INVALID_UID         = "Неверный UID",
	.TXT_NFC_UID_4BYTE_REQUIRED  = "Нужен 4-байт.",
	.TXT_NFC_WRITE_SUCCESS       = "Запись успешна!",
	.TXT_NFC_UID_WRITTEN         = "UID записан",
	.TXT_NFC_WRITE_FAILED        = "Запись не удал.",
	.TXT_NFC_WRITE_ERROR         = "Нет тега/ошибка",
	.TXT_NFC_READY_TO_WRITE      = "Готов к записи...",

	// Settings text
	.TXT_SETTINGS_LANGUAGE       = "язык: ",
	.TXT_SETTINGS_BRIGHTNESS     = "яркость: ",
	.TXT_SETTINGS_COLOR          = "цвет: ",
	.TXT_SETTINGS_FONT           = "шрифт:",
	.TXT_SETTINGS_ROTATION       = "ориентация экрана",
	.TXT_SETTINGS_SOUND          = "звук: ",
	.TXT_SETTINGS_STATUS_BAR     = "статус бар: ",
	.TXT_SETTINGS_PID            = "PID: ",
	.TXT_SETTINGS_KB_GYRO        = "гиро клав.: ",
};
