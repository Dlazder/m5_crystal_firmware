#include "cc1101.h"
#include <SPI.h>

namespace CC1101 {

static uint8_t csPin = CC1101_CS_PIN;
static bool begun = false;

bool begin(uint8_t cs, uint8_t sck, uint8_t mosi, uint8_t miso, uint8_t gdo0) {
	csPin = cs;

	pinMode(cs, OUTPUT);
	digitalWrite(cs, HIGH);
	pinMode(miso, INPUT);
	delay(5);
	digitalWrite(cs, LOW);
	uint32_t start = millis();
	while (digitalRead(miso) && millis() - start < 50) {}
	bool ready = !digitalRead(miso);
	digitalWrite(cs, HIGH);
	if (!ready) {
		Serial.println("[CC1101] MISO stuck high, module not responding");
		pinMode(cs, INPUT);
		return false;
	}

	// The library calls SPI.begin() only once per boot, so (re)start the bus here
	// to honour pin changes made in settings.
	SPI.begin(sck, miso, mosi, cs);
	ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, cs);
	ELECHOUSE_cc1101.setGDO0(gdo0);
	ELECHOUSE_cc1101.Init();

	begun = ELECHOUSE_cc1101.getCC1101();
	Serial.printf("[CC1101] %s\n", begun ? "found" : "not found");
	return begun;
}

void end() {
	if (begun) ELECHOUSE_cc1101.setSidle();
	SPI.end(); // detach the pins from the SPI matrix so they can be reused
	pinMode(csPin, INPUT);
	begun = false;
}

bool isConnected() { return begun; }

} // namespace CC1101
