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
  &fonts::Font0,          // Basic 8x8
  &fonts::lgfxJapanMincho_8,
  &fonts::lgfxJapanMinchoP_8,
  &fonts::lgfxJapanGothic_8,
  &fonts::lgfxJapanGothicP_8,
  // &fonts::Font2,          // 8x16
  // &fonts::Font4,          // 8x16
  // &fonts::Font6,          // 24x32
  // &fonts::Font7,          // 24x48
  // &fonts::Font8,          // 8x16
  // &fonts::AsciiFont8x16,  // Classic 8x16
  // &fonts::AsciiFont24x48  // Classic 24x48
};

const char* fontNames[] = {
  "Default (Font0)",
  "JapanMincho_8",
  "JapanMinchoP_8",
  "JapanGothic_8",
  "JapanGothicP_8"
  // "Font2 (8x16)",
  // "Font4 (8x16 bold)",
  // "Font6 (24x32)",
  // "Font7 (24x48)",
  // "Font8 (8x16 thin)",
  // "Ascii 8x16",
  // "Ascii 24x48"
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