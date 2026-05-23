#include <stdint.h>
#include "./processes.h"
#include "./deauth_patch.h"
#include "M5Unified.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
DNSServer dnsServer;
WebServer webServer(80);
#include <string>
using std::to_string;
#include "version.h"
#include "conf.h"

// Localization
#include "../system/locale/locale.h"

const Locale* locales[] = { &LANG_EN, &LANG_ES, &LANG_IT, &LANG_ID, &LANG_RU, &LANG_UK, &LANG_DE };
const char* localeNames[] = { "English", "Espanol", "Italiano", "Indonesia", "Русский", "Українська", "Deutsch" };
int languageIndex = 0;
int localesCount = sizeof(locales) / sizeof(locales[0]);

// NFC PN532
#include <Adafruit_PN532.h>
#include <Wire.h>



// For storage utils
#include <Preferences.h>
Preferences preferences;

struct MENU {
  int command;
  String name;
};

bool hasImu = false;

int cursor = 0;
int process = PID::MAIN_MENU;
int previousProcess = PID::MAIN_MENU;
bool isSwitching = true;
int rotation = 1;

#define DEVICE M5
#define DISP DEVICE.Display

M5Canvas canvas(&DISP);
M5Canvas statusBarCanvas(&DISP);

uint16_t BGCOLOR=TFT_BLACK;
uint16_t FGCOLOR=TFT_WHITE;
uint16_t colors[] = {TFT_WHITE, TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_CYAN, TFT_BLUE, TFT_VIOLET, TFT_MAGENTA};
const char* colorsEntry[] = {"WHITE", "RED", "ORANGE", "YELLOW", "GREEN", "CYAN", "BLUE", "VIOLET", "MAGENTA"};
int colorIndex = 0;

int TINY_TEXT = 1;
float SMALL_TEXT = 1.5;
int MEDIUM_TEXT = 2;
int BIG_TEXT = 3;
int HUGE_TEXT = 4;

// Fonts
#include <U8g2lib.h>
// static lgfx::U8g2font u8g2Font5x8(u8g2_font_5x8_t_cyrillic);
static lgfx::U8g2font u8g2Font6x12(u8g2_font_6x12_t_cyrillic);

const lgfx::IFont* systemFonts[] = {
  // &fonts::Font0,          // Basic 8x8 ASCII
  // &u8g2Font5x8,           // 5x8 Cyrillic
  &u8g2Font6x12,          // 6x12 Cyrillic
};
const char* fontNames[] = {
  // "Default",
  // "5x8",
  "6x12",
};

int globalTimer = millis();
int globalPreviousTimer = 0;

bool isWebInterfaceEnabled = false;
bool webDataRequested = true;
String webData = "";
String webDataType = "";

// Brightness (stored as a step value 1–10)
int brightness = defaultBrightnessLevel;
static const int BRIGHTNESS_STEPS   = 10;
static const int BRIGHTNESS_HW_MAX  = 255;
static const int BRIGHTNESS_HW_STEP = BRIGHTNESS_HW_MAX / BRIGHTNESS_STEPS;
inline int brightnessToHw(int step) { return step * BRIGHTNESS_HW_STEP; }

// Volume
int volumeMax = 255;
int volumeStep = 10;
int volumeDividor = volumeMax / volumeStep;
int volume = defaultVolumeLevel * volumeDividor;
int volumeMin = 0;

// Screen dimming
int screenDimTimeout = 0;
bool screenIsDimmed = false;
int dimmingPreviousTimer = 0;

// files
String selectedFilePath = "";

// wifi deauth
String ssid;
String mac;
int channel;
uint8_t* bssid;
int rssi;
String wifiPassword;
int wifiScanIndex;


// bluetooth
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BleCombo.h>

#include <BLEAdvertising.h>
#include <esp_gap_ble_api.h>

String bleName;
String bleAddress;
int bleRssi;
String bleManufacturer;
String bleManufacturerRaw;
int8_t bleTxPower = -128;
uint8_t bleAdvFlags = 0xFF;
String bleServiceUUIDs;
String bleAppearance;

BleComboKeyboard bleKeyboard("M5 Crystal", "M5 Crystal");
BleComboMouse bleMouse(&bleKeyboard);

bool bleCompositeBegan = false;

// Device abstraction — must come after BleCombo to avoid KEY_BACKSPACE redefinition
// by M5Cardputer headers.
#include "../devices/device.h"
Adafruit_PN532 nfc(NFC_SDA, NFC_SCL, &Wire);