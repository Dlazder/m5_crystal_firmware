#pragma once
// CC1101 sub-GHz transceiver service.
//
// Thin lifecycle layer over SmartRC-CC1101-Driver-Lib: owns the SPI bus and pins
// and probes the chip. Radio control itself goes straight through the library's
// global `ELECHOUSE_cc1101` object. The bus is the global Arduino `SPI`, shared
// with the SD card (never active at the same time — SD mounts lazily).

#include <Arduino.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "../../hal/cc1101Pins.h"

namespace CC1101 {

// Initialises SPI and the library. Returns true when a CC1101 answers.
bool begin(uint8_t cs = CC1101_CS_PIN, uint8_t sck = CC1101_SCK_PIN,
					uint8_t mosi = CC1101_MOSI_PIN, uint8_t miso = CC1101_MISO_PIN,
					uint8_t gdo0 = CC1101_GDO0_PIN);

// Idles the radio and releases the SPI pins so they can be reused.
void end();

bool isConnected();

} // namespace CC1101
