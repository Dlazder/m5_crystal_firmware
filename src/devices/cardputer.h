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

// NFC I2C pins (Cardputer GROVE port — G2=SDA, G1=SCL)
#define NFC_SDA  2
#define NFC_SCL  1

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
    // Wire.begin(SDA, SCL) expands to Wire.begin(8, 9) on the esp32s3 variant.
    // On original Cardputer:  GPIO8/GPIO9 are 74HC138 (keyboard matrix), not I2C.
    //    Using them as I2C breaks keyboard scanning → use GROVE pins (GPIO1/GPIO2).
    // On Cardputer-ADV:       GPIO8/GPIO9 are SYS I2C (where the GROVE port is wired).
    //    Wire.begin(8, 9) is correct there.
    if (DEVICE.getBoard() == m5::board_t::board_M5Cardputer) {
        Wire.begin(2, 1);   // original Cardputer — GROVE I2C on GPIO2(SDA)/GPIO1(SCL)
    } else {
        Wire.begin(SDA, SCL); // Cardputer-ADV and others — SYS I2C GPIO8(SDA)/GPIO9(SCL)
    }
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
