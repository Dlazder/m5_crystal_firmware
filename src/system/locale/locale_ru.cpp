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
	.MENU_WIFI_SYNC_TIME         = "синхр. время",
	.MENU_WIFI_WPA_BF            = "wpa брутфорс",
	.MENU_WIFI_WPA_BF_ALL        = "wpa бф все сети",
	.MENU_WIFI_DEAUTH_ALL        = "деаут все",
	.MENU_WIFI_EVIL_PORTAL       = "evil portal",
	.MENU_WIFI_EVIL_TWIN         = "evil twin",
	.MENU_SETTINGS_WIFI_SSID      = "Wi-Fi SSID",

	// Bluetooth menu
	.MENU_BT                     = "Bluetooth",
	.MENU_BT_INFO                = "инфо",
	.MENU_BT_FIND                = "поиск",
	.MENU_BT_KEYBOARD            = "клавиатура",
	.MENU_BT_AIR_MOUSE           = "air mouse",
	.MENU_BT_MOUSE_JIGGLER       = "подергиватель мыши",
	.MENU_BT_PRESENTER           = "презентер",
	.MENU_BT_SHUTTER             = "затвор",
	.MENU_BT_BAD_BLE             = "bad BLE",
	.MENU_BT_SNIFFER             = "BLE сниффер",

	// Files menu
	.MENU_FILES                  = "файлы",
	.MENU_FILES_SDCARD           = "SD card",
	.MENU_FILES_DELETE           = "удалить",
	.MENU_FILES_INFO             = "инфо",
	.MENU_FILES_RENAME           = "переим.",
	.MENU_FILES_VIEW             = "просмотр",
	.MENU_FILES_EDIT             = "редактировать",
	.MENU_FILES_LITTLEFS               = "littleFS",

	// NFC menu
	.MENU_NFC                    = "NFC",
	.MENU_NFC_READ_UID           = "читать UID",
	.MENU_NFC_WRITE_UID          = "записать UID",

	// IR menu
	.MENU_IR                     = "IR",
	.MENU_IR_READ                = "читать сигнал",
	.MENU_IR_SEND                = "отправить сигнал",
	.MENU_IR_TV_B_GONE           = "TV-B-Gone",

	// IR text
	.TXT_IR_WAITING              = "Ожидание...",
	.TXT_IR_PRESS_A_SAVE         = "A - сохранить",
	.TXT_IR_PRESS_A_SEND         = "A - отправить",
	.TXT_IR_SAVED                = "Сохранено",
	.TXT_IR_SAVE_ERROR           = "Ошибка записи",
	.TXT_IR_PARSE_ERROR          = "Ошибка файла",
	.TXT_IR_TVB_RUNNING          = "Отправка...",
	.TXT_IR_TVB_DONE             = "Готово!",

	// Settings menu
	.MENU_SETTINGS               = "настройки",
	.MENU_SETTINGS_TIME          = "время",
	.MENU_SETTINGS_BRIGHTNESS    = "яркость",
	.MENU_SETTINGS_VOLUME        = "громкость",
	.MENU_SETTINGS_ROTATION      = "поворот",
	.MENU_SETTINGS_COLORS        = "цвета",
	.MENU_SETTINGS_STATUS_BAR    = "статус бар",
	.MENU_SETTINGS_STATUS_BAR_PID = "PID",
	.MENU_SETTINGS_STARTUP_SOUND = "звук запуска",
	.MENU_SETTINGS_FONT          = "шрифт",
	.MENU_SETTINGS_KB_GYRO       = "гиро клав.",
	.MENU_SETTINGS_LANGUAGE      = "язык",
	.MENU_SETTINGS_DIMMING       = "диммер",
	.MENU_SETTINGS_WEBSERVER_FS  = "веб-сервер FS",

	// Other menu
	.MENU_OTHER                  = "другое",
	.MENU_OTHER_SYSTEM           = "система",
	.MENU_OTHER_CLOCK            = "часы",
	.MENU_OTHER_SOUND_LEVEL      = "громкость",
	.MENU_OTHER_FLASHLIGHT       = "фонарик",
	.MENU_OTHER_FLICKER          = "мигание",
	.MENU_OTHER_LEVEL_TOOL       = "уровень",

	// USB menu
	.MENU_USB                    = "USB",
	.MENU_USB_BAD_USB            = "bad USB",
	.MENU_USB_MOUSE_JIGGLER      = "подергиватель мыши",
	.MENU_USB_MOUSE              = "аэромышь",
	.MENU_USB_KEYBOARD           = "клавиатура",
	.MENU_USB_STORAGE            = "накопитель",

	// System menu
	.MENU_SYSTEM_INFO            = "инфо",
	.MENU_SYSTEM_SHUTDOWN        = "выключить",
	.MENU_SYSTEM_BATTERY_INFO    = "батарея",

	// Common text
	.TXT_PRESS_A                 = "нажмите A",
	.TXT_LEFT                    = "влево",
	.TXT_RIGHT                   = "вправо",
	.TXT_SELECT                  = "выбор",
	.TXT_ADJUST                  = "изменить",
	.TXT_SWITCH                  = "переключить",
	.TXT_ENABLED                 = "включено",
	.TXT_DISABLED                = "выключено",
	.TXT_SCANNING                = "Сканирование...",
	.TXT_CONNECTING              = "Подключение...",
	.TXT_CONNECTED               = "Подключено",
	.TXT_DISCONNECTED            = "Отключено",
	.TXT_DISCONNECTING           = "Отключение...",
	.TXT_WAITING_CONNECTION      = "Ожидание...",
	.TXT_WORKING                 = "Работает",
	.TXT_NOT_CONNECTED           = "Не подключено",
	.TXT_FAILED                  = "Ошибка",
	.TXT_SUCCESS                 = "Успешно",
	.TXT_ERROR                   = "Ошибка",
	.TXT_NA                      = "н/д",
	.TXT_UNNAMED                 = "без имени",

	// Wi-Fi text
	.TXT_WIFI_AP_ENABLED         = "WiFi AP вкл.",
	.TXT_WIFI_SSID_NOT_FOUND     = "SSID не найден",
	.TXT_WIFI_TIMEOUT            = "Таймаут",
	.TXT_WIFI_WRONG_PASSWORD     = "Неверн. пароль",
	.TXT_WIFI_SYNC_FAILED        = "Ошибка синхр.",
	.TXT_WIFI_TIME_SYNCED        = "Время синхр.",
	.TXT_WIFI_NOT_CONNECTED      = "Нет Wi-Fi",
	.TXT_WIFI_BF_LOADED          = "Загружено: %d",
	.TXT_WIFI_BF_DONE            = "Проверено: %d",
	.TXT_WIFI_BF_ALL_NETS        = "Сетей: %d",
	.TXT_WIFI_BF_ALL_CRACKED     = "Взломано: %d/%d",
	.TXT_WIFI_NO_NETWORKS        = "Нет сетей",
	.TXT_WIFI_EVIL_PORTAL_RUNNING     = "Evil Portal активен",
	.TXT_WIFI_EVIL_PORTAL_CRED_CAPTURED = "Креды получены!",
	.TXT_WIFI_EVIL_TWIN_RUNNING     = "Evil Twin активен",
	.TXT_SETTINGS_WIFI_SSID      = "Wi-Fi SSID: ",

	// Bluetooth text
	.TXT_BT_FILE_ERROR           = "Ошибка файла",
	.TXT_BT_PRESS_A_TO_RUN       = "A - запустить",
	.TXT_BT_RUNNING              = "Выполнение...",
	.TXT_BT_DONE                 = "Готово!",

	// USB text
	.TXT_USB_HID_UNSOPPORTED     = "USB HID не поддерживается",
	.TXT_USB_STORAGE_NO_SD       = "Нет SD карты",
	.TXT_USB_STORAGE_ACTIVE      = "USB накопитель активен",
	.TXT_USB_STORAGE_HINT        = "Извлеките для выхода",

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
	.TXT_SETTINGS_VOLUME         = "громкость: ",
	.TXT_SETTINGS_COLOR          = "цвет: ",
	.TXT_SETTINGS_FONT           = "шрифт:",
	.TXT_SETTINGS_ROTATION       = "ориентация экрана",
	.TXT_SETTINGS_SOUND          = "звук: ",
	.TXT_SETTINGS_STATUS_BAR     = "статус бар: ",
	.TXT_SETTINGS_PID            = "PID: ",
	.TXT_SETTINGS_KB_GYRO        = "гиро клав.: ",
	.TXT_SETTINGS_WEBSERVER_FS   = "Веб-сервер FS: ",
};
