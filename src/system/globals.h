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

// Localization
#include "../system/locale/locale.h"

const Locale* locales[] = { &LANG_EN, &LANG_ES, &LANG_IT, &LANG_ID };
const char* localeNames[] = { "English", "Espanol", "Italiano", "Indonesia" };
int languageIndex = 0;
int localesCount = sizeof(locales) / sizeof(locales[0]);

// NFC PN532
#include <Adafruit_PN532.h>
#include <Wire.h>
Adafruit_PN532 nfc(G32, G33, &Wire);


#include "conf.h"
#include "version.h"

// For storage utils
#include <Preferences.h>
Preferences preferences;

struct MENU {
  int command;
  String name;
};

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

float TINY_TEXT = 1.5;
int SMALL_TEXT = 2;
int MEDIUM_TEXT = 3;
int BIG_TEXT = 4;

int currentFontIndex = 0;

const lgfx::IFont* systemFonts[] = {
  &fonts::Font0,          // Basic 8x8 ASCII
  // &u8g2FontUnicode,       // Terminus 11px Unicode (Cyrillic, Latin Extended)
  &fonts::Font2,          // 8x16
};

const char* fontNames[] = {
  "Default (Font0)",
  // "Terminus 11px",
  "Font2 (8x16)",
};

int brightnessMax = 255;
int brightnessStep = 10;
int brightnessDividor = brightnessMax / brightnessStep;
int brightness = 1*brightnessDividor;
int brightnessMin = brightnessDividor;

int globalTimer = millis();
int globalPreviousTimer = 0;

int isWebInterfaceEnabled = false;


bool webDataRequested = true;
String webData = "";
String webDataType = "";

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