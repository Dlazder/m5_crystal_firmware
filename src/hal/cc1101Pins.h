#pragma once

#if defined(CARDPUTER)
	#define CC1101_CS_PIN 13
	#define CC1101_SCK_PIN 5
	#define CC1101_MOSI_PIN 6
	#define CC1101_MISO_PIN 4
	#define CC1101_GDO0_PIN 3
#elif defined(STICKS3)
	#define CC1101_CS_PIN 2
	#define CC1101_SCK_PIN 5
	#define CC1101_MOSI_PIN 6
	#define CC1101_MISO_PIN 4
	#define CC1101_GDO0_PIN 3
#else
	#define CC1101_CS_PIN 32
	#define CC1101_SCK_PIN 0
	#define CC1101_MOSI_PIN 26
	#define CC1101_MISO_PIN 36
	#define CC1101_GDO0_PIN 33
#endif
