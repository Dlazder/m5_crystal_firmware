#include "nfc.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "nfcRc522.h"
#include "../storage/storage.h"

// The library's shared packet buffer (Adafruit_PN532.cpp).
extern byte pn532_packetbuffer[];

namespace Nfc {

static const uint8_t PN532_I2C_ADDR = 0x24;

// Keys tried on every MIFARE Classic sector (transport / MAD / NFC Forum defaults).
static const uint8_t KEYS[][6] = {
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5},
	{0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5},
	{0x4D, 0x3A, 0x99, 0xC3, 0x51, 0xDD},
	{0x1A, 0x98, 0x2C, 0x7E, 0x45, 0x9A},
	{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},
};
static const uint8_t KEY_COUNT = sizeof(KEYS) / sizeof(KEYS[0]);

void Tag::release() {
	free(blocks);
	free(blockKnown);
	free(pages);
	blocks = nullptr;
	blockKnown = nullptr;
	pages = nullptr;
	uidLen = 0;
	type = TagType::ISO14443_3A;
	subtype = "";
	blockCount = pagesTotal = pagesRead = 0;
	sectorsTotal = sectorsRead = 0;
}

// Lifecycle

static Module _active = Module::PN532;

// Created by begin(sda, scl) so the pins can follow the settings.
static Adafruit_PN532* pn532 = nullptr;
static int pn532Sda = -1, pn532Scl = -1;

static bool pn532IsConnected() {
	Wire.beginTransmission(PN532_I2C_ADDR);
	return Wire.endTransmission() == 0;
}

bool begin(int sda, int scl) {
	_active = Module::PN532;
	// The library's I2C constructor takes (irq, reset) and configures those two pins, so,
	// as it always has, it is given the bus pins; build it before the bus is rebound below.
	if (!pn532 || sda != pn532Sda || scl != pn532Scl) {
		delete pn532;
		pn532 = new Adafruit_PN532(sda, scl, &Wire);
		pn532Sda = sda;
		pn532Scl = scl;
	}
	// End any existing I2C session (e.g. the internal bus brought up by device init) so
	// the bus is re-bound to the NFC pins; on ESP32 Arduino core 2.x a repeated
	// Wire.begin() with new pins is ignored unless the bus is released first.
	Wire.end();
	Wire.begin(sda, scl);
	if (!pn532IsConnected()) return false;
	pn532->begin();
	if (!pn532->getFirmwareVersion()) return false;
	pn532->SAMConfig();
	return true;
}

bool begin(const Rc522Pins& pins) {
	_active = Module::RC522;
	return rc522::begin(pins);
}

static bool _wasConnected = false;

void end() {
	if (_active == Module::RC522) rc522::end();
	_wasConnected = false;
}

Module activeModule() { return _active; }
const char* moduleName() { return _active == Module::RC522 ? "RC522" : "PN532"; }

bool isConnected() {
	return _active == Module::RC522 ? rc522::isConnected() : pn532IsConnected();
}

bool wasConnected() { return _wasConnected; }
void setWasConnected(bool connected) { _wasConnected = connected; }

static uint8_t _lastUID[10] = {0};
static uint8_t _lastUIDLen = 0;

bool hasLastUID() { return _lastUIDLen > 0; }

void setLastUID(const uint8_t* uid, uint8_t uidLen) {
	_lastUIDLen = min(uidLen, (uint8_t)sizeof(_lastUID));
	memcpy(_lastUID, uid, _lastUIDLen);
}

const uint8_t* lastUID() { return _lastUID; }
uint8_t lastUIDLength() { return _lastUIDLen; }

// Low-level helpers

/// Finds a tag in the field and fills uid/atqa/sak. ATQA and SAK are not returned by
/// the library's API, but it leaves the target frame in its packet buffer:
/// b9..10 = SENS_RES (ATQA, MSB first), b11 = SEL_RES (SAK).
static bool selectTag(uint8_t* uid, uint8_t& uidLen, uint8_t* atqa, uint8_t* sak, uint16_t timeoutMs) {
	if (!pn532->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, timeoutMs)) return false;
	if (atqa) {
		atqa[0] = pn532_packetbuffer[9];
		atqa[1] = pn532_packetbuffer[10];
	}
	if (sak) *sak = pn532_packetbuffer[11];
	return true;
}

