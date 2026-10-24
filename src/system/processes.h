#pragma once

// Add new processes here: X(NAME, ID, loopFunction)
#define PROCESS_LIST \
    /* Core (0-99) */ \
    X(MAIN_MENU,    0, mainMenuLoop) \
    X(PLACEHOLDER,  1, defaultLoop) \
    X(ERROR_SCREEN, 2, errorLoop) \
    \
    /* Settings (100-199) */ \
    X(SETTINGS,              100, settingsMenuLoop) \
    X(SETTINGS_CLOCK,        101, settingsClockLoop) \
    X(SETTINGS_STATUS_BAR,   102, settingsStatusBarMenuLoop) \
    X(SETTINGS_SSID,         103, setSsidLoop) \
    X(SETTINGS_UI,           104, settingsUIMenuLoop) \
    X(SETTINGS_MENU_ITEMS,   105, settingsMenuItemsLoop) \
    \
    /* Wi-Fi (200-299) */ \
    X(WIFI,              200, wifiMenuLoop) \
    X(WIFI_SCAN,         201, wifiScanLoop) \
    X(WIFI_SELECTED,     202, selectedWifiMenuLoop) \
    X(WIFI_INFO,         203, wifiInfoLoop) \
    X(WIFI_DEAUTH,       204, wifiDeauthLoop) \
    X(WIFI_ACCESS_POINT, 205, wifiApLoop) \
    X(WIFI_CONNECT,      206, wifiConnectLoop) \
    X(WIFI_DISCONNECT,   207, wifiDisconnectLoop) \
    X(WIFI_WPA_BF,       208, wifiBruteforceLoop) \
    X(WIFI_WPA_BF_ALL,   209, wifiBruteforceAllLoop) \
    X(WIFI_DEAUTH_ALL,   210, wifiDeauthAllLoop) \
    X(EVIL_PORTAL,       211, evilPortalLoop) \
    X(EVIL_TWIN,         212, evilTwinLoop) \
    X(WIFI_HANDSHAKE_CAPTURE, 213, wifiHandshakeLoop) \
    X(WIFI_PCAP_TO_HASH, 214, pcapToHashLoop) \
    X(WIFI_PMKID_CAPTURE, 215, wifiPmkidLoop) \
    X(WIFI_CHANNEL_ANALYZER, 216, wifiChannelAnalyzerLoop) \
    X(WEB_SERVER,        217, webServerLoop) \
    X(LFS_WEB_UI,        218, lfsWebUILoop) \
    X(WIFI_WPS_PBC,      219, wifiWpsPbcLoop) \
    \
    /* Bluetooth (300-399) */ \
    X(BLUETOOTH,    300, bluetoothMenuLoop) \
    X(BT_SCAN,      301, bluetoothScanLoop) \
    X(BT_SELECTED,  302, selectedBluetoothMenuLoop) \
    X(BT_INFO,      303, bluetoothInfoLoop) \
    X(BT_FINDER,    304, bluetoothFinderLoop) \
    X(BT_KEYBOARD,  305, bluetoothKeyboardLoop) \
    X(BT_MOUSE,     306, bluetoothMouseLoop) \
    X(BT_PRESENTER, 307, bluetoothPresenterLoop) \
    X(BT_SHUTTER,   308, bluetoothShutterLoop) \
    X(BAD_BLE,      309, badBleLoop) \
    X(BT_MOUSE_JIGGLER, 310, bluetoothMouseJigglerLoop) \
    X(BT_SNIFFER,   311, bluetoothSnifferLoop) \
    \
    /* NFC (400-449) */ \
    X(NFC,               400, nfcMenuLoop) \
    X(NFC_READ,          401, nfcReadLoop) \
    X(NFC_WRITE,         402, nfcWriteLoop) \
    X(NFC_WRITE_UID,     403, nfcWriteUidLoop) \
    X(NFC_WRITE_SECTORS, 404, nfcWriteSectorsLoop) \
    \
    /* RF / sub-GHz CC1101 (450-499) */ \
    X(RF,              450, rfMenuLoop) \
    X(RF_SPECTRUM,     451, cc1101SpectrumAnalyzerLoop) \
    X(RF_RAW_SNIFFER,  452, cc1101RawSnifferLoop) \
    X(RF_CONFIG_PINS,  453, rfConfigurePinsMenuLoop) \
    X(RF_CONFIG_RESET, 454, rfConfigureResetLoop) \
    \
    /* Files (500-599) */ \
    X(FILES_MENU,         500, filesMenuLoop) \
    X(FILE_PICKER,        501, filePickerLFSLoop) \
    X(FILE_PICKER_SD,     502, filePickerSDLoop) \
    X(SELECTED_FILE_MENU, 503, selectedFileMenuLoop) \
    X(FILE_DELETE,        504, deleteFileLoop) \
    X(FILE_CREATE,        505, createFileLoop) \
    X(FILE_INFO,          506, fileInfoLoop) \
    X(FILE_RENAME,        507, renameFileLoop) \
    X(FILE_VIEW,          508, readFileLoop) \
    X(FILE_EDIT,          509, editFileLoop) \
    X(FILE_OPTIONS,       510, fileOptionsLoop) \
    X(FILE_CREATE_DIR,    511, createDirLoop) \
    X(FILE_DELETE_DIR,    512, deleteDirLoop) \
    X(FILE_FOLDER_INFO,   513, folderInfoLoop) \
    X(FILE_RENAME_DIR,    514, renameDirLoop) \
    X(FILE_COPY,          515, copyFileLoop) \
    X(FILE_PASTE,         516, pasteFileLoop) \
    \
    /* IR (600-649) */ \
    X(IR,                 600, irMenuLoop) \
    X(IR_READ,            601, irReadLoop) \
    X(IR_SEND,            602, irSendLoop) \
    X(IR_TV_B_GONE,       603, irTvBGoneLoop) \
    X(IR_CONFIG_PINS,     604, irConfigurePinsMenuLoop) \
    X(IR_CONFIG_RESET,    605, irConfigureResetLoop) \
    \
    /* USB (650-699) */ \
    X(USB,                650, usbMenuLoop) \
    X(BAD_USB,            651, badUsbLoop) \
    X(USB_MOUSE_JIGGLER,  652, usbMouseJigglerLoop) \
    X(USB_MOUSE,          653, usbMouseLoop) \
    X(USB_KEYBOARD,       654, usbKeyboardLoop) \
    X(USB_STORAGE,        655, usbStorageLoop) \
    \
    /* Other (700-799) */ \
    X(OTHER,              700, otherMenuLoop) \
    X(CLOCK,              701, clockLoop) \
    X(BATTERY,            702, batteryLoop) \
    X(SOUND_LEVEL,        703, soundLevelLoop) \
    X(FLASHLIGHT_MENU,    704, flashLightMenuLoop) \
    X(FLASHLIGHT,         705, flashLightLoop) \
    X(FLASHLIGHT_FLICKER, 706, flickerLoop) \
    X(LEVEL_TOOL,         707, levelToolLoop) \
    X(SYSTEM,             708, systemMenuLoop) \
    X(SYSTEM_INFO,        709, systemInfoLoop) \
    X(SHUTDOWN,           710, shutdownLoop) \
    X(WEATHER,            711, weatherLoop) \
    \
    /* UART (800-849) */ \
    X(UART,               800, uartMenuLoop) \
    X(UART_TERMINAL,      801, uartTerminalLoop) \

namespace PID {
    #define X(name, id, fn) const int name = id;
    PROCESS_LIST
    #undef X
}

// Name/ID lookup table, generated from PROCESS_LIST, used by the serial
// "/goto" command to resolve a process by its PID macro name (e.g. "WIFI_SCAN").
struct ProcessTableEntry {
    const char* name;
    int id;
};

const ProcessTableEntry PROCESS_TABLE[] = {
    #define X(name, id, fn) { #name, id },
    PROCESS_LIST
    #undef X
};

const int PROCESS_TABLE_SIZE = sizeof(PROCESS_TABLE) / sizeof(PROCESS_TABLE[0]);
