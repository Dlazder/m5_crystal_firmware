#pragma once
// Default pins of the nRF24L01(+) module, per device.
// Selected by the same build flags as devices/device.h. The SPI lines match the
// CC1101 defaults (hal/cc1101Pins.h): CSN takes the CC1101 CS pin and CE its GDO0
// pin, so the module goes where a CC1101 would. Every pin is configurable in
// NRF > configure pins. On StickS3 they follow the 8-pin adapter header
// (GDO0 = CE on G7, CSN on G8). IRQ is not used and stays unconnected.

#if defined(CARDPUTER)
	#define NRF_CSN_PIN 13
	#define NRF_SCK_PIN 5
	#define NRF_MOSI_PIN 6
	#define NRF_MISO_PIN 4
	#define NRF_CE_PIN 3
#elif defined(STICKS3)
	#define NRF_CSN_PIN 8
	#define NRF_SCK_PIN 5
	#define NRF_MOSI_PIN 6
	#define NRF_MISO_PIN 4
	#define NRF_CE_PIN 7
#else
	#define NRF_CSN_PIN 32
	#define NRF_SCK_PIN 0
	#define NRF_MOSI_PIN 26
	#define NRF_MISO_PIN 36
	#define NRF_CE_PIN 33
#endif
