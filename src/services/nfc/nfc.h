#pragma once
// NFC service: tag dumping and saving in the Flipper Zero ".nfc" file format.
//
// Mirrors Flipper's nfc service: callers get a protocol-agnostic `Tag` (what the
// card is + what could be read from it) and a serializer to the Flipper
// "Flipper NFC device" text format (version 4), so saved files open in the Flipper
// app and on the Flipper itself.
//
// Two readers are supported, picked by which begin() the caller uses:
//  - PN532 (I2C): ISO14443-3A, MIFARE Classic Mini/1K/4K (sectors that open with a
//    well-known key), MIFARE Ultralight EV1 / NTAG213/215/216.
//  - RC522 (SPI): reading is UID level only (UID + ATQA + SAK), no dump.
// Both write MIFARE Classic (writeUid / writeClassicSectors).
// Anything else is saved as a plain ISO14443-3A tag (UID + ATQA + SAK).
// isConnected()/readTag()/end() act on the reader started by the last begin().

#include <Arduino.h>

namespace Nfc {

enum class TagType : uint8_t {
	ISO14443_3A, // UID/ATQA/SAK only
	MIFARE_CLASSIC,
	ULTRALIGHT, // MIFARE Ultralight family and NTAG21x
};

struct Tag {
	uint8_t  uid[10] = {0};
	uint8_t  uidLen = 0;
	uint8_t  atqa[2] = {0};  // MSB first, the order Flipper stores it in
	uint8_t  sak = 0;
	TagType  type = TagType::ISO14443_3A;
	const char* subtype = ""; // Flipper type name: "Mini"/"1K"/"4K" or "NTAG215", ...

	// MIFARE Classic
	uint16_t blockCount = 0;
	uint8_t* blocks = nullptr;     // blockCount * 16 bytes
	uint8_t* blockKnown = nullptr; // blockCount flags (0 = unreadable, saved as "??")
	uint8_t  sectorsTotal = 0;
	uint8_t  sectorsRead = 0;

	// Ultralight / NTAG
	uint16_t pagesTotal = 0;
	uint16_t pagesRead = 0;
	uint8_t* pages = nullptr;      // pagesTotal * 4 bytes
	uint8_t  version[8] = {0};     // GET_VERSION reply
	uint8_t  signature[32] = {0};  // READ_SIG reply (zeros when the tag has none)

	Tag() = default;
	Tag(const Tag&) = delete;
	Tag& operator=(const Tag&) = delete;
	~Tag() { release(); }

	/// Frees the dump buffers and resets the tag to an empty state.
	void release();
};

/// Called with 0..100 while a dump is in progress, so the UI can show progress.
typedef void (*ProgressFn)(uint8_t percent);

// Reader lifecycle

enum class Module : uint8_t { PN532, RC522 };

struct Rc522Pins {
	uint8_t ss, sck, mosi, miso, rst;
};

/// Binds I2C to the given pins and wakes the PN532. Returns true when it answers.
bool begin(int sda, int scl);

/// Binds SPI to the given pins and starts the RC522. Returns true when it answers.
bool begin(const Rc522Pins& pins);

/// Releases the reader's bus (the RC522 shares SPI with the SD card) and clears wasConnected().
void end();

/// The reader started by the last begin(), even if that call failed.
Module activeModule();
const char* moduleName(); // "PN532" / "RC522"

/// True when the active reader answers (PN532: ACK on I2C, RC522: version register).
bool isConnected();

/// Tracks whether the reader answered on the previous poll, so callers can tell a
/// connect/disconnect edge apart from "still connected"/"still disconnected" and
/// avoid repeating the same notification every tick.
bool wasConnected();
void setWasConnected(bool connected);

/// The most recently read UID, kept so the write screen can reuse it as the clone
/// target without re-scanning. hasLastUID() is false until readTag() succeeds once.
bool hasLastUID();
void setLastUID(const uint8_t* uid, uint8_t uidLen);
const uint8_t* lastUID();
uint8_t lastUIDLength();

// Reading

/// Looks for a tag and dumps as much of it as possible (the RC522 reads the UID only). Returns false when no tag
/// answers within `timeoutMs`. A tag that is only partly readable still returns true;
/// see `sectorsRead` / `pagesRead`.
bool readTag(Tag& tag, uint16_t timeoutMs = 100, ProgressFn progress = nullptr);

// Formatting

/// "AA:BB:CC:DD" — colon-separated hex, the form used across the NFC screens.
String uidToString(const uint8_t* uid, uint8_t uidLength);

// Flipper ".nfc" format

/// Serializes the tag in the Flipper NFC file format (version 4).
void writeFlipperFile(const Tag& tag, Print& out);

/// Parses a Flipper ".nfc" file. Ultralight/NTAG pages are not parsed (writing to
/// those tags is not supported); UID/ATQA/SAK and Mifare Classic blocks are.
bool parseFlipperFile(Stream& in, Tag& tag);

/// Saves the tag to a new "/nfc_<UID>_N.nfc" file on the default backend (SD if present,
/// else LittleFS). On success `path` holds the file path and `usedLittleFS` the backend.
bool save(const Tag& tag, String& path, bool& usedLittleFS);

// Writing

/// Writes block 0 (UID + BCC + SAK + ATQA) to a magic/CUID Mifare Classic card in the
/// field of the active reader, using `tag.uid`. Requires a 4-byte UID; returns false otherwise or if no
/// magic card answers the block-0 authentication.
bool writeUid(const Tag& tag);

/// Writes every known block of `tag` (as parsed from a ".nfc" file) to a Mifare Classic
/// card in the field, authenticating each sector with the same well-known keys used by
/// readTag(). Unknown ("??") blocks and sector trailers are skipped. Works on any
/// writable Mifare Classic card — unlike writeUid(), the UID itself is not touched.
bool writeClassicSectors(const Tag& tag, ProgressFn progress = nullptr);

} // namespace Nfc
