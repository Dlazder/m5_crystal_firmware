#pragma once
// nRF24L01(+) 2.4 GHz transceiver service.
//
// Minimal register-level driver over the global Arduino `SPI` (shared with the SD
// card, never active at the same time). Only what the scanner needs: probe the
// chip, hop channels and read the received-power-detector (RPD) flag.

#include <Arduino.h>
#include "../../hal/nrfPins.h"

namespace NRF24 {

constexpr uint8_t CHANNEL_COUNT = 126; // channel n = 2400 + n MHz

// Initialises SPI and probes the chip. Returns true when an nRF24 answers.
// Leaves it powered up as a 2 Mbps receiver with CE low (standby).
bool begin(uint8_t csn = NRF_CSN_PIN, uint8_t sck = NRF_SCK_PIN,
					uint8_t mosi = NRF_MOSI_PIN, uint8_t miso = NRF_MISO_PIN,
					uint8_t ce = NRF_CE_PIN);

// Powers the chip down and releases the SPI pins so they can be reused.
void end();

bool isConnected();

// Listens on `channel` (0..125) for ~130 us and returns true if a carrier
// stronger than about -64 dBm was seen (the chip's RPD threshold).
bool carrierOnChannel(uint8_t channel);

} // namespace NRF24