// Tag access primitives of the active reader, used by the writers and reselect().

static bool tagSelect(uint8_t* uid, uint8_t& uidLen, uint16_t timeoutMs) {
	if (_active == Module::RC522) return rc522::select(uid, uidLen, timeoutMs);
	return selectTag(uid, uidLen, nullptr, nullptr, timeoutMs);
}

static bool tagAuthenticate(const uint8_t* uid, uint8_t uidLen, uint16_t block, uint8_t keyType, const uint8_t* key) {
	if (_active == Module::RC522) return rc522::authenticate(block, keyType, key);
	uint8_t keyCopy[6];
	memcpy(keyCopy, key, sizeof(keyCopy)); // the library takes a non-const key
	return pn532->mifareclassic_AuthenticateBlock((uint8_t*)uid, uidLen, block, keyType, keyCopy);
}

static bool tagWriteBlock(uint16_t block, const uint8_t* data) {
	if (_active == Module::RC522) return rc522::writeBlock(block, data);
	uint8_t dataCopy[16];
	memcpy(dataCopy, data, sizeof(dataCopy));
	return pn532->mifareclassic_WriteDataBlock(block, dataCopy);
}

/// Re-selects the same tag after a failed auth/read dropped it out of the active state.
static bool reselect(const Tag& tag) {
	uint8_t uid[10];
	uint8_t len = 0;
	if (!tagSelect(uid, len, 100)) return false;
	return len == tag.uidLen && memcmp(uid, tag.uid, len) == 0;
}

/// Sends a raw frame with InCommunicateThru (0x42); needed for commands such as
/// GET_VERSION (0x60), which InDataExchange would treat as a MIFARE Classic auth.
/// The library does not expose the response reader, so the reply is read over I2C here.
static bool commThru(const uint8_t* tx, uint8_t txLen, uint8_t* rx, uint8_t& rxLen) {
	pn532_packetbuffer[0] = 0x42;
	memcpy(pn532_packetbuffer + 1, tx, txLen);
	if (!pn532->sendCommandCheckAck(pn532_packetbuffer, txLen + 1, 100)) return false;

	// Wait for the "ready" status byte
	uint32_t start = millis();
	bool ready = false;
	while (millis() - start < 100) {
		if (Wire.requestFrom(PN532_I2C_ADDR, (uint8_t)1) == 1 && (Wire.read() & 1)) { ready = true; break; }
		delay(2);
	}
	if (!ready) return false;

	// Frame: status | 00 00 FF LEN LCS D5 43 STATUS DATA... DCS 00
	uint8_t frame[48];
	uint8_t want = sizeof(frame);
	uint8_t got = Wire.requestFrom(PN532_I2C_ADDR, (uint8_t)(want + 1));
	if (got < 9) return false;
	Wire.read(); // status byte
	for (uint8_t i = 0; i < got - 1 && i < want; i++) frame[i] = Wire.read();

	if (frame[0] != 0x00 || frame[1] != 0x00 || frame[2] != 0xFF) return false;
	uint8_t len = frame[3];
	if ((uint8_t)(frame[3] + frame[4]) != 0 || frame[5] != 0xD5 || frame[6] != 0x43) return false;
	if (frame[7] != 0x00) return false; // PN532 reported an RF error (tag NAK / timeout)
	if (len < 3 || 8 + (len - 3) > (int)sizeof(frame)) return false;

	uint8_t dataLen = len - 3;
	if (dataLen > rxLen) dataLen = rxLen;
	memcpy(rx, frame + 8, dataLen);
	rxLen = dataLen;
	return true;
}

