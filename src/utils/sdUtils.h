#if HAS_SD
#include <SD.h>
#include <SPI.h>

bool sdBegun = false;

bool sdBegin() {
	if (sdBegun) return true;
	SPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
	sdBegun = SD.begin(SD_CS_PIN);
	return sdBegun;
}

void sdEnd() {
	if (!sdBegun) return;
	SD.end();
	sdBegun = false;
}
#endif
