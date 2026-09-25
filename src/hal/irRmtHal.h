/*
 * irRmtHal.h — RMT-based IR for M5StickS3 (ESP-IDF 5.x / Arduino 3.x)
 *
 * Uses the ESP-IDF 5.x RMT driver API (rmt_rx.h / rmt_tx.h).
 * Protocols supported: NEC, SONY, SAMSUNG, RC5
 */

#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/rmt.h"

// In ESP-IDF 4.x the RMT symbol type is rmt_item32_t (same layout as rmt_symbol_word_t)
typedef rmt_item32_t rmt_symbol_word_t;

// Bit-reverse helper (NEC/Samsung transmit each byte LSB-first)
static inline uint8_t ir_rev8(uint8_t b) {
	b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
	b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
	b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
	return b;
}

// Build NEC transmission-order code (MSB-first, each byte LSB-first)
static inline uint32_t irBuildNEC(uint8_t addr, uint8_t cmd) {
	return ((uint32_t)ir_rev8(addr) << 24)
		 | ((uint32_t)ir_rev8((uint8_t)~addr) << 16)
		 | ((uint32_t)ir_rev8(cmd) << 8)
		 | (uint32_t)ir_rev8((uint8_t)~cmd);
}

// Convert transmission-order to display format (for .ir file compatibility)
static inline uint32_t irNecForDisplay(uint32_t transCode) {
	uint8_t a = ir_rev8((transCode >> 24) & 0xFF);
	uint8_t na = ir_rev8((transCode >> 16) & 0xFF);
	uint8_t c = ir_rev8((transCode >> 8) & 0xFF);
	uint8_t nc = ir_rev8(transCode & 0xFF);
	return ((uint32_t)nc << 24) | ((uint32_t)c << 16)
		 | ((uint32_t)na << 8) | (uint32_t)a;
}

// Convert display format back to transmission-order (for playback from .ir files)
// Display format: [~cmd, cmd, ~addr, addr] — preserves exact received bytes
static inline uint32_t irNecFromDisplay(uint32_t displayCode) {
	uint8_t nc = (displayCode >> 24) & 0xFF;
	uint8_t c  = (displayCode >> 16) & 0xFF;
	uint8_t na = (displayCode >> 8) & 0xFF;
	uint8_t a  = displayCode & 0xFF;
	return ((uint32_t)ir_rev8(a) << 24)
		 | ((uint32_t)ir_rev8(na) << 16)
		 | ((uint32_t)ir_rev8(c) << 8)
		 | (uint32_t)ir_rev8(nc);
}

// ── Types ──

enum irproto { UNK, NEC, SONY, SAM, RC5, PROTO_COUNT };

typedef struct {
	uint16_t header_high;
	uint16_t header_low;
	uint16_t one_high;
	uint16_t one_low;
	uint16_t zero_high;
	uint16_t zero_low;
	uint16_t footer_high;
	uint8_t footer_low;
	uint16_t frequency;
	const char* name;
} ir_protocol_t;

extern const ir_protocol_t proto[PROTO_COUNT];

// ── Pin configuration ──

extern uint8_t irRxPin;
extern uint8_t irTxPin;
extern uint8_t irRmtRxPin; // fixed built-in RMT RX pin (M5StickS3 photodiode)

// ── TX/RX synchronisation ──

extern volatile uint8_t irTX;
extern volatile uint8_t irRX;

// ── User callback (defined in irReadLoop.h) ──

extern void irReceived(irproto brand, uint32_t code, size_t len,
					rmt_symbol_word_t *item);

// ── Public API ──

void recvIR(void* param);
void irStopReceiver(); // signal RX task to stop and free RMT resources
void sendIR(irproto brand, uint32_t code, uint8_t bits = 32,
			uint8_t burst = 1, uint8_t repeat = 1);

// ── Internal decoders ──

uint32_t nec_check(rmt_symbol_word_t *item, size_t &len);
uint32_t sam_check(rmt_symbol_word_t *item, size_t &len);
uint32_t sony_check(rmt_symbol_word_t *item, size_t &len);
uint32_t rc5_check(rmt_symbol_word_t *item, size_t &len);
bool checkbit(rmt_symbol_word_t &item, uint16_t high, uint16_t low);
bool rc5_bit(uint32_t d, uint32_t v);
