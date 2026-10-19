// Serializer for the Flipper Zero "Flipper NFC device" file format, version 4.
// Format reference: flipperzero-firmware/documentation/file_formats/NfcFileFormats.md
// Pure formatting: no hardware or filesystem access.

#include "nfc.h"

namespace Nfc {

String uidToString(const uint8_t* uid, uint8_t uidLength) {
	String uidString = "";
	for (uint8_t i = 0; i < uidLength; i++) {
		if (uid[i] < 0x10) uidString += "0";
		uidString += String(uid[i], HEX);
		if (i < uidLength - 1) uidString += ":";
	}
	return uidString;
}

static void printHex(Print& out, const uint8_t* data, size_t len) {
	static const char digits[] = "0123456789ABCDEF";
	for (size_t i = 0; i < len; i++) {
		if (i) out.print(' ');
		out.print(digits[data[i] >> 4]);
		out.print(digits[data[i] & 0x0F]);
	}
}

static void printHexLine(Print& out, const char* key, const uint8_t* data, size_t len) {
	out.print(key);
	out.print(": ");
	printHex(out, data, len);
	out.print('\n');
}

static void printLine(Print& out, const char* key, const char* value) {
	out.print(key);
	out.print(": ");
	out.print(value);
	out.print('\n');
}

static void printNumLine(Print& out, const char* key, unsigned value) {
	out.print(key);
	out.print(": ");
	out.print(value);
	out.print('\n');
}

static const char* deviceName(TagType type) {
	switch (type) {
		case TagType::MIFARE_CLASSIC: return "Mifare Classic";
		case TagType::ULTRALIGHT: return "NTAG/Ultralight";
		default:                  return "ISO14443-3A";
	}
}

static void writeClassic(const Tag& tag, Print& out) {
	out.print("# Mifare Classic specific data\n");
	printLine(out, "Mifare Classic type", tag.subtype);
	out.print("Data format version: 2\n");
	out.print("# Mifare Classic blocks, '?\?' means unknown data\n"); // '?\?' avoids the ??' trigraph

	for (uint16_t b = 0; b < tag.blockCount; b++) {
		out.print("Block ");
		out.print(b);
		out.print(": ");
		if (tag.blockKnown[b]) {
			printHex(out, tag.blocks + b * 16, 16);
		} else {
			for (uint8_t i = 0; i < 16; i++) {
				if (i) out.print(' ');
				out.print("??");
			}
		}
		out.print('\n');
	}
}

static void writeUltralight(const Tag& tag, Print& out) {
	out.print("# NTAG/Ultralight specific data\n");
	out.print("Data format version: 2\n");
	printLine(out, "NTAG/Ultralight type", tag.subtype);
	printHexLine(out, "Signature", tag.signature, sizeof(tag.signature));
	printHexLine(out, "Mifare version", tag.version, sizeof(tag.version));

	// Counters are not read; Flipper expects the fields to be present.
	for (uint8_t i = 0; i < 3; i++) {
		out.print("Counter ");
		out.print(i);
		out.print(": 0\n");
		out.print("Tearing ");
		out.print(i);
		out.print(": 00\n");
	}

	printNumLine(out, "Pages total", tag.pagesTotal);
	printNumLine(out, "Pages read", tag.pagesRead);
	for (uint16_t p = 0; p < tag.pagesRead; p++) {
		out.print("Page ");
		out.print(p);
		out.print(": ");
		printHex(out, tag.pages + p * 4, 4);
		out.print('\n');
	}
	out.print("Failed authentication attempts: 0\n");
}

// Parsing

static int hexNibble(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return -1;
}

/// Parses space-separated hex bytes ("AA BB CC"); "??" marks a byte unknown.
/// Returns the number of bytes decoded, or 0 if the line held no byte at all.
static size_t parseHexBytes(const String& s, uint8_t* out, uint8_t* known, size_t maxLen) {
	size_t n = 0;
	int i = 0, len = s.length();
	while (i < len && n < maxLen) {
		while (i < len && s[i] == ' ') i++;
		if (i + 1 >= len) break;
		int hi = hexNibble(s[i]), lo = hexNibble(s[i + 1]);
		if (hi >= 0 && lo >= 0) {
			out[n] = (uint8_t)(hi << 4 | lo);
			if (known) known[n] = 1;
		} else {
			out[n] = 0;
			if (known) known[n] = 0;
		}
		n++;
		i += 2;
	}
	return n;
}

static String valueOf(const String& line, const char* key) {
	String k = String(key) + ": ";
	if (!line.startsWith(k)) return String();
	return line.substring(k.length());
}

static TagType deviceType(const String& name) {
	if (name == "Mifare Classic") return TagType::MIFARE_CLASSIC;
	if (name == "NTAG/Ultralight") return TagType::ULTRALIGHT;
	return TagType::ISO14443_3A;
}

static uint16_t classicBlockCount(const String& subtype) {
	if (subtype == "4K") return 256;
	if (subtype == "Mini") return 20;
	return 64; // "1K" and anything else default to 1K's layout
}

/// Parses a Flipper ".nfc" file (the subset writeFlipperFile() produces: UID/ATQA/SAK
/// and, for Mifare Classic, the per-block data). Ultralight/NTAG pages are not parsed —
/// writing to those tags is not supported yet.
bool parseFlipperFile(Stream& in, Tag& tag) {
	tag.release();
	bool sawFiletype = false;
	static char subtype[16] = {0};

	while (in.available()) {
		String line = in.readStringUntil('\n');
		line.trim();
		if (line.length() == 0 || line.startsWith("#")) continue;

		String v;
		if ((v = valueOf(line, "Filetype")).length()) {
			sawFiletype = (v == "Flipper NFC device");
		} else if ((v = valueOf(line, "Device type")).length()) {
			tag.type = deviceType(v);
		} else if ((v = valueOf(line, "UID")).length()) {
			tag.uidLen = parseHexBytes(v, tag.uid, nullptr, sizeof(tag.uid));
		} else if ((v = valueOf(line, "ATQA")).length()) {
			parseHexBytes(v, tag.atqa, nullptr, sizeof(tag.atqa));
		} else if ((v = valueOf(line, "SAK")).length()) {
			uint8_t sak;
			if (parseHexBytes(v, &sak, nullptr, 1)) tag.sak = sak;
		} else if ((v = valueOf(line, "Mifare Classic type")).length()) {
			strncpy(subtype, v.c_str(), sizeof(subtype) - 1);
			tag.subtype = subtype;
			tag.blockCount = classicBlockCount(v);
			if (!tag.blocks) {
				tag.blocks = (uint8_t*)calloc(tag.blockCount, 16);
				tag.blockKnown = (uint8_t*)calloc(tag.blockCount, 1);
			}
		} else if (line.startsWith("Block ")) {
			int colon = line.indexOf(':');
			if (colon < 0) continue;
			uint16_t b = (uint16_t)line.substring(6, colon).toInt();
			if (!tag.blocks || b >= tag.blockCount) continue;
			uint8_t known[16];
			parseHexBytes(line.substring(colon + 2), tag.blocks + b * 16, known, 16);
			tag.blockKnown[b] = known[0]; // a block's bytes are read/written as one unit
			for (uint8_t i = 1; i < 16 && tag.blockKnown[b]; i++) {
				if (!known[i]) tag.blockKnown[b] = 0;
			}
		}
	}

	if (!sawFiletype || tag.uidLen == 0) {
		tag.release();
		return false;
	}
	if (tag.type == TagType::MIFARE_CLASSIC && tag.blocks) {
		tag.sectorsTotal = tag.blockCount == 256 ? 40 : (tag.blockCount == 64 ? 16 : 5);
	}
	return true;
}

void writeFlipperFile(const Tag& tag, Print& out) {
	out.print("Filetype: Flipper NFC device\n");
	out.print("Version: 4\n");
	out.print("# Device type can be ISO14443-3A, NTAG/Ultralight, Mifare Classic, ...\n");
	printLine(out, "Device type", deviceName(tag.type));
	out.print("# UID is common for all formats\n");
	printHexLine(out, "UID", tag.uid, tag.uidLen);
	out.print("# ISO14443-3A specific data\n");
	printHexLine(out, "ATQA", tag.atqa, sizeof(tag.atqa));
	printHexLine(out, "SAK", &tag.sak, 1);

	if (tag.type == TagType::MIFARE_CLASSIC) writeClassic(tag, out);
	else if (tag.type == TagType::ULTRALIGHT) writeUltralight(tag, out);
}

} // namespace Nfc
