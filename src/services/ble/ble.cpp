#include "ble.h"
#include "../storage/storage.h"

namespace Ble {

BleComboKeyboard keyboard("M5 Crystal", "M5 Crystal");
BleComboMouse mouse(&keyboard);

bool compositeBegan = false;
bool connected = false;
SelectedDevice selected;

// See the declaration in ble.h for the weak/strong mechanism.
__attribute__((weak)) void reportBeginWaiting() {}

void begin() {
	if (!compositeBegan) {
		// Free the SD card before BLE init: Bluedroid needs a large contiguous
		// block of internal RAM, and the SD driver + script (already loaded by
		// the Duckyscript service) are no longer needed once the file has been read.
		// Cardputer ADV (no PSRAM) this is the difference between a successful
		// BLE init and xSemaphoreCreateBinary() returning NULL (heap exhaustion).
		Storage::unmountSD();
		keyboard.begin();
		compositeBegan = true;
	}
	connected = false;
	reportBeginWaiting();
}

void handleConnection(void (*onConnect)(), void (*onDisconnect)()) {
	if (keyboard.isConnected()) {
		if (!connected) {
			connected = true;
			onConnect();
		}
	} else {
		if (connected) {
			connected = false;
			onDisconnect();
		}
	}
}

String manufacturerName(uint16_t companyId) {
	switch (companyId) {
		case 0x004C: return "Apple";
		case 0x0006: return "Microsoft";
		case 0x0075: return "Samsung";
		case 0x00E0: return "Google";
		case 0x0157: return "Garmin";
		case 0x0059: return "Nordic Semi";
		case 0x0499: return "Ruuvi";
		case 0x0087: return "Garmin";
		case 0x0010: return "Mitsumi";
		case 0x0001: return "Ericsson";
		case 0x0002: return "Nokia";
		case 0x000F: return "Broadcom";
		case 0x004F: return "M-Way";
		case 0x0171: return "Amazon";
		case 0x0183: return "Jabra";
		case 0x00D2: return "Xiaomi";
		case 0x038F: return "Amazfit";
		case 0x0103: return "Huawei";
		case 0x02D5: return "Espressif";
		case 0x008C: return "Sony";
		case 0x0046: return "Philips";
		default: {
			char buf[8];
			sprintf(buf, "0x%04X", companyId);
			return String(buf);
		}
	}
}

} // namespace Ble
