/*
 * irGpioRx.cpp — GPIO-based IR receiver (IRremote library).
 *
 * This is the only translation unit that includes IRremote.hpp for RX, so it
 * can coexist with the RMT HAL (irRmtHal.h) which uses the legacy driver/rmt.h
 * and a conflicting protocol enum.
 */

#include "irGpioRx.h"
#include <IRremote.hpp>

void irGpioRxBegin(uint8_t pin) {
	IrReceiver.begin(pin, DISABLE_LED_FEEDBACK);
}

bool irGpioRxPoll(uint16_t& addr, uint16_t& cmd, uint32_t& raw, String& proto) {
	if (!IrReceiver.decode()) return false;
	bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;
	raw   = IrReceiver.decodedIRData.decodedRawData;
	addr  = IrReceiver.decodedIRData.address;
	cmd   = IrReceiver.decodedIRData.command;
	proto = getProtocolString(IrReceiver.decodedIRData.protocol);
	IrReceiver.resume();
	return !isRepeat;
}

void irGpioRxEnd() {
	IrReceiver.end();
}
