#include "nrf24.h"
#include <SPI.h>

namespace NRF24 {

// Commands and registers (nRF24L01+ datasheet, section 8 and 9)
static const uint8_t CMD_W_REGISTER = 0x20;
static const uint8_t CMD_FLUSH_RX = 0xE2;
static const uint8_t REG_CONFIG = 0x00;
static const uint8_t REG_EN_AA = 0x01;
static const uint8_t REG_EN_RXADDR = 0x02;
static const uint8_t REG_RF_CH = 0x05;
static const uint8_t REG_RF_SETUP = 0x06;
static const uint8_t REG_RPD = 0x09;

static const uint8_t CONFIG_PWR_UP_RX = 0x03; // PWR_UP | PRIM_RX, CRC off
static const uint8_t RF_SETUP_2MBPS_MAX = 0x0F;

static uint8_t csnPin = NRF_CSN_PIN;
static uint8_t cePin = NRF_CE_PIN;
static bool begun = false;

// Well under the chip's 10 MHz limit: jumper wires to a breakout are marginal above ~2 MHz.
static const SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

static uint8_t readReg(uint8_t reg) {
	SPI.beginTransaction(spiSettings);
	digitalWrite(csnPin, LOW);
	SPI.transfer(reg);
	uint8_t v = SPI.transfer(0xFF);
	digitalWrite(csnPin, HIGH);
	SPI.endTransaction();
	return v;
}

static void writeReg(uint8_t reg, uint8_t value) {
	SPI.beginTransaction(spiSettings);
	digitalWrite(csnPin, LOW);
	SPI.transfer(CMD_W_REGISTER | reg);
	SPI.transfer(value);
	digitalWrite(csnPin, HIGH);
	SPI.endTransaction();
}

static void command(uint8_t cmd) {
	SPI.beginTransaction(spiSettings);
	digitalWrite(csnPin, LOW);
	SPI.transfer(cmd);
	digitalWrite(csnPin, HIGH);
	SPI.endTransaction();
}

bool begin(uint8_t csn, uint8_t sck, uint8_t mosi, uint8_t miso, uint8_t ce) {
	csnPin = csn;
	cePin = ce;

	pinMode(ce, OUTPUT);
	digitalWrite(ce, LOW);
	pinMode(csn, OUTPUT);
	digitalWrite(csn, HIGH);

	SPI.begin(sck, miso, mosi, csn);
	delay(5); // power-on reset settling

	// A reset-state RF_CH is 0x02 and a missing module reads a constant 0x00/0xFF,
	// so write two different values and require both to read back.
	writeReg(REG_RF_CH, 0x2A);
	bool ok = readReg(REG_RF_CH) == 0x2A;
	writeReg(REG_RF_CH, 0x55);
	ok = ok && readReg(REG_RF_CH) == 0x55;
	if (!ok) {
		// Raw values tell the failure apart: all 0xFF = MISO pulled high (no power / not
		// wired), all 0x00 = MISO shorted low or CSN not reaching the chip.
		Serial.printf("[NRF24] not responding: CSN=G%d SCK=G%d MOSI=G%d MISO=G%d CE=G%d\n",
									csn, sck, mosi, miso, ce);
		Serial.printf("[NRF24] CONFIG=0x%02X (reset 0x08) EN_AA=0x%02X (0x3F) RF_CH=0x%02X (0x02) RF_SETUP=0x%02X (0x0E)\n",
									readReg(REG_CONFIG), readReg(REG_EN_AA), readReg(REG_RF_CH), readReg(REG_RF_SETUP));
		SPI.end();
		pinMode(csn, INPUT);
		pinMode(ce, INPUT);
		return false;
	}

	writeReg(REG_EN_AA, 0x00); // no auto-ack
	writeReg(REG_EN_RXADDR, 0x00); // no pipes: RPD only needs the receiver listening
	writeReg(REG_RF_SETUP, RF_SETUP_2MBPS_MAX);
	writeReg(REG_CONFIG, CONFIG_PWR_UP_RX);
	delay(5); // Tpd2stby
	command(CMD_FLUSH_RX);

	begun = true;
	Serial.println("[NRF24] found");
	return true;
}

void end() {
	if (begun) {
		digitalWrite(cePin, LOW);
		writeReg(REG_CONFIG, 0x00); // power down
	}
	SPI.end(); // detach the pins from the SPI matrix so they can be reused
	pinMode(csnPin, INPUT);
	pinMode(cePin, INPUT);
	begun = false;
}

bool isConnected() { return begun; }

bool carrierOnChannel(uint8_t channel) {
	writeReg(REG_RF_CH, channel);
	digitalWrite(cePin, HIGH);
	delayMicroseconds(130); // Tstby2a + RPD settling
	bool hit = readReg(REG_RPD) & 0x01;
	digitalWrite(cePin, LOW);
	return hit;
}

} // namespace NRF24
