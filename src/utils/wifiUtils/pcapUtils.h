// PCAP file format utilities: structures and helpers for writing .pcap files
// with IEEE 802.11 + Radiotap headers (LINKTYPE_IEEE802_11_RADIOTAP = 127).

// --- PCAP file format structures -------------------------------------------
// __attribute__((packed)) ensures exact on-disk layout (no compiler padding).

struct __attribute__((packed)) pcap_hdr_s {
	uint32_t magic_number;   // 0xa1b2c3d4
	uint16_t version_major;  // 2
	uint16_t version_minor;  // 4
	int32_t  thiszone;       // GMT to local offset (0)
	uint32_t sigfigs;        // timestamp accuracy (0)
	uint32_t snaplen;        // max packet length
	uint32_t network;        // link type: 127 = IEEE 802.11 + radiotap
};

struct __attribute__((packed)) pcaprec_hdr_s {
	uint32_t ts_sec;
	uint32_t ts_usec;
	uint32_t incl_len;
	uint32_t orig_len;
};

// --- Radiotap header (16 bytes, packed) ------------------------------------
// Present bits: 1 (Flags) | 3 (Channel) | 5 (dBm Signal)
// Fields must appear in increasing present-bit order: 1, 3, 5.
// flags = 0x10 (FCS at end) — ESP32 sig_len includes FCS (4 bytes).

struct __attribute__((packed)) radiotap_hdr {
	uint8_t  it_version;     // 0
	uint8_t  it_pad;         // 1
	uint16_t it_len;         // 2-3
	uint32_t it_present;     // 4-7: bitmask of present fields
	uint8_t  flags;          // 8:  bit 1 — Flags (0x10 = FCS at end)
	uint8_t  pad1;           // 9:  align Channel to 2 bytes
	uint16_t chan_freq;      // 10-11: bit 3 — Channel freq (MHz)
	uint16_t chan_flags;     // 12-13: bit 3 — Channel flags
	int8_t   ant_signal;     // 14: bit 5 — dBm Antenna Signal
	uint8_t  pad2;           // 15: pad to 16 bytes
};

// --- Channel helpers --------------------------------------------------------

static uint16_t channelToFreq(int ch) {
	if (ch >= 1 && ch <= 13) return 2412 + (ch - 1) * 5;
	if (ch == 14)            return 2484;
	if (ch >= 36 && ch <= 196) return 5000 + ch * 5;
	return 2412; // fallback
}

static uint16_t channelFlagsFor(int ch) {
	return (ch >= 1 && ch <= 14) ? 0x000a   // CCK + 2 GHz spectrum
	                             : 0x0014;  // OFDM + 5 GHz spectrum
}

// --- PCAP write helpers -----------------------------------------------------

static void writePcapGlobalHeader(File& f) {
	pcap_hdr_s hdr;
	hdr.magic_number  = 0xa1b2c3d4;
	hdr.version_major = 2;
	hdr.version_minor = 4;
	hdr.thiszone      = 0;
	hdr.sigfigs       = 0;
	hdr.snaplen       = 65535;
	hdr.network       = 127; // LINKTYPE_IEEE802_11_RADIOTAP
	f.write((uint8_t*)&hdr, sizeof(hdr));
}

static void writePcapPacket(File& f, const uint8_t* data, uint16_t len,
                            int8_t rssi, uint32_t timestamp, int channel) {
	// PCAP record header
	pcaprec_hdr_s rec;
	rec.ts_sec   = timestamp / 1000000;
	rec.ts_usec  = timestamp % 1000000;
	rec.incl_len = sizeof(radiotap_hdr) + len;
	rec.orig_len = sizeof(radiotap_hdr) + len;
	f.write((uint8_t*)&rec, sizeof(rec));

	// Radiotap header
	radiotap_hdr rt;
	memset(&rt, 0, sizeof(rt));
	rt.it_version = 0;
	rt.it_len     = sizeof(radiotap_hdr);
	rt.it_present = (1 << 1) | (1 << 3) | (1 << 5); // Flags + Channel + Signal
	rt.flags      = 0x10; // FCS at end (ESP32 sig_len includes FCS)
	rt.chan_freq  = channelToFreq(channel);
	rt.chan_flags = channelFlagsFor(channel);
	rt.ant_signal = rssi;
	f.write((uint8_t*)&rt, sizeof(rt));

	// Raw 802.11 frame (includes FCS at end — radiotap flags=0x10 tells parsers)
	f.write(data, len);
}
