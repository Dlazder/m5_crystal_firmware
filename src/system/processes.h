#pragma once

// Add new processes here: X(NAME, ID, loopFunction)
#define PROCESS_LIST \
    X(MAIN_MENU,   0,   mainMenuLoop) \
    \
    /* Settings (100-199) */ \
    X(SETTINGS,              100, settingsMenuLoop) \
    X(SETTINGS_CLOCK,        101, settingsClockLoop) \
    X(SETTINGS_STATUS_BAR,   102, settingsStatusBarMenuLoop) \
    X(BRIGHTNESS,            103, brightnessLoop) \
    X(ROTATION,              104, rotationLoop) \
    X(COLORS,                105, colorsLoop) \
    X(FONTS,                 106, fontsLoop) \
    X(STARTUP_SOUND,         107, toggleStartupSound) \
    X(KB_GYRO,               108, toggleKbGyroLoop) \
    X(TOGGLE_STATUS_BAR,     109, toggleStatusBarLoop) \
    X(TOGGLE_STATUS_BAR_PID, 110, toggleStatusBarPidLoop) \
    X(LANGUAGE,              111, languageLoop) \
    X(DIMMING,               112, dimmingLoop) \
    X(VOLUME,                113, volumeLoop) \
    X(WEBSERVER_FS,          114, toggleWebServerFs) \
    X(SETTINGS_SSID,         115, setSsidLoop) \
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
    X(WIFI_TIME_SYNC,    208, wifiTimeSyncLoop) \
    X(WIFI_WPA_BF,       209, wifiBruteforceLoop) \
    X(WIFI_WPA_BF_ALL,   210, wifiBruteforceAllLoop) \
    X(WIFI_DEAUTH_ALL,   211, wifiDeauthAllLoop) \
    X(EVIL_PORTAL,      212, evilPortalLoop) \
    X(EVIL_TWIN,        213, evilTwinLoop) \
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
    X(BT_SNIFFER,       311, bluetoothSnifferLoop) \
    \
    /* NFC (400-499) */ \
    X(NFC,       400, nfcMenuLoop) \
    X(NFC_READ,  401, nfcReadLoop) \
    X(NFC_WRITE, 402, nfcWriteLoop) \
    \
    /* Files (500-599) */ \
    X(FILES_MENU,         500, filesMenuLoop) \
    X(FILE_PICKER,        501, lfsFilePickerLoop) \
    X(FILE_PICKER_SD,     502, filePickerSDLoop) \
    X(SELECTED_FILE_MENU, 503, selectedFileMenuLoop) \
    X(FILE_DELETE,        504, deleteFileLoop) \
    X(FILE_CREATE,        505, createFileLoop) \
    X(FILE_INFO,          506, fileInfoLoop) \
    X(FILE_RENAME,        507, renameFileLoop) \
    X(FILE_VIEW,          508, readFileLoop) \
    X(FILE_EDIT,          509, editFileLoop) \
    \
    /* IR (600-699) */ \
    X(IR,                 600, irMenuLoop) \
    X(IR_READ,            601, irReadLoop) \
    X(IR_SEND,            602, irSendLoop) \
    X(IR_TV_B_GONE,       603, irTvBGoneLoop) \
    \
    X(USB,                700, usbMenuLoop) \
    X(BAD_USB,            701, badUsbLoop) \
    X(USB_MOUSE_JIGGLER,  702, usbMouseJigglerLoop) \
    X(USB_MOUSE,          703, usbMouseLoop) \
    X(USB_KEYBOARD,       704, usbKeyboardLoop) \
    X(USB_STORAGE,        705, usbStorageLoop) \
    /* Other (800-899) */ \
    X(OTHER,              800, otherMenuLoop) \
    X(CLOCK,              801, clockLoop) \
    X(BATTERY,            802, batteryLoop) \
    X(SOUND_LEVEL,        803, soundLevelLoop) \
    X(FLASHLIGHT_MENU,    805, flashLightMenuLoop) \
    X(FLASHLIGHT,         804, flashLightLoop) \
    X(FLASHLIGHT_FLICKER, 806, flickerLoop) \
    X(LEVEL_TOOL,         807, levelToolLoop) \
    X(SYSTEM,             808, systemMenuLoop) \
    X(SYSTEM_INFO,        809, systemInfoLoop) \
    X(SHUTDOWN,           810, shutdownLoop) \
    X(WEATHER,            811, weatherLoop) \

namespace PID {
    #define X(name, id, fn) const int name = id;
    PROCESS_LIST
    #undef X
}
