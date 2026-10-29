#pragma once
#include <Arduino.h>
#include "ch9329.h"
#include "ch9329KeycodeMap.h"

namespace Ch9329Sink {
	static uint8_t heldKeys[6] = {0};
	static uint8_t heldCount = 0;
	static uint8_t heldMods = 0;

	static const unsigned long REPORT_GAP_MS = 10;
	static unsigned long s_releaseDue = 0; // 0 = no release pending

	static void flush() {
		CH9329::sendKeyReport(heldMods, heldKeys, heldCount);
	}

	// Sends the deferred release from write() once REPORT_GAP_MS has elapsed.
	// No-op if nothing is pending or the gap hasn't elapsed yet.
	static void poll() {
		if (s_releaseDue == 0) return;
		if ((long)(millis() - s_releaseDue) < 0) return;
		CH9329::releaseAll();
		s_releaseDue = 0;
	}

	// If a write()'s release is still pending, force it out now (e.g. before a
	// press()/COMBO needs the wire, or on teardown) instead of waiting for poll().
	static void flushPendingRelease() {
		if (s_releaseDue == 0) return;
		CH9329::releaseAll();
		s_releaseDue = 0;
	}

	// write(): press then release — used for STRING chars and KEY.
	static void doWrite(uint8_t duckyCode) {
		flushPendingRelease(); // finish any previous write() before starting a new one

		uint8_t modBit;
		if (CH9329KeycodeMap::isModifier(duckyCode, modBit)) {
			CH9329::sendKeyReport(modBit, nullptr, 0);
			s_releaseDue = millis() + REPORT_GAP_MS;
			return;
		}

		uint8_t base = duckyCode;
		uint8_t mods = 0;
		if (duckyCode < 0x80 && CH9329KeycodeMap::needsShift((char)duckyCode)) {
			mods |= CH9329::MOD_LSHIFT;
			char shiftedBase = CH9329KeycodeMap::shiftedSymbolBase((char)duckyCode);
			if (shiftedBase) base = (uint8_t)shiftedBase;
		}

		uint8_t usage = CH9329KeycodeMap::toHidUsage(base);
		if (!usage) return; // unsupported code — drop, same as other sinks

		CH9329::sendKeyReport(mods, &usage, 1);
		s_releaseDue = millis() + REPORT_GAP_MS;
	}

	// press(): add to the held set (for COMBO) without releasing.
	static void doPress(uint8_t duckyCode) {
		flushPendingRelease();

		uint8_t modBit;
		if (CH9329KeycodeMap::isModifier(duckyCode, modBit)) {
			heldMods |= modBit;
			flush();
			return;
		}
		uint8_t usage = CH9329KeycodeMap::toHidUsage(duckyCode);
		if (!usage || heldCount >= 6) return;
		heldKeys[heldCount++] = usage;
		flush();
	}

	static void doReleaseAll() {
		s_releaseDue = 0;
		heldCount = 0;
		heldMods = 0;
		CH9329::releaseAll();
	}
}
