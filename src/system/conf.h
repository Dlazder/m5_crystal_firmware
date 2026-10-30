// set to false for disable status bar
bool statusBar = true;
// set to false to hide current PID in statusBar
bool statusBarPid = false;
// set to false to hide seconds in statusBar
bool statusBarSeconds = true;


// Wi-Fi access point name
String wifiSsid = "M5 Crystal";
// Wi-Fi access point password
// Leave the blank if you do not need a password
String apWifiPassword = "";

// Set to true to enable the startup sound
bool startupSound = false;
// keyboard gyro mode
bool kbGyroEnabled = true;

// Current color index
int colorIndex = 2;

int currentFontIndex = 0;

// Default brightness level from 0 to 10
int defaultBrightnessLevel = 3;

// Default volume level from 0 to 10
int defaultVolumeLevel = 4;

// Default screen dim timeout index (0 = OFF)
int defaultDimTimeoutIndex = 1;

bool iconsEnabled = true;

// Visibility of these sections on the main menu
bool showMenuWifi = true;
bool showMenuBt = true;
bool showMenuNfc = true;
bool showMenuIr = true;
bool showMenuUsb = true;
bool showMenuRf = false;
bool showMenuNrf = true;
bool showMenuUart = true;
bool showMenuFiles = true;
bool showMenuOther = true;

// Timezone offset from UTC in seconds (default 0 = UTC, e.g. 10800 = UTC+3)
int timezoneOffset = 0;


// false = mount SD at "/", true = mount SD at "/crystal"
bool sdMountCrystal = true;