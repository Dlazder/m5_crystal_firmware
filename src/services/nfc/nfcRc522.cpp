#include "nfcRc522.h"
#include <SPI.h>
#include <MFRC522.h>

namespace Nfc {
namespace rc522 {

static MFRC522* reader = nullptr;
static uint8_t ssPin = 0;

bool begin(const Rc522Pins& pins) {
	end();
	ssPin = pins.ss;

	// Restart the bus to honour pin changes made in settings.
	SPI.begin(pins.sck, pins.miso, pins.mosi, pins.ss);
	reader = new MFRC522(pins.ss, pins.rst);
	reader->PCD_Init();
	delay(10);

	bool ok = isConnected();
	Serial.printf("[RC522] %s\n", ok ? "found" : "not found");
	return ok;
}

void end() {
	if (reader) {
		reader->PCD_AntennaOff();
		delete reader;
		reader = nullptr;
		SPI.end(); // detach the pins from the SPI matrix so they can be reused
		pinMode(ssPin, INPUT);
	}
}

/// 0x00 / 0xFF mean nothing drives MISO (no module, or wiring fault).
bool isConnected() {
	if (!reader) return false;
	uint8_t v = reader->PCD_ReadRegister(MFRC522::VersionReg);
	return v != 0x00 && v != 0xFF;
}

bool readTag(Tag& tag) {
	if (!reader) return false;
	tag.release();

	byte atqa[2];
	byte atqaSize = sizeof(atqa);
	MFRC522::StatusCode st = reader->PICC_RequestA(atqa, &atqaSize);
	if (st != MFRC522::STATUS_OK && st != MFRC522::STATUS_COLLISION) return false;
	if (!reader->PICC_ReadCardSerial()) return false;

	const MFRC522::Uid& uid = reader->uid;
	tag.uidLen = min<uint8_t>(uid.size, sizeof(tag.uid));
	memcpy(tag.uid, uid.uidByte, tag.uidLen);
	tag.sak = uid.sak;
	// REQA answers LSB first; Tag keeps ATQA MSB first (the Flipper order)
	tag.atqa[0] = atqa[1];
	tag.atqa[1] = atqa[0];

	// Only the family is identified from the SAK, the contents are not dumped, so the
	// tag stays plain ISO14443-3A (the subtype is just shown on screen).
	switch (MFRC522::PICC_GetType(uid.sak)) {
		case MFRC522::PICC_TYPE_MIFARE_MINI: tag.subtype = "Mifare Mini"; break;
		case MFRC522::PICC_TYPE_MIFARE_1K:   tag.subtype = "Mifare Classic 1K"; break;
		case MFRC522::PICC_TYPE_MIFARE_4K:   tag.subtype = "Mifare Classic 4K"; break;
		case MFRC522::PICC_TYPE_MIFARE_UL:   tag.subtype = "Mifare Ultralight / NTAG"; break;
		case MFRC522::PICC_TYPE_MIFARE_PLUS: tag.subtype = "Mifare Plus"; break;
		case MFRC522::PICC_TYPE_MIFARE_DESFIRE: tag.subtype = "Mifare DESFire"; break;
		default: tag.subtype = "ISO14443-3A"; break;
	}

	reader->PICC_HaltA();
	return true;
}

bool select(uint8_t* uid, uint8_t& uidLen, uint16_t timeoutMs) {
	if (!reader) return false;
	uint32_t start = millis();
	do {
		reader->PCD_StopCrypto1(); // a previous auth leaves the crypto unit on
		// WUPA, unlike REQA, also answers a tag that is in the HALT state
		byte atqa[2];
		byte atqaSize = sizeof(atqa);
		MFRC522::StatusCode st = reader->PICC_WakeupA(atqa, &atqaSize);
		if ((st == MFRC522::STATUS_OK || st == MFRC522::STATUS_COLLISION) && reader->PICC_ReadCardSerial()) {
			uidLen = min<uint8_t>(reader->uid.size, 10);
			memcpy(uid, reader->uid.uidByte, uidLen);
			return true;
		}
		delay(5);
	} while (millis() - start < timeoutMs);
	return false;
}

bool authenticate(uint16_t block, uint8_t keyType, const uint8_t* key) {
	if (!reader) return false;
	MFRC522::MIFARE_Key mifareKey;
	memcpy(mifareKey.keyByte, key, MFRC522::MF_KEY_SIZE);
	byte command = keyType == 0 ? MFRC522::PICC_CMD_MF_AUTH_KEY_A : MFRC522::PICC_CMD_MF_AUTH_KEY_B;
	return reader->PCD_Authenticate(command, (byte)block, &mifareKey, &reader->uid) == MFRC522::STATUS_OK;
}

bool writeBlock(uint16_t block, const uint8_t* data) {
	if (!reader) return false;
	byte buffer[16];
	memcpy(buffer, data, sizeof(buffer));
	return reader->MIFARE_Write((byte)block, buffer, sizeof(buffer)) == MFRC522::STATUS_OK;
}

} // namespace rc522
} // namespace Nfc
