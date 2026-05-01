struct ProcessEntry {
    int processId;
    void (*loopFunction)();
};

// Add new processes here
const ProcessEntry processEntries[] = {
    {0, mainMenuLoop},
    {1, clockLoop},
    {2, batteryLoop},
    {3, settingsMenuLoop},
    {4, wifiApLoop},
    {5, brightnessLoop},
    {6, rotationLoop},
    {7, colorsLoop},
    {8, wifiScanLoop},
    {9, selectedWifiMenuLoop},
    {10, settingsStatusBarMenuLoop},
    {11, wifiDeauthLoop},
    {12, wifiInfoLoop},
    {13, wifiMenuLoop},
    {14, bluetoothMenuLoop},
    {15, bluetoothShutterLoop},
    {16, toggleStatusBarLoop},
    {17, toggleStatusBarPidLoop},
    {19, fontsLoop},
    {20, levelToolLoop},
    {21, nfcMenuLoop},
    {22, nfcReadLoop},
    {23, nfcWriteLoop},
    {24, flashLightMenuLoop},
    {25, flashLightLoop},
    {26, flickerLoop},
    {27, soundLevelLoop},
    {28, toggleStartupSound},
    {29, otherMenuLoop},
    {30, systemMenuLoop},
    {31, bluetoothMouseLoop},
    {32, bluetoothPresenterLoop},
    {33, shutdownLoop},
    {34, systemInfoLoop},
    {35, settingsClockLoop},
    {36, bluetoothScanLoop},
    {37, bluetoothKeyboardLoop},
    {38, selectedBluetoothMenuLoop},
    {39, bluetoothInfoLoop},
    {40, bluetoothFinderLoop},
    {41, toggleKbGyroLoop},
    {42, badBleMenuLoop},
    {43, badBleLoop}
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