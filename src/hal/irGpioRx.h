/*
 * irGpioRx.h — GPIO-based IR receiver (IRremote library).
 *
 * Kept in its own .cpp/.h pair so IRremote.hpp and the RMT HAL
 * (irRmtHal.h) never land in the same translation unit — their protocol
 * enums (decode_type_t vs irproto) share enumerator names and would clash.
 */

#pragma once
#include <Arduino.h>

// Start the GPIO receiver on `pin`.
void irGpioRxBegin(uint8_t pin);

// Poll for a decoded signal. Returns true when a NEW (non-repeat) frame was
// decoded, and writes the decoded address / command / raw / protocol name.
// Repeat frames are consumed and reported as false.
bool irGpioRxPoll(uint16_t& addr, uint16_t& cmd, uint32_t& raw, String& proto);

// Stop the receiver, releasing its resources.
void irGpioRxEnd();
