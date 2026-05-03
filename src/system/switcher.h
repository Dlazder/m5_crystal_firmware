struct ProcessEntry {
    int processId;
    void (*loopFunction)();
};

// Add new processes here
const ProcessEntry processEntries[] = {
    {PID::MAIN_MENU,   mainMenuLoop},
    {PID::FILE_PICKER, lfsFilePickerLoop},

    // Settings
    {PID::SETTINGS,              settingsMenuLoop},
    {PID::SETTINGS_CLOCK,        settingsClockLoop},
    {PID::SETTINGS_STATUS_BAR,   settingsStatusBarMenuLoop},
    {PID::BRIGHTNESS,            brightnessLoop},
    {PID::ROTATION,              rotationLoop},
    {PID::COLORS,                colorsLoop},
    {PID::FONTS,                 fontsLoop},
    {PID::STARTUP_SOUND,         toggleStartupSound},
    {PID::KB_GYRO,               toggleKbGyroLoop},
    {PID::TOGGLE_STATUS_BAR,     toggleStatusBarLoop},
    {PID::TOGGLE_STATUS_BAR_PID, toggleStatusBarPidLoop},

    // Wi-Fi
    {PID::WIFI,          wifiMenuLoop},
    {PID::WIFI_SCAN,     wifiScanLoop},
    {PID::WIFI_SELECTED, selectedWifiMenuLoop},
    {PID::WIFI_INFO,     wifiInfoLoop},
    {PID::WIFI_DEAUTH,   wifiDeauthLoop},
    {PID::WIFI_ACCESS_POINT, wifiApLoop},
    {PID::WIFI_CONNECT,      wifiConnectLoop},

    // Bluetooth
    {PID::BLUETOOTH,    bluetoothMenuLoop},
    {PID::BT_SCAN,      bluetoothScanLoop},
    {PID::BT_SELECTED,  selectedBluetoothMenuLoop},
    {PID::BT_INFO,      bluetoothInfoLoop},
    {PID::BT_FINDER,    bluetoothFinderLoop},
    {PID::BT_KEYBOARD,  bluetoothKeyboardLoop},
    {PID::BT_MOUSE,     bluetoothMouseLoop},
    {PID::BT_PRESENTER, bluetoothPresenterLoop},
    {PID::BT_SHUTTER,   bluetoothShutterLoop},
    {PID::BAD_BLE,      badBleLoop},
    {PID::BAD_BLE_MENU, badBleMenuLoop},

    // NFC
    {PID::NFC,       nfcMenuLoop},
    {PID::NFC_READ,  nfcReadLoop},
    {PID::NFC_WRITE, nfcWriteLoop},

    // Other
    {PID::OTHER,              otherMenuLoop},
    {PID::CLOCK,              clockLoop},
    {PID::BATTERY,            batteryLoop},
    {PID::SOUND_LEVEL,        soundLevelLoop},
    {PID::FLASHLIGHT_MENU,    flashLightMenuLoop},
    {PID::FLASHLIGHT,         flashLightLoop},
    {PID::FLASHLIGHT_FLICKER, flickerLoop},
    {PID::LEVEL_TOOL,         levelToolLoop},
    {PID::SYSTEM,             systemMenuLoop},
    {PID::SYSTEM_INFO,        systemInfoLoop},
    {PID::SHUTDOWN,           shutdownLoop},
};

void switcher() {
    for (const ProcessEntry& entry : processEntries) {
        if (entry.processId == process) {
            entry.loopFunction();
            return;
        }
    }
    defaultLoop();
}
