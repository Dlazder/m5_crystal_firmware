#pragma once
// RC522 (MFRC522) backend of the NFC service: 13.56 MHz reader over SPI.
// Internal to nfc.cpp, which dispatches to it when begin(Rc522Pins) was used; call
// the Nfc:: functions, not these.
//
// It reads the UID level only (UID + ATQA + SAK), so a tag is reported as plain
// ISO14443-3A. The bus is the global Arduino `SPI`, shared with the SD card and
// CC1101 (never active at the same time) — end() releases it.

#include "nfc.h"

namespace Nfc {
namespace rc522 {

bool begin(const Rc522Pins& pins);
void end();
bool isConnected();
bool readTag(Tag& tag);

// Primitives for MIFARE Classic writing (see Nfc::writeUid / writeClassicSectors)

/// Wakes and selects a tag (also one halted by a failed auth), waiting up to `timeoutMs`.
bool select(uint8_t* uid, uint8_t& uidLen, uint16_t timeoutMs);

/// Authenticates a block of the selected tag. keyType: 0 = key A, 1 = key B.
bool authenticate(uint16_t block, uint8_t keyType, const uint8_t* key);

/// Writes 16 bytes to an authenticated block.
bool writeBlock(uint16_t block, const uint8_t* data);

} // namespace rc522
} // namespace Nfc
