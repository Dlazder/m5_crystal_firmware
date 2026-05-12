#pragma once
#include "M5Unified.h"
#include <Wire.h>
#include <time.h>

// Capabilities
#define HAS_RTC            true
#define HAS_PHYSICAL_KB    false
#define HAS_SPEAKER        true

// IMU axis mapping for mouse and level tool
#define IMU_MOUSE_X(ax, ay)           ( (ay) )
#define IMU_MOUSE_Y(ax, ay)           ( (ax) )
#define IMU_LEVEL_ROLL(ax, ay)        ( atan2((ax), (ay)) * 180 / PI )
#define IMU_LEVEL_ANGLE(ax, ay)       ( abs(atan2((ay), (ax)) * 180 / PI) )

// NFC I2C pins
#define NFC_SDA  G32
#define NFC_SCL  G33

// KEY_BACKSPACE value for BLE keyboard
#define BLE_KEY_BACKSPACE  KEY_BACKSPACE

// Init
inline void deviceInit() {
    auto cfg = DEVICE.config();
    DEVICE.begin(cfg);
    Wire.begin(G32, G33);
}

inline void deviceUpdate() {
    DEVICE.update();
}

// Speaker
inline void deviceSpeakerBegin() { /* always on */ }
inline void deviceSpeakerEnd()   { /* always on */ }

// Time
struct DeviceTime { int hours; int minutes; int seconds; };

inline DeviceTime deviceGetTime() {
    DeviceTime t;
    auto dt = DEVICE.Rtc.getDateTime();
    t.hours   = dt.time.hours;
    t.minutes = dt.time.minutes;
    t.seconds = dt.time.seconds;
    return t;
}

inline void deviceSetTime(int hours, int minutes, int seconds) {
    m5::rtc_datetime_t dt = DEVICE.Rtc.getDateTime();
    dt.time.hours   = hours;
    dt.time.minutes = minutes;
    dt.time.seconds = seconds;
    DEVICE.Rtc.setDateTime(&dt);
}