static void progress(ProgressFn fn, uint32_t done, uint32_t total) {
	if (fn && total) fn((uint8_t)(done * 100 / total));
}

// MIFARE Classic

static uint8_t classicSectors(uint16_t blocks) {
	return blocks == 256 ? 40 : (blocks == 64 ? 16 : 5);
}

static uint16_t sectorFirstBlock(uint8_t s) {
	return s < 32 ? s * 4 : 128 + (s - 32) * 16;
}

static uint8_t sectorBlocks(uint8_t s) {
	return s < 32 ? 4 : 16;
}

static bool dumpClassic(Tag& tag, ProgressFn onProgress) {
	tag.blocks = (uint8_t*)calloc(tag.blockCount, 16);
	tag.blockKnown = (uint8_t*)calloc(tag.blockCount, 1);
	if (!tag.blocks || !tag.blockKnown) return false;
	tag.sectorsTotal = classicSectors(tag.blockCount);

	uint8_t lastKey = 0; // cards usually share one key between sectors: try it first
	for (uint8_t s = 0; s < tag.sectorsTotal; s++) {
		uint16_t first = sectorFirstBlock(s);
		uint8_t count = sectorBlocks(s);
		uint16_t trailer = first + count - 1;

		bool opened = false;
		uint8_t usedKey = 0, usedType = 0; // type: 0 = key A, 1 = key B
		for (uint8_t kt = 0; kt < 2 && !opened; kt++) {
			for (uint8_t n = 0; n < KEY_COUNT && !opened; n++) {
				uint8_t k = (n + lastKey) % KEY_COUNT;
				uint8_t key[6];
				memcpy(key, KEYS[k], 6);
				if (pn532->mifareclassic_AuthenticateBlock(tag.uid, tag.uidLen, first, kt, key)) {
					opened = true;
					usedKey = k;
					usedType = kt;
				} else if (!reselect(tag)) {
					return s > 0; // tag left the field
				}
			}
		}

		if (opened) {
			lastKey = usedKey;
			bool allRead = true;
			for (uint8_t i = 0; i < count; i++) {
				uint16_t b = first + i;
				if (pn532->mifareclassic_ReadDataBlock(b, tag.blocks + b * 16)) tag.blockKnown[b] = 1;
				else allRead = false;
			}
			if (tag.blockKnown[trailer]) {
				// The card never returns key A (and often not key B); put the key that
				// opened the sector into the trailer, like the Flipper does.
				uint8_t* t = tag.blocks + trailer * 16;
				memcpy(usedType == 0 ? t : t + 10, KEYS[usedKey], 6);
			}
			if (allRead) tag.sectorsRead++;
		}
		progress(onProgress, s + 1, tag.sectorsTotal);
	}
	return true;
}

// MIFARE Ultralight / NTAG

struct UltralightInfo {
	const char* name;
	uint16_t pages;
};

/// Maps a GET_VERSION reply to a type Flipper knows; nullptr name = not supported.
static UltralightInfo ultralightFromVersion(const uint8_t* v) {
	if (v[0] != 0x00 || v[1] != 0x04) return {nullptr, 0};
	if (v[2] == 0x04 && v[3] == 0x02) { // NTAG21x
		switch (v[6]) {
			case 0x0F: return {"NTAG213", 45};
			case 0x11: return {"NTAG215", 135};
			case 0x13: return {"NTAG216", 231};
		}
	} else if (v[2] == 0x03 && v[3] == 0x01) { // Ultralight EV1
		switch (v[6]) {
			case 0x0B: return {"Mifare Ultralight 11", 20};
			case 0x0E: return {"Mifare Ultralight 21", 41};
		}
	}
	return {nullptr, 0};
}

