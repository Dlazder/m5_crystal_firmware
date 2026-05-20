// TV power-off IR codes
// Sources: TV-B-Gone (Mitch Altman), IRDB (probonopd), Tasmota, LIRC
// Format: { protocol, address, command, bits }
// bits: only used for SONY (12/15/20); set 0 for all other protocols

struct TvbCode {
	decode_type_t protocol;
	uint16_t address;
	uint16_t command;
	uint8_t bits;  // for SONY: 12/15/20; 0 for all others
};

static const TvbCode TVB_CODES[] = {

	// SAMSUNG
	{ SAMSUNG,   0xE0E0, 0x40BF, 0 },  // Universal power toggle
	{ SAMSUNG,   0xE0E0, 0x19E6, 0 },  // Older models
	{ SAMSUNG,   0x0707, 0x02FD, 0 },  // Series 3/4/5 alt
	{ SAMSUNG,   0xE0E0, 0xC43B, 0 },  // 2018+ Smart TV
	{ SAMSUNG,   0xE0E0, 0x54AB, 0 },  // Q-series
	{ SAMSUNG,   0x0000, 0x0002, 0 },  // Legacy CRT
	{ SAMSUNG,   0x0007, 0x02FD, 0 },  // LE/UE series alt

	// LG
	{ NEC,       0x20DF, 0x10EF, 0 },  // Standard power toggle
	{ NEC,       0x20DF, 0x23DC, 0 },  // Power on
	{ NEC,       0x20DF, 0xA35C, 0 },  // Power off
	{ NEC,       0x0004, 0x0050, 0 },  // OLED series
	{ NEC,       0x00B0, 0x00F8, 0 },  // Older LG
	{ NEC,       0x00F8, 0x00F7, 0 },  // LG Flatron
	{ NEC,       0x2583, 0xC03F, 0 },  // LG 2020+
	{ NEC,       0x04CB, 0x0102, 0 },  // LG NanoCell

	// SONY
	{ SONY,      0x0001, 0x0015, 12 }, // SIRC-12 standard
	{ SONY,      0x0001, 0x0015, 15 }, // SIRC-15
	{ SONY,      0x0001, 0x0015, 20 }, // SIRC-20
	{ SONY,      0x0041, 0x0015, 20 }, // Bravia KDL series
	{ SONY,      0x0081, 0x0015, 20 }, // Bravia XR series
	{ SONY,      0x000E, 0x0015, 12 }, // Sony Trinitron
	{ SONY,      0x0001, 0x002E, 12 }, // Sony KV series alt
	{ SONY,      0x0051, 0x0015, 20 }, // Sony XBR series
	{ SONY,      0x0021, 0x0015, 12 }, // Sony WEGA

	// PHILIPS RC5
	{ RC5,       0x0000, 0x000C, 0 },  // Standard RC5
	{ RC5,       0x0001, 0x000C, 0 },  // Alternate device code
	{ RC5,       0x0000, 0x006C, 0 },  // Extended RC5
	{ RC5,       0x0003, 0x000C, 0 },  // Philips alt

	// PHILIPS RC6
	{ RC6,       0x0000, 0x000C, 0 },  // RC6 mode 0
	{ RC6,       0x0000, 0x008C, 0 },  // RC6 mode 0 alt

	// PANASONIC
	{ PANASONIC, 0x4004, 0x100C, 0 },  // Viera standard
	{ PANASONIC, 0x4008, 0x100C, 0 },  // Viera alt
	{ PANASONIC, 0x4004, 0x3D82, 0 },  // Plasma series
	{ PANASONIC, 0x4004, 0x0100, 0 },  // TX series
	{ PANASONIC, 0x4005, 0x100C, 0 },  // Viera 2014+
	{ PANASONIC, 0x4006, 0x100C, 0 },  // Viera 2016+
	{ PANASONIC, 0x4004, 0x0001, 0 },  // Legacy Panasonic

	// SHARP
	{ SHARP,     0x0002, 0x0020, 0 },  // Aquos standard
	{ SHARP,     0x0045, 0x0020, 0 },  // Aquos alt
	{ SHARP,     0x0002, 0x0021, 0 },  // LC series
	{ SHARP,     0x0001, 0x007C, 0 },  // Older Sharp

	// DENON
	{ DENON,     0x0002, 0x004B, 0 },  // Standard
	{ DENON,     0x0004, 0x004B, 0 },  // Alt

	// JVC
	{ JVC,       0x00C5, 0x0003, 0 },  // Standard
	{ JVC,       0x00F7, 0x0000, 0 },  // Alt

	// TOSHIBA
	{ NEC,       0x02FB, 0x002D, 0 },  // Regza series
	{ NEC,       0x00FF, 0x00FE, 0 },  // Older Toshiba
	{ NEC,       0x00BF, 0x0000, 0 },  // CT-90 remote
	{ NEC,       0x02FB, 0x00D2, 0 },  // Toshiba 2015+
	{ NEC,       0x40BF, 0x00FF, 0 },  // Toshiba Fire TV
	{ NEC,       0x02FB, 0x0027, 0 },  // Regza alt

	// HITACHI
	{ NEC,       0x0068, 0x0070, 0 },  // Standard
	{ NEC,       0x07F8, 0x0708, 0 },  // UltraVision
	{ NEC,       0x0013, 0x0018, 0 },  // Older Hitachi
	{ NEC,       0x0249, 0x00A8, 0 },  // Wooo series

	// HISENSE
	{ NEC,       0x00FD, 0x000B, 0 },  // Standard NEC
	{ NEC,       0x00FD, 0x008B, 0 },  // Power off specific
	{ NEC,       0x4CB3, 0x807F, 0 },  // H6/H8 series
	{ NEC,       0x0000, 0x00F2, 0 },  // Older Hisense
	{ NEC,       0xC003, 0x00F2, 0 },  // U8/U9 series

	// TCL
	{ NEC,       0x57E3, 0x18E7, 0 },  // 5/6-series
	{ NEC,       0x57E3, 0x16E9, 0 },  // Alt
	{ NEC,       0x4CB3, 0x20DF, 0 },  // Older TCL
	{ NEC,       0xFF00, 0x0F01, 0 },  // TCL Roku TV

	// VIZIO 
	{ NEC,       0xFB04, 0x09F6, 0 },  // Standard
	{ NEC,       0x1AE5, 0x00C1, 0 },  // M/P series
	{ NEC,       0x7F80, 0x0002, 0 },  // Older Vizio
	{ NEC,       0xFB04, 0x4FB0, 0 },  // D-series

	// PIONEER 
	{ NEC,       0x00A5, 0x0016, 0 },  // Kuro plasma
	{ NEC,       0x00D0, 0x00BE, 0 },  // LCD series
	{ NEC,       0xAA55, 0x0016, 0 },  // Elite series

	// SANYO 
	{ NEC,       0x00E0, 0x001C, 0 },  // Standard
	{ NEC,       0x00E0, 0x001D, 0 },  // Alt code
	{ NEC,       0x0040, 0x001C, 0 },  // Older Sanyo

	// MITSUBISHI 
	{ NEC,       0x05A2, 0x00C1, 0 },  // Diamond series
	{ NEC,       0x05A2, 0x0049, 0 },  // Laservue
	{ PANASONIC, 0x0221, 0x100C, 0 },  // Mitsubishi Kaseikyo

	// ONKYO 
	{ ONKYO,     0x04CB, 0x0423, 0 },  // Standard
	{ ONKYO,     0x04CB, 0x0400, 0 },  // Alt

	// PHILIPS / MAGNAVOX 
	{ RC5,       0x0000, 0x000C, 0 },  // Magnavox (RC5)
	{ NEC,       0x0092, 0x0015, 0 },  // Philips NEC variant
	{ NEC,       0x00BE, 0x00F7, 0 },  // Magnavox/Funai

	// RCA / GE 
	{ NEC,       0x00EE, 0x00F7, 0 },  // RCA standard
	{ NEC,       0x00BE, 0x00E8, 0 },  // GE/RCA common
	{ NEC,       0x00F7, 0x00EE, 0 },  // GE alt

	// AOC
	{ NEC,       0x0040, 0x0038, 0 },  // Standard
	{ NEC,       0x0040, 0x003F, 0 },  // Alt

	// HAIER
	{ NEC,       0x0040, 0x0000, 0 },  // Standard
	{ NEC,       0x1FE0, 0x807F, 0 },  // Newer Haier

	// FUNAI / SYLVANIA
	{ NEC,       0x00BE, 0x00F7, 0 },  // Funai standard
	{ NEC,       0x00BE, 0x00DB, 0 },  // Sylvania

	// EMERSON 
	{ NEC,       0x00BE, 0x00F7, 0 },  // Standard
	{ NEC,       0x5EA1, 0x1AE5, 0 },  // Emerson/Orion

	// INSIGNIA / BEST BUY 
	{ NEC,       0x20DF, 0x10EF, 0 },  // LG-based
	{ SAMSUNG,   0xE0E0, 0x40BF, 0 },  // Samsung-based OEM

	// POLAROID / WESTINGHOUSE 
	{ NEC,       0x0092, 0x0000, 0 },  // Westinghouse
	{ NEC,       0x4CB3, 0x3DC2, 0 },  // Polaroid

	// ONN (Walmart) 
	{ NEC,       0xFF00, 0x0A05, 0 },  // ONN Roku TV
	{ NEC,       0x40BF, 0x3DC2, 0 },  // ONN standard

	// ELEMENT 
	{ NEC,       0x5EA1, 0x00FF, 0 },  // Standard
	{ NEC,       0x5EA1, 0x1AE5, 0 },  // Alt

	// SKYWORTH / COOCAA 
	{ NEC,       0x00FD, 0x00F2, 0 },  // Skyworth
	{ NEC,       0x40BF, 0x807F, 0 },  // Coocaa

	// XIAOMI / MI TV 
	{ NEC,       0x6B86, 0x0001, 0 },  // Mi TV 4A/4X
	{ NEC,       0x0D00, 0x0001, 0 },  // Mi TV 3S

	// CHANGHONG / CHiQ 
	{ NEC,       0x00BF, 0x00FC, 0 },  // Changhong standard
	{ NEC,       0x40BF, 0x00FC, 0 },  // CHiQ

	// GRUNDIG 
	{ RC5,       0x0000, 0x000C, 0 },  // RC5 compatible
	{ NEC,       0x00CC, 0x0087, 0 },  // Newer Grundig

	// LOEWE 
	{ RC5,       0x0000, 0x000C, 0 },  // RC5 standard
	{ RC6,       0x0000, 0x000C, 0 },  // Newer Loewe

	// BEKO / ARÇELIK 
	{ NEC,       0x40BF, 0x00FF, 0 },  // Standard
	{ NEC,       0x40BF, 0x807F, 0 },  // Alt

	// VESTEL 
	{ NEC,       0x01FE, 0x0CF3, 0 },  // Standard (also Telefunken, Finlux)
	{ NEC,       0x01FE, 0x48B7, 0 },  // Alt

	// TELEFUNKEN 
	{ NEC,       0x01FE, 0x0CF3, 0 },  // Vestel platform
	{ RC5,       0x0000, 0x000C, 0 },  // Old Telefunken

	// FINLUX / WHARFEDALE 
	{ NEC,       0x01FE, 0x0CF3, 0 },  // Vestel platform
	{ NEC,       0x00BE, 0x00DB, 0 },  // Wharfedale

	// BUSH / ALBA (Vestel) 
	{ NEC,       0x01FE, 0x0CF3, 0 },

	// FUJITSU 
	{ KASEIKYO,  0x0220, 0x100C, 0 },  // Kaseikyo platform
	{ NEC,       0x0098, 0x000A, 0 },  // Older Fujitsu

	// MARANTZ 
	{ DENON,     0x0002, 0x004B, 0 },  // Same as Denon platform
	{ NEC,       0x02CB, 0x004B, 0 },  // Marantz NEC variant

	// DAEWOO 
	{ NEC,       0x00FF, 0x003D, 0 },  // Standard
	{ NEC,       0x00FF, 0x00C2, 0 },  // Alt

	// ORION / SANSUI 
	{ NEC,       0x5EA1, 0x1AE5, 0 },  // Orion standard
	{ NEC,       0x00BE, 0x00F7, 0 },  // Sansui

	// MEDION 
	{ NEC,       0x03FC, 0x0CF3, 0 },  // Standard
	{ RC5,       0x0000, 0x000C, 0 },  // Older Medion

	// KOGAN 
	{ NEC,       0x40BF, 0x00F7, 0 },  // Standard
	{ NEC,       0x1FE0, 0x40BF, 0 },  // Newer Kogan

	// AMAZON FIRE TV 
	{ NEC,       0x40BF, 0x3DC2, 0 },  // Fire TV Stick
	{ NEC,       0xFF00, 0x00F7, 0 },  // Fire TV Cube

	// ROKU TV (TCL/Hisense/etc.) 
	{ NEC,       0xFF00, 0x0A05, 0 },  // Standard Roku remote
	{ NEC,       0xFF00, 0x0F01, 0 },  // Alt Roku

	// ANDROID TV (Sony/Sharp/etc.) 
	{ SONY,      0x0001, 0x0015, 12 }, // Sony Android TV (same as Bravia)
	{ NEC,       0x00FD, 0x000B, 0 },  // Hisense Android TV

	// ADDITIONAL NEC GENERICS 
	{ NEC,       0x0020, 0x00DF, 0 },  // Generic NEC-1
	{ NEC,       0x0000, 0x00F2, 0 },  // Generic NEC-2
	{ NEC,       0x00FF, 0x00FE, 0 },  // Generic NEC-3
	{ NEC,       0x00EE, 0x00E8, 0 },  // Generic NEC-4
	{ NEC,       0x00CC, 0x003C, 0 },  // Generic NEC-5
	{ NEC,       0x00AA, 0x0055, 0 },  // Generic NEC-6
	{ NEC,       0x00F0, 0x00FF, 0 },  // Generic NEC-7

};

static const int TVB_COUNT = sizeof(TVB_CODES) / sizeof(TvbCode);
