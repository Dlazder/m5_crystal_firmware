// Handshake session tracker — counts only complete M1→M2→M3→M4 sequences.
// Call hsProcessFrame() for each captured EAPOL-Key data frame;
// returns true when a full 4-way handshake completes for a given client.

#pragma once
#include <stdint.h>

#define HS_MAX_SESSIONS 4
#define HS_TIMEOUT_MS   30000

struct hs_session_t {
	uint8_t client[6];
	uint8_t ap[6];
	uint8_t step;       // 0=idle, 1=M1, 2=M2, 3=M3
	uint64_t replay;
	uint32_t lastUpdate;
};
static hs_session_t hs_sessions[HS_MAX_SESSIONS];
static int hsDisplayStep = 0; // 0-4, exposed for UI

// Returns true when this EAPOL-Key frame completes a 4-way handshake.
static bool hsProcessFrame(const uint8_t* data) {
	// --- 802.11 header ---
	uint16_t fc = data[0] | (data[1] << 8);
	bool toDS = (fc >> 8) & 1;
	bool fromDS = (fc >> 9) & 1;

	int hdrLen = 24;
	uint8_t subtype = (fc >> 4) & 0xF;
	if (toDS && fromDS) hdrLen += 6;
	if (subtype & 0x8) {
		hdrLen += 2;
		if ((fc >> 15) & 1) hdrLen += 4;
	}

	// --- EAPOL-Key flags ---
	uint16_t keyInfo = (data[hdrLen + 13] << 8) | data[hdrLen + 14];
	bool kAck = (keyInfo >> 7) & 1;
	bool kMic = (keyInfo >> 8) & 1;

	// 1=M1, 2=M2/M4, 3=M3
	uint8_t msg = 0;
	if (kAck && !kMic)      msg = 1;
	else if (!kAck && kMic) msg = 2;
	else if (kAck && kMic)  msg = 3;
	if (!msg) return false;

	// --- Client / AP MAC ---
	const uint8_t *clientMac, *apMac;
	if (toDS && !fromDS) {
		clientMac = data + 10; apMac = data + 4;   // STA→AP
	} else {
		clientMac = data + 4;  apMac = data + 10;  // AP→STA
	}

	// --- Replay counter ---
	uint64_t replay = 0;
	for (int i = 0; i < 8; i++)
		replay = (replay << 8) | data[hdrLen + 17 + i];

	// --- Session lookup ---
	uint32_t now = millis();
	int idx = -1, freeIdx = -1;
	for (int i = 0; i < HS_MAX_SESSIONS; i++) {
		if (hs_sessions[i].step == 0) {
			if (freeIdx < 0) freeIdx = i;
			continue;
		}
		if (now - hs_sessions[i].lastUpdate > HS_TIMEOUT_MS) {
			hs_sessions[i].step = 0;
			if (freeIdx < 0) freeIdx = i;
			continue;
		}
		if (memcmp(hs_sessions[i].client, clientMac, 6) == 0 &&
			memcmp(hs_sessions[i].ap, apMac, 6) == 0) {
			idx = i; break;
		}
	}

	// New session on M1
	if (idx < 0 && msg == 1 && freeIdx >= 0) {
		idx = freeIdx;
		memcpy(hs_sessions[idx].client, clientMac, 6);
		memcpy(hs_sessions[idx].ap, apMac, 6);
		hs_sessions[idx].step = 0;
	}
	if (idx < 0) return false;

	// --- State machine ---
	hs_session_t& s = hs_sessions[idx];
	static int hsCompleteCount = 0; // for log numbering

	if (msg == 1) {
		// M1: start (or restart)
		s.step = 1;
		s.replay = replay;
		s.lastUpdate = now;
		hsDisplayStep = 1;
		Serial.printf("HS %02x:%02x:%02x: M1\n",
			clientMac[3], clientMac[4], clientMac[5]);
	} else if (msg == 2) {
		// M2 or M4 — distinguished by state + replay
		if (s.step == 1 && replay == s.replay) {
			s.step = 2;                     // M1→M2
			s.lastUpdate = now;
			hsDisplayStep = 2;
			Serial.printf("HS %02x:%02x:%02x: M1→M2\n",
				clientMac[3], clientMac[4], clientMac[5]);
		} else if (s.step == 3 && replay == s.replay) {
			// M3→M4 → complete!
			s.step = 0;
			hsCompleteCount++;
			hsDisplayStep = 4;
			Serial.printf("HS %02x:%02x:%02x: M1→M2→M3→M4 ✓ #%d\n",
				clientMac[3], clientMac[4], clientMac[5], hsCompleteCount);
			return true;
		}
	} else if (msg == 3) {
		// M3: must follow M2 with replay+1
		if (s.step == 2 && replay == s.replay + 1) {
			s.step = 3;
			s.replay = replay;
			s.lastUpdate = now;
			hsDisplayStep = 3;
			Serial.printf("HS %02x:%02x:%02x: M1→M2→M3\n",
				clientMac[3], clientMac[4], clientMac[5]);
		}
	}
	return false;
}

static void hsReset() {
	memset(hs_sessions, 0, sizeof(hs_sessions));
	hsDisplayStep = 0;
}

static int hsGetDisplayStep() {
	return hsDisplayStep;
}