static bool dumpUltralight(Tag& tag, ProgressFn onProgress) {
	// Identify via GET_VERSION. Tags without it (original Ultralight, NTAG203, Ultralight C)
	// NAK it, which drops them out of the active state.
	uint8_t cmd = 0x60;
	uint8_t len = sizeof(tag.version);
	UltralightInfo info = {nullptr, 0};
	if (commThru(&cmd, 1, tag.version, len) && len == sizeof(tag.version)) {
		info = ultralightFromVersion(tag.version);
	} else {
		memset(tag.version, 0, sizeof(tag.version));
		if (!reselect(tag)) return false;
		// Original Ultralight has 16 pages and NAKs a read past the end; the larger
		// look-alikes do not, so they stay unsupported (saved as plain ISO14443-3A).
		uint8_t rd[2] = {0x30, 16};
		uint8_t buf[16], n = sizeof(buf);
		if (!commThru(rd, 2, buf, n)) {
			info = {"Mifare Ultralight", 16};
			if (!reselect(tag)) return false;
		}
	}
	if (!info.name) return false;

	tag.subtype = info.name;
	tag.pagesTotal = info.pages;
	tag.pages = (uint8_t*)calloc(tag.pagesTotal, 4);
	if (!tag.pages) return false;

	// READ returns 4 pages (16 bytes) at a time
	for (uint16_t p = 0; p < tag.pagesTotal; p += 4) {
		uint8_t buf[16], n = sizeof(buf);
		uint8_t rd[2] = {0x30, (uint8_t)p};
		if (!commThru(rd, 2, buf, n) || n < 16) break;
		uint16_t take = min<uint16_t>(4, tag.pagesTotal - p);
		memcpy(tag.pages + p * 4, buf, take * 4);
		tag.pagesRead = p + take;
		progress(onProgress, tag.pagesRead, tag.pagesTotal);
	}

	// Signature is optional (Ultralight EV1 has none); zeros are saved when absent
	uint8_t sig[2] = {0x3C, 0x00};
	uint8_t sigLen = sizeof(tag.signature);
	if (!commThru(sig, 2, tag.signature, sigLen) || sigLen != sizeof(tag.signature)) {
		memset(tag.signature, 0, sizeof(tag.signature));
	}
	return tag.pagesRead == tag.pagesTotal;
}

// Public reading entry point

static bool pn532ReadTag(Tag& tag, uint16_t timeoutMs, ProgressFn onProgress) {
	tag.release();
	if (!selectTag(tag.uid, tag.uidLen, tag.atqa, &tag.sak, timeoutMs)) return false;

	switch (tag.sak) {
		case 0x08: tag.subtype = "1K";   tag.blockCount = 64;  break;
		case 0x18: tag.subtype = "4K";   tag.blockCount = 256; break;
		case 0x09: tag.subtype = "Mini"; tag.blockCount = 20;  break;
	}

	if (tag.blockCount) {
		tag.type = TagType::MIFARE_CLASSIC;
		if (!dumpClassic(tag, onProgress)) {
			// Out of memory or tag lost: keep the UID, drop the partial dump
			uint8_t uid[10], uidLen = tag.uidLen, atqa[2], sak = tag.sak;
			memcpy(uid, tag.uid, sizeof(uid));
			memcpy(atqa, tag.atqa, sizeof(atqa));
			tag.release();
			memcpy(tag.uid, uid, sizeof(uid));
			memcpy(tag.atqa, atqa, sizeof(atqa));
			tag.uidLen = uidLen;
			tag.sak = sak;
		}
	} else if (tag.sak == 0x00 && tag.uidLen == 7) {
		tag.type = TagType::ULTRALIGHT;
		if (!dumpUltralight(tag, onProgress)) {
			// Unsupported or unreadable: fall back to a plain ISO14443-3A tag
			free(tag.pages);
			tag.pages = nullptr;
			tag.pagesTotal = tag.pagesRead = 0;
			tag.subtype = "";
			tag.type = TagType::ISO14443_3A;
		}
	}
	return true;
}

bool readTag(Tag& tag, uint16_t timeoutMs, ProgressFn onProgress) {
	if (_active == Module::RC522) return rc522::readTag(tag);
	return pn532ReadTag(tag, timeoutMs, onProgress);
}

// Saving

