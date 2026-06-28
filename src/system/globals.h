#include <stdint.h>
#include "./processes.h"
#include "./deauth_patch.h"

// Must come before M5GFX/M5Unified so its DataWrapperT<fs::FS> and
// per-filesystem specializations are compiled (see M5GFX.h comment).
#include <SD.h>
#include <LittleFS.h>

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

// Text sizes — scaled by applyFont() to normalize line height across fonts
float TINY_TEXT        = 1.0f;
float SMALL_TEXT       = 1.5f;
float MEDIUM_TEXT      = 2.0f;
float BIG_TEXT         = 3.0f;
float HUGE_TEXT        = 4.0f;
float MENU_TEXT        = 1.8f;
const float STATUS_BAR_TEXT = 1.5f; // always default font scale, not affected by applyFont

// Fonts
#include <U8g2lib.h>
static lgfx::U8g2font u8g2Font6x12Cyrillic(u8g2_font_6x12_t_cyrillic);
static lgfx::U8g2font u8g2FontIbmVga8(u8g2_font_pxplusibmvga8_t_all);
static lgfx::U8g2font u8g2FontIbmVga9(u8g2_font_pxplusibmvga9_t_all);
static lgfx::U8g2font u8g2FontMercutio(u8g2_font_mercutio_basic_nbp_t_all);
static lgfx::U8g2font u8g2FontGlasstown(u8g2_font_glasstown_nbp_t_all);

const lgfx::IFont* systemFonts[] = {
  &u8g2Font6x12Cyrillic,    // 6x12 Cyrillic only (default)
  &u8g2FontMercutio,        // Mercutio ~10px all scripts
  &u8g2FontGlasstown,       // Glasstown ~11px all scripts
  &u8g2FontIbmVga8,         // IBM VGA 8px all scripts
  &u8g2FontIbmVga9,         // IBM VGA 9px all scripts
};
const char* fontNames[] = {
  "default",
  "Mercutio",
  "Glasstown",
  "IBM VGA8",
  "IBM VGA9",
};
const float fontScales[] = {
  1.000f,  // default (6x12_cyrillic): 12px * 1.000 = 12
  0.750f,  // Mercutio:                16px * 0.750 = 12
  0.750f,  // Glasstown:               16px * 0.750 = 12
  0.750f,  // IBM VGA8:                16px * 0.750 = 12
  0.750f,  // IBM VGA9:                16px * 0.750 = 12
};

void applyFont(int index) {
  DISP.setFont(systemFonts[index]);
  canvas.setFont(systemFonts[index]);
  // statusBarCanvas.setFont(systemFonts[index]);
  float s = fontScales[index];
  TINY_TEXT   = 1.0f * s;
  SMALL_TEXT  = 1.5f * s;
  MEDIUM_TEXT = 2.0f * s;
  BIG_TEXT    = 3.0f * s;
  HUGE_TEXT   = 4.0f * s;
  MENU_TEXT   = 1.8f * s;
}

int globalTimer = millis();
int globalPreviousTimer = 0;

bool isWebInterfaceEnabled = false;
bool webServerFs = false;
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

// Native USB HID keyboard (ESP32-S3 only; e.g. Cardputer ADV).
// USBHIDKeyboard.h declares its own `typedef struct {...} KeyReport;`, which is
// byte-identical to the one BleCombo already defined but counts as a conflicting
// declaration. Rename USB's typedef to UsbKeyReport for the duration of the
// include so both can coexist in this translation unit. Both KEY_* macro sets
// are identical standard HID usages, so reusing badUsbResolveKey stays valid.
#ifdef ESP32S3
#define KeyReport UsbKeyReport
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#undef KeyReport
USBHIDKeyboard usbKeyboard;
USBHIDMouse usbMouse;
bool usbHidBegan = false;
#endif

// Device abstraction — must come after BleCombo to avoid KEY_BACKSPACE redefinition by M5Cardputer headers.
#include "../devices/device.h"
Adafruit_PN532 nfc(NFC_SDA, NFC_SCL, &Wire);