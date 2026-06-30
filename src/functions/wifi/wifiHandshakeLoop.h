// pid PID::WIFI_HANDSHAKE_CAPTURE

// Passive WPA handshake capture via promiscuous mode.
// Listens on the target channel for EAPOL frames to/from the selected
// BSSID, saves them to a PCAP file on SD card, and shows a live
// packet counter. Also captures beacon frames so aircrack-ng can
// match ESSID -> BSSID.

#include "esp_wifi.h"

static const char* HANDSHAKE_CAPTURE_PATH = "/handshake_capture.pcap";

// Ring buffer for passing packets from callback -> main loop

#define HANDSHAKE_RING_SIZE 16

struct pkt_entry {
	uint8_t data[512];
	uint16_t len;
	int8_t rssi;
	uint32_t timestamp;
};

static pkt_entry handshakeRing[HANDSHAKE_RING_SIZE];
static volatile int handshakeRingHead = 0; // producer (callback) write index
static volatile int handshakeRingTail = 0; // consumer (main loop) read index
static int handshakeTotalPackets = 0; // total frames saved (beacons + EAPOL)

// Snapshot of target params, copied on setup (avoid relying on scan pointers).
static uint8_t hsTargetBssid[6];
static int hsTargetChannel;
static String hsTargetSsid;

// Beacon counter — reset each session, cap at 3 to save PCAP space
static int beaconCount = 0;

// Promiscuous callback
// Runs in WiFi task context — no file I/O, no display calls, no long delays.

static void wifiHandshakeSniffCb(void* buf, wifi_promiscuous_pkt_type_t type) {
	// Accept MGMT (beacons) and DATA (EAPOL) frames
	if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;

	wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
	uint16_t frameLen = pkt->rx_ctrl.sig_len; // includes FCS (4 bytes)
	if (frameLen < 28 || frameLen > 512) return; // min: 802.11 hdr(24) + FCS(4)

	uint8_t* frame = pkt->payload;

	// Parse Frame Control
	uint16_t fc = frame[0] | (frame[1] << 8);
	uint8_t frameType = (fc >> 2) & 0x3;
	if (frameType != 0 && frameType != 2) return;

	// Beacon frames (MGMT, subtype 8) - capture for aircrack-ng ESSID
	if (frameType == 0) {
		uint8_t subtype = (fc >> 4) & 0xF;
		if (subtype != 8) return; // Beacon only

		// Beacon Addr3 (offset 16) = BSSID
		if (memcmp(frame + 16, hsTargetBssid, 6) != 0) return;

		if (beaconCount >= 3) {
			static int overflowDbg = 0;
			if (overflowDbg < 3) {
				Serial.printf("Beacon DROP: beaconCount=%d (cap reached)\n", beaconCount);
				overflowDbg++;
			}
			return;
		}
		beaconCount++;

		int nextHead = (handshakeRingHead + 1) % HANDSHAKE_RING_SIZE;
		if (nextHead == handshakeRingTail) return;

		pkt_entry& entry = handshakeRing[handshakeRingHead];
		memcpy(entry.data, frame, frameLen);
		entry.len = frameLen;
		entry.rssi = pkt->rx_ctrl.rssi;
		entry.timestamp = pkt->rx_ctrl.timestamp;
		handshakeRingHead = nextHead;

		Serial.printf("Beacon #%d: len=%u total=%d\n", beaconCount, frameLen, handshakeTotalPackets + (handshakeRingHead - handshakeRingTail + HANDSHAKE_RING_SIZE) % HANDSHAKE_RING_SIZE);
		return;
	}

	// Data frames - EAPOL detection

	bool toDS = (fc >> 8) & 0x1;
	bool fromDS = (fc >> 9) & 0x1;

	// Determine BSSID position in address fields
	uint8_t* bssidPtr = nullptr;
	if (toDS && !fromDS) bssidPtr = frame + 4; // Addr1
	else if (!toDS && fromDS) bssidPtr = frame + 10; // Addr2
	else if (!toDS && !fromDS) bssidPtr = frame + 16; // Addr3
	if (!bssidPtr) return;

	// Filter by target BSSID
	if (memcmp(bssidPtr, hsTargetBssid, 6) != 0) return;

	// Calculate 802.11 header length to find LLC/SNAP
	int hdrLen = 24;
	if (toDS && fromDS) hdrLen += 6; // Addr4

	bool isQoS = false;
	uint8_t subtype = (fc >> 4) & 0xF;
	if (subtype & 0x8) {
		hdrLen += 2; // QoS Control
		isQoS = true;
		// HT Control only present in QoS Data frames when +HTC bit (15) is set
		// In non-QoS frames bit 15 = Order, does NOT add HT Control
		if ((fc >> 15) & 0x1) hdrLen += 4;
	}

	if (frameLen < hdrLen + 8 + 4) return; // need room for LLC/SNAP + FCS

	// Diagnostic: hex dump first 40 bytes of payload for the first few data frames
	static int dataDbgCount = 0;
	if (dataDbgCount < 3) {
		Serial.printf("DATA #%d: fc=0x%04x subtype=%u toDS=%d fromDS=%d "
			"hdrLen=%d frameLen=%u\n",
			dataDbgCount + 1, fc, subtype, toDS, fromDS,
			hdrLen, frameLen);
		Serial.print("	payload: ");
		for (int i = 0; i < (frameLen < 40 ? frameLen : 40); i++)
			Serial.printf("%02x ", frame[i]);
		Serial.println();
		dataDbgCount++;
	}

	// LLC/SNAP: AA AA 03 00 00 00 88 8E (EAPOL)
	uint8_t* llc = frame + hdrLen;
	if (llc[0] == 0xAA && llc[1] == 0xAA && llc[2] == 0x03 &&
		llc[3] == 0x00 && llc[4] == 0x00 && llc[5] == 0x00 &&
		llc[6] == 0x88 && llc[7] == 0x8E) {

		// Verify EAPOL type — must be 3 (EAPOL-Key) for handshake frames
		uint8_t eapolType = llc[9]; // llc+8=version, llc+9=type
		if (eapolType != 3) return; // skip EAPOL-Start, EAPOL-Logoff, etc.

		int nextHead = (handshakeRingHead + 1) % HANDSHAKE_RING_SIZE;
		if (nextHead == handshakeRingTail) return; // ring full

		pkt_entry& entry = handshakeRing[handshakeRingHead];
		memcpy(entry.data, frame, frameLen);
		entry.len = frameLen;
		entry.rssi = pkt->rx_ctrl.rssi;
		entry.timestamp = pkt->rx_ctrl.timestamp;
		handshakeRingHead = nextHead;

		// Diagnostic: verify frame alignment + EAPOL-Key details
		static int eapolDebugCount = 0;
		if (eapolDebugCount < 8) {
			// EAPOL header at llc+8: version(1) + type(1) + length(2, big-endian)
			uint8_t eapolVer = llc[8];
			// eapolType already checked above at llc[9]
			uint16_t eapolDeclared = (llc[10] << 8) | llc[11];
			// Actual EAPOL body = frameLen - 802.11hdr - LLC(8) - EAPOLhdr(4) - FCS(4)
			uint16_t eapolActual = frameLen - hdrLen - 8 - 4 - 4;
			// EAPOL-Key body at llc+12: desc(1) + info(2) + keylen(2) + replay(8) + nonce(32) + ...
			uint8_t keyDesc = llc[12];
			uint16_t keyInfo = (llc[13] << 8) | llc[14];
			uint16_t keyLen = (llc[15] << 8) | llc[16];
			// Decode flags: ack=(7), mic=(8), secure=(9), install=(6)
			bool kAck = (keyInfo >> 7) & 1;
			bool kMic = (keyInfo >> 8) & 1;
			bool kSecure = (keyInfo >> 9) & 1;
			bool kInstall= (keyInfo >> 6) & 1;
			char msgType = (kAck && !kMic) ? '1' : (kMic && !kAck && !kSecure) ? '2'
				: (kAck && kMic && kSecure) ? '3' : (kMic && kSecure && !kAck) ? '4' : '?';
			Serial.printf("EAPOL #%d [M%c]: ver=%u type=%u eapol_decl=%u eapol_actual=%u "
				"key_info=0x%04x key_desc=%u key_len=%u "
				"ack=%d mic=%d sec=%d inst=%d "
				"hdrLen=%d frameLen=%u toDS=%d fromDS=%d isQoS=%d\n",
				eapolDebugCount + 1, msgType,
				eapolVer, eapolType,
				eapolDeclared, eapolActual,
				keyInfo, keyDesc, keyLen,
				kAck, kMic, kSecure, kInstall,
				hdrLen, frameLen, toDS, fromDS, isQoS);
			// Dump first 8 bytes of Key Nonce (offset 13 from EAPOL-Key body = llc+12+13 = llc+25)
			Serial.printf("	nonce[0:8]: %02x %02x %02x %02x %02x %02x %02x %02x\n",
				llc[25], llc[26], llc[27], llc[28],
				llc[29], llc[30], llc[31], llc[32]);
			eapolDebugCount++;
		}
	}
}

void wifiHandshakeLoop() {
	static File pcapFile;
	static bool fileOpen = false;

	if (isSetup()) {
		handshakeRingHead = 0;
		handshakeRingTail = 0;
		handshakeTotalPackets = 0;
		beaconCount = 0;
		fileOpen = false;

		// Snapshot target params (BSSID pointer from scan may become stale)
		memcpy(hsTargetBssid, bssid, 6);
		hsTargetChannel = channel;
		hsTargetSsid = ssid;

		// Open PCAP file on SD card
		if (sdBegin()) {
			if (SD.exists(HANDSHAKE_CAPTURE_PATH))
				SD.remove(HANDSHAKE_CAPTURE_PATH);

			pcapFile = SD.open(HANDSHAKE_CAPTURE_PATH, FILE_WRITE);
			if (pcapFile) {
				writePcapGlobalHeader(pcapFile);
				fileOpen = true;
			}
		}

		// Set WiFi and enable promiscuous sniffing
		WiFi.mode(WIFI_STA);
		esp_wifi_set_promiscuous(true);
		esp_wifi_set_promiscuous_rx_cb(&wifiHandshakeSniffCb);

		// Capture MGMT (beacons) + Data (EAPOL)
		wifi_promiscuous_filter_t filter;
		filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT
			| WIFI_PROMIS_FILTER_MASK_DATA
			| WIFI_PROMIS_FILTER_MASK_DATA_MPDU;
		esp_wifi_set_promiscuous_filter(&filter);

		esp_wifi_set_channel(hsTargetChannel, WIFI_SECOND_CHAN_NONE);

		String lines[] = {
			L->TXT_WIFI_HANDSHAKE_CAPTURING,
			hsTargetSsid,
			"Ch: " + String(hsTargetChannel)
		};
		centeredPrintRows(lines, 3, SMALL_TEXT);

		Serial.println("Handshake capture: ch=" + String(hsTargetChannel) +
			" bssid=" + mac + " ssid=" + hsTargetSsid);
	}

	// Drain ring buffer -> write PCAP packets
	while (handshakeRingTail != handshakeRingHead) {
		if (fileOpen && pcapFile) {
			const pkt_entry& pkt = handshakeRing[handshakeRingTail];
			writePcapPacket(pcapFile, pkt.data, pkt.len, pkt.rssi, pkt.timestamp, hsTargetChannel);
		}
		handshakeRingTail = (handshakeRingTail + 1) % HANDSHAKE_RING_SIZE;
		handshakeTotalPackets++;
	}

	// Flush after new packets to commit to flash
	static int lastFlushedCount = 0;
	if (handshakeTotalPackets > lastFlushedCount && fileOpen && pcapFile) {
		pcapFile.flush();
		lastFlushedCount = handshakeTotalPackets;
	}

	// Refresh display when new packets arrive
	static int lastDisplayedCount = -1;
	if (handshakeTotalPackets != lastDisplayedCount) {
		lastDisplayedCount = handshakeTotalPackets;

		char buf[40];
		snprintf(buf, sizeof(buf), L->TXT_WIFI_HANDSHAKE_PACKETS, handshakeTotalPackets);

		String lines[] = {
			L->TXT_WIFI_HANDSHAKE_CAPTURING,
			hsTargetSsid,
			"Ch: " + String(hsTargetChannel),
			String(buf)
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	if (checkExit()) {
		esp_wifi_set_promiscuous(false);
		esp_wifi_set_promiscuous_rx_cb(nullptr);

		if (fileOpen && pcapFile) {
			pcapFile.flush();
			pcapFile.close();
			fileOpen = false;

			char countBuf[40];
			snprintf(countBuf, sizeof(countBuf), L->TXT_WIFI_HANDSHAKE_PACKETS, handshakeTotalPackets);

			String lines[] = {
				String(countBuf),
				L->TXT_WIFI_HANDSHAKE_SAVED
			};
			centeredPrintRows(lines, 2, MEDIUM_TEXT);
			soundSuccess();
			delay(800);
		} else {
			canvas.clear();
			soundError();
			delay(400);
		}
	}
}