bool save(const Tag& tag, String& path, bool& usedLittleFS) {
	String name = "/";
	for (uint8_t i = 0; i < tag.uidLen; i++) {
		if (tag.uid[i] < 0x10) name += "0";
		name += String(tag.uid[i], HEX);
	}
	name.toUpperCase();

	usedLittleFS = Storage::autoDetect();
	path = Storage::uniquePath(name, ".nfc", usedLittleFS);
	File f = Storage::open(path.c_str(), FILE_WRITE, usedLittleFS);
	if (!f) return false;
	writeFlipperFile(tag, f);
	f.close();
	return true;
}

// Writing

static uint8_t calculateBCC(const uint8_t* uid, uint8_t len) {
	uint8_t bcc = 0;
	for (uint8_t i = 0; i < len; i++) bcc ^= uid[i];
	return bcc;
}

bool writeUid(const Tag& tag) {
	if (tag.uidLen != 4) return false; // CUID block 0 only supports a 4-byte UID

	uint8_t uid[10], uidLen;
	if (!tagSelect(uid, uidLen, 500)) return false;
	if (uidLen != 4) return false;

	static const uint8_t DEFAULT_KEY[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	if (!tagAuthenticate(uid, uidLen, 0, 0, DEFAULT_KEY)) return false;

	uint8_t block0[16] = {0};
	memcpy(block0, tag.uid, 4);
	block0[4] = calculateBCC(tag.uid, 4);
	block0[5] = tag.sak ? tag.sak : 0x08;
	block0[6] = tag.atqa[1]; // ATQA is stored MSB-first; block 0 wants LSB first
	block0[7] = tag.atqa[0];

	if (!tagWriteBlock(0, block0)) return false;
	delay(200);

	if (!tagSelect(uid, uidLen, 500)) return false;
	return uidLen == 4 && memcmp(uid, tag.uid, 4) == 0;
}

bool writeClassicSectors(const Tag& tag, ProgressFn onProgress) {
	if (tag.type != TagType::MIFARE_CLASSIC || !tag.blocks || !tag.blockKnown) return false;

	uint8_t uid[10], uidLen;
	if (!tagSelect(uid, uidLen, 500)) return false;

	Tag onCard; // only uid/uidLen are used, to drive reselect() after a dropped auth
	memcpy(onCard.uid, uid, uidLen);
	onCard.uidLen = uidLen;

	uint8_t lastKey = 0;
	for (uint8_t s = 0; s < tag.sectorsTotal; s++) {
		uint16_t first = sectorFirstBlock(s);
		uint8_t count = sectorBlocks(s);
		uint16_t trailer = first + count - 1;

		// Nothing known in this sector (all "??" or the trailer only): skip it.
		bool haveData = false;
		for (uint8_t i = 0; i < count - 1; i++) {
			if (tag.blockKnown[first + i]) { haveData = true; break; }
		}
		if (!haveData) {
			progress(onProgress, s + 1, tag.sectorsTotal);
			continue;
		}

		bool opened = false;
		uint8_t usedType = 0;
		for (uint8_t kt = 0; kt < 2 && !opened; kt++) {
			for (uint8_t n = 0; n < KEY_COUNT && !opened; n++) {
				uint8_t k = (n + lastKey) % KEY_COUNT;
				uint8_t key[6];
				memcpy(key, KEYS[k], 6);
				if (tagAuthenticate(uid, uidLen, first, kt, key)) {
					opened = true;
					lastKey = k;
					usedType = kt;
				} else if (!reselect(onCard)) {
					return false; // tag left the field
				}
			}
		}
		if (!opened) { progress(onProgress, s + 1, tag.sectorsTotal); continue; }

		for (uint8_t i = 0; i < count - 1; i++) { // trailer (key + access bits) is left alone
			uint16_t b = first + i;
			if (tag.blockKnown[b]) tagWriteBlock(b, tag.blocks + b * 16);
		}
		progress(onProgress, s + 1, tag.sectorsTotal);
	}
	return true;
}

} // namespace Nfc
