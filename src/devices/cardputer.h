#pragma once
// This file is included from globals.h AFTER BleCombo to avoid KEY_BACKSPACE
// redefinition by M5Cardputer headers.
#include "M5Unified.h"
#include <M5Cardputer.h>
#include <Wire.h>
#include <time.h>

// Capabilities
#define HAS_RTC            false
#define HAS_PHYSICAL_KB    true
#define HAS_SPEAKER        true
#define HAS_SD             true

// SD card SPI pins (Cardputer)
#define SD_CS_PIN   12
#define SD_CLK_PIN  40
#define SD_MOSI_PIN 14
#define SD_MISO_PIN 39

// IMU axis mapping for Cardputer orientation
#define IMU_MOUSE_X(ax, ay)           ( -(ax) )
#define IMU_MOUSE_Y(ax, ay)           ( (ay) )
#define IMU_LEVEL_ROLL(ax, ay)        ( -atan2((ay), (ax)) * 180 / PI )
#define IMU_LEVEL_ANGLE(ax, ay)       ( abs(atan2((ax), (ay)) * 180 / PI) )

// NFC I2C pins (Cardputer uses default SDA/SCL)
#define NFC_SDA  SDA
#define NFC_SCL  SCL

// IR transmitter — built-in LED on G44
#define IR_SEND_PIN 44
// IR receiver — external module on G8 (SDA)
#define IR_RECEIVE_PIN 8

// KEY_BACKSPACE conflicts with M5Cardputer header — use raw HID value
#define BLE_KEY_BACKSPACE  0xB2

// Init
inline void deviceInit() {
    auto cfg = DEVICE.config();
    M5Cardputer.begin(cfg, true);
    Wire.begin(SDA, SCL);
}

// cardputerKbUpdate() is defined in utils/cardputerKbUtils.h, included after this file.
// We declare it here so deviceUpdate can call it.
void cardputerKbUpdate();

inline void deviceUpdate() {
    cardputerKbUpdate();
}

// Speaker
inline void deviceSpeakerBegin() { DEVICE.Speaker.begin(); }
inline void deviceSpeakerEnd()   { DEVICE.Speaker.end(); }

// Time
struct DeviceTime { int hours; int minutes; int seconds; };

inline DeviceTime deviceGetTime() {
    DeviceTime t;
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    struct tm timeinfo;
    localtime_r(&tv.tv_sec, &timeinfo);
    t.hours   = timeinfo.tm_hour;
    t.minutes = timeinfo.tm_min;
    t.seconds = timeinfo.tm_sec;
    return t;
}

inline void deviceSetTime(int hours, int minutes, int seconds) {
    struct timeval tv;
    struct tm timeinfo = {};
    timeinfo.tm_hour = hours;
    timeinfo.tm_min  = minutes;
    timeinfo.tm_sec  = seconds;
    timeinfo.tm_year = 70;
    timeinfo.tm_mday = 1;
    tv.tv_sec  = mktime(&timeinfo);
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);
}
