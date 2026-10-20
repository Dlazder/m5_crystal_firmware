#include "ch9329.h"

namespace CH9329 {

static HardwareSerial* s_port = nullptr;

void begin(HardwareSerial* port) {
	s_port = port;
}

void end() {
	s_port = nullptr;
}

bool isReady() {
	return s_port != nullptr;
}

static void sendPacket(uint8_t cmd, const uint8_t* data, uint8_t len) {
	if (!s_port) return;

	uint8_t header[5] = { 0x57, 0xAB, 0x00, cmd, len };
	uint8_t sum = 0;
	for (uint8_t b : header) sum += b;
	for (uint8_t i = 0; i < len; i++) sum += data[i];

	s_port->write(header, sizeof(header));
	if (len) s_port->write(data, len);
	s_port->write(sum);
}

void sendKeyReport(uint8_t modifiers, const uint8_t* keys, uint8_t keyCount) {
	uint8_t report[8] = { modifiers, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t n = (keyCount > 6) ? 6 : keyCount;
	for (uint8_t i = 0; i < n; i++) report[2 + i] = keys[i];
	sendPacket(0x02, report, sizeof(report));
}

void releaseAll() {
	sendKeyReport(0, nullptr, 0);
}

void sendMouseReport(uint8_t buttons, int8_t dx, int8_t dy, int8_t wheel) {
	uint8_t report[5] = { 0x01, buttons, (uint8_t)dx, (uint8_t)dy, (uint8_t)wheel };
	sendPacket(0x05, report, sizeof(report));
}

} // namespace CH9329
