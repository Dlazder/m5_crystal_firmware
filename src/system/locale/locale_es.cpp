#include "locale.h"

const Locale LANG_ES = {
	// Common menu items
	.MENU_BACK = "atras",
	.MENU_SCAN = "buscar",
	.MENU_RESCAN = "rebuscar",

	// Wi-Fi menu
	.MENU_WIFI = "Wi-Fi",
	.MENU_WIFI_INFO= "info",
	.MENU_WIFI_DEAUTH= "deauth",
	.MENU_WIFI_CONNECT = "conectar",
	.MENU_WIFI_DISCONNECT = "desconectar wifi",
	.MENU_WIFI_AP= "Wi-Fi AP",
	.MENU_WIFI_WEATHER = "clima",
	.MENU_WIFI_SYNC_TIME = "sincronizar hora",
	.MENU_WIFI_WPA_BF = "wpa fuerza bruta",
	.MENU_WIFI_WPA_BF_ALL = "wpa bf todas redes",
	.MENU_WIFI_DEAUTH_ALL = "deauth todas",
	.MENU_WIFI_EVIL_PORTAL       = "evil portal",
	.MENU_WIFI_EVIL_TWIN         = "evil twin",
	.MENU_WIFI_HANDSHAKE         = "captura handshake",
	.MENU_WIFI_PMKID             = "captura PMKID",
	.MENU_WIFI_CHANNEL_ANALYZER  = "analizador de canales",
	.MENU_SETTINGS_WIFI_SSID      = "Wi-Fi SSID",

	// Bluetooth menu
	.MENU_BT = "Bluetooth",
	.MENU_BT_INFO= "info",
	.MENU_BT_FIND= "buscar",
	.MENU_BT_KEYBOARD= "teclado",
	.MENU_BT_AIR_MOUSE = "raton aire",
	.MENU_BT_MOUSE_JIGGLER = "mouse jiggler",
	.MENU_BT_PRESENTER = "presentador",
	.MENU_BT_SHUTTER = "obturador",
	.MENU_BT_BAD_BLE = "bad BLE",
	.MENU_BT_SNIFFER = "sniffer BLE",

	// Files menu
	.MENU_FILES = "archivos",
	.MENU_FILES_SDCARD = "SD card",
	.MENU_FILES_DELETE = "eliminar",
	.MENU_FILES_INFO = "info",
	.MENU_FILES_RENAME = "renombrar",
	.MENU_FILES_VIEW   = "ver",
	.MENU_FILES_EDIT   = "editar",
	.MENU_FILES_LITTLEFS = "littleFS",

	// NFC menu
	.MENU_NFC= "NFC",
	.MENU_NFC_READ_UID = "leer UID",
	.MENU_NFC_WRITE_UID = "escribir UID",
	.MENU_NFC_ADVANCED_INFO = "info avanzada",

	// IR menu
	.MENU_IR                     = "IR",
	.MENU_IR_READ                = "leer señal",
	.MENU_IR_SEND                = "enviar señal",
	.MENU_IR_TV_B_GONE           = "TV-B-Gone",

	// IR text
	.TXT_IR_WAITING              = "Esperando...",
	.TXT_IR_PRESS_A_SAVE         = "A para guardar",
	.TXT_IR_PRESS_A_SEND         = "A para enviar",
	.TXT_IR_SAVED                = "Guardado",
	.TXT_IR_SAVE_ERROR           = "Error al guardar",
	.TXT_IR_PARSE_ERROR          = "Error de archivo",
	.TXT_IR_TVB_RUNNING          = "Enviando...",
	.TXT_IR_TVB_DONE             = "Hecho!",

	// Settings menu
	.MENU_SETTINGS = "ajustes",
	.MENU_SETTINGS_TIME = "hora",
	.MENU_SETTINGS_BRIGHTNESS = "brillo",
	.MENU_SETTINGS_VOLUME     = "volumen",
	.MENU_SETTINGS_ROTATION = "rotacion",
	.MENU_SETTINGS_COLORS = "colores",
	.MENU_SETTINGS_STATUS_BAR = "barra estado",
	.MENU_SETTINGS_STATUS_BAR_PID = "PID",
	.MENU_SETTINGS_STARTUP_SOUND = "sonido inicio",
	.MENU_SETTINGS_FONT = "fuente",
	.MENU_SETTINGS_KB_GYRO = "giroscopio",
	.MENU_SETTINGS_LANGUAGE = "idioma",
	.MENU_SETTINGS_DIMMING = "atenuar",
	.MENU_SETTINGS_WEBSERVER_FS = "servidor web FS",

	// Other menu
	.MENU_OTHER = "otros",
	.MENU_OTHER_SYSTEM = "sistema",
	.MENU_OTHER_CLOCK = "reloj",
	.MENU_OTHER_SOUND_LEVEL = "nivel sonido",
	.MENU_OTHER_FLASHLIGHT = "linterna",
	.MENU_OTHER_FLICKER = "parpadeo",
	.MENU_OTHER_LEVEL_TOOL = "nivel",

	// USB menu
	.MENU_USB = "USB",
	.MENU_USB_BAD_USB = "bad USB",
	.MENU_USB_MOUSE_JIGGLER = "mouse jiggler",
	.MENU_USB_MOUSE = "raton aire",
	.MENU_USB_KEYBOARD = "teclado",
	.MENU_USB_STORAGE = "almacenamiento",

	// System menu
	.MENU_SYSTEM_INFO = "info",
	.MENU_SYSTEM_SHUTDOWN = "apagar",
	.MENU_SYSTEM_BATTERY_INFO = "bateria",

	// Common text
	.TXT_PRESS_A = "pulsa A",
	.TXT_LEFT    = "izquierda",
	.TXT_RIGHT   = "derecha",
	.TXT_SELECT  = "seleccionar",
	.TXT_ADJUST  = "ajustar",
	.TXT_SWITCH  = "cambiar",
	.TXT_ENABLED = "activado",
	.TXT_DISABLED = "desactivado",
	.TXT_ON      = "ON",
	.TXT_OFF     = "OFF",
	.TXT_SCANNING = "Buscando...",
	.TXT_CONNECTING = "Conectando...",
	.TXT_CONNECTED = "Conectado",
	.TXT_DISCONNECTED = "Desconectado",
	.TXT_DISCONNECTING = "Desconectando...",
	.TXT_WAITING_CONNECTION = "Esperando conexion",
	.TXT_WORKING = "Trabajando",
	.TXT_NOT_CONNECTED = "No conectado",
	.TXT_FAILED = "Error",
	.TXT_SUCCESS = "Exito",
	.TXT_ERROR = "Error",
	.TXT_NA = "n/d",
	.TXT_UNNAMED = "sin nombre",

	// Wi-Fi text
	.TXT_WIFI_AP_ENABLED = "WiFi AP activado",
	.TXT_WIFI_SSID_NOT_FOUND = "Red no encontrada",
	.TXT_WIFI_TIMEOUT= "Tiempo agotado",
	.TXT_WIFI_WRONG_PASSWORD = "Contrasena incorrecta",
	.TXT_WIFI_SYNC_FAILED = "Sync fallido",
	.TXT_WIFI_TIME_SYNCED = "Hora sincronizada",
	.TXT_WIFI_NOT_CONNECTED = "Sin WiFi",
	.TXT_WIFI_BF_LOADED = "Cargado: %d",
	.TXT_WIFI_BF_DONE = "Intentado: %d",
	.TXT_WIFI_BF_ALL_NETS = "Redes: %d",
	.TXT_WIFI_BF_ALL_CRACKED = "Crackeado: %d/%d",
	.TXT_WIFI_NO_NETWORKS = "Sin redes",
	.TXT_WIFI_EVIL_PORTAL_RUNNING     = "Evil Portal activo",
	.TXT_WIFI_EVIL_PORTAL_CRED_CAPTURED = "Creds capturadas!",
	.TXT_WIFI_EVIL_TWIN_RUNNING     = "Evil Twin activo",
	.TXT_SETTINGS_WIFI_SSID      = "Wi-Fi SSID: ",
	
	.TXT_WIFI_HANDSHAKE_PACKETS   = "Handshakes: ",
	.TXT_WIFI_HANDSHAKE_SAVED     = "Guardado en SD",
	.TXT_WIFI_NO_SCAN_DATA        = "Escanea redes primero",

	// Bluetooth text
	.TXT_BT_FILE_ERROR = "Error de archivo",
	.TXT_BT_PRESS_A_TO_RUN= "Pulsa A para ejecutar",
	.TXT_BT_RUNNING= "Ejecutando...",
	.TXT_BT_DONE = "Hecho!",

	// USB text
	.TXT_USB_HID_UNSOPPORTED = "USB HID no soportado",
	.TXT_USB_STORAGE_NO_SD = "Sin tarjeta SD",
	.TXT_USB_STORAGE_ACTIVE = "Almacenamiento USB activo",
	.TXT_USB_STORAGE_HINT = "Expulsar para salir",

	// NFC text
	.TXT_NFC_NO_UID_STORED = "Sin UID guardado",
	.TXT_NFC_READ_TAG_FIRST= "Lee una etiqueta primero",
	.TXT_NFC_INVALID_UID = "UID invalido",
	.TXT_NFC_UID_4BYTE_REQUIRED= "Se requiere UID de 4 bytes",
	.TXT_NFC_WRITE_SUCCESS = "Escritura exitosa!",
	.TXT_NFC_UID_WRITTEN = "UID escrito en etiqueta",
	.TXT_NFC_WRITE_FAILED= "Escritura fallida!",
	.TXT_NFC_WRITE_ERROR = "Sin etiqueta o error",
	.TXT_NFC_READY_TO_WRITE= "Listo para escribir...",

	// Settings text
	.TXT_SETTINGS_LANGUAGE = "Idioma: ",
	.TXT_SETTINGS_BRIGHTNESS = "brillo: ",
	.TXT_SETTINGS_VOLUME     = "volumen: ",
	.TXT_SETTINGS_COLOR= "color: ",
	.TXT_SETTINGS_FONT = "Fuente:",
	.TXT_SETTINGS_ROTATION = "rotacion",
	.TXT_SETTINGS_SOUND= "Sonido: ",
	.TXT_SETTINGS_STATUS_BAR = "barra estado: ",
	.TXT_SETTINGS_PID= "PID: ",
	.TXT_SETTINGS_KB_GYRO= "Giroscopio: ",
	.TXT_SETTINGS_WEBSERVER_FS = "WebServer FS: ",
};
