#pragma once
// Default pins of the RC522 (MFRC522) module, per device.
// Selected by the same build flags as devices/device.h. The SPI lines match the
// CC1101 defaults (hal/cc1101Pins.h) so both SPI add-ons can share one header;
// every pin is configurable in NFC > configure pins > RC522.

#if defined(CARDPUTER)
	#define RC522_SS_PIN 15
	#define RC522_SCK_PIN 5
	#define RC522_MOSI_PIN 6
	#define RC522_MISO_PIN 4
	#define RC522_RST_PIN 3
#elif defined(STICKS3)
	#define RC522_SS_PIN 8
	#define RC522_SCK_PIN 5
	#define RC522_MOSI_PIN 6
	#define RC522_MISO_PIN 4
	#define RC522_RST_PIN 3
#else
	#define RC522_SS_PIN 32
	#define RC522_SCK_PIN 0
	#define RC522_MOSI_PIN 26
	#define RC522_MISO_PIN 36
	#define RC522_RST_PIN 33
#endif
