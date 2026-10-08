// Wifi service — offline crypto (pcap -> ft-crack hash line, PMKID -> mode-22000)
// and live 802.11 attack primitives (deauth, handshake-session tracking, WPS/FT-PSK
// beacon probe), plus WiFi credential storage and NTP sync.

#pragma once
#include <Arduino.h>
#include <FS.h>
#include "esp_wifi.h"

namespace Wifi {

// PCAP -> ft-crack hash line (offline)

// Write the pcap global header (LINKTYPE_IEEE802_11_RADIOTAP) to an open file.
void writePcapGlobalHeader(File& f);

// Write one radiotap + 802.11 frame as a pcap record.
void writePcapPacket(File& f, const uint8_t* data, uint16_t len,
										int8_t rssi, uint32_t timestamp, int channel);

// Extract the EAPOL 4-way handshake from a .pcap and write a WPA*04* hash line
// (ft-crack / mode 22000) to a same-named .hash file. Returns true on success.
// `useLittleFS` selects the backend (see Storage::open).
bool pcapToFTHash(const String& pcapPath, bool useLittleFS = false);

// PMKID capture (offline parse + raw frame builders)

// Walk tagged IEs for an RSN IE (tag 0x30) carrying a PMKID; copy it to `out`.
bool pmkScanIEs(const uint8_t* data, int len, uint8_t* out);

// Write one WPA*01* (mode 22000 PMKID) hash line to an open file.
void writePmkidHashLine(File& f, const uint8_t* pmkid, const uint8_t* apMac,
												const uint8_t* staMac, const uint8_t* ssid, uint8_t ssidLen);

// Send a raw 802.11 frame via the AP interface (esp_wifi_80211_tx).
void pmkSendRawFrame(const uint8_t* buf, int len);

// Build an Open System Authentication frame (30 bytes) into `buf`.
void pmkBuildAuthFrame(uint8_t* buf, const uint8_t* bssid,
											const uint8_t* staMac, uint16_t seq);

// Build an Association Request frame with an RSN IE carrying a fake PMKID.
// Returns the total frame length.
int pmkBuildAssocFrame(uint8_t* buf, const uint8_t* bssid, const uint8_t* staMac,
											const uint8_t* ssid, int ssidLen, uint16_t seq);

// Deauthentication

// Reset the internal deauth frame to its broadcast template. Call once before a
// deauth session (the frame buffer was previously mutated in place by the loops).
void deauthResetFrame();

// Send a raw 802.11 frame via the AP interface.
void deauthSendRawFrame(const uint8_t* frame_buffer, int size);

// Send a deauth frame spoofing the given BSSID on the given channel.
void deauthSendFrame(const uint8_t* bssid, int chan);

// Handshake session tracker

// Feed one captured EAPOL-Key data frame; returns true when a full M1->M2->M3->M4
// handshake completes. Drive hsGetDisplayStep() for progress.
bool hsProcessFrame(const uint8_t* data);
void hsReset();
int hsGetDisplayStep();

// AP info probe (WPS / FT-PSK beacon detection)

void wiuBegin(uint8_t* targetBssid, int targetChannel);
void wiuUpdate();
bool wiuDone();
bool wiuHasWps();
bool wiuHasFt();
void wiuCleanup();
const char* wiuAuthStr(wifi_auth_mode_t m);

// WiFi credential storage (LittleFS CSV) 

// Look up the saved password for `targetSsid`; empty string if not found.
String loadWifiPassword(const String& targetSsid);

// Save/update the password for `targetSsid` (rewrites the CSV).
void saveWifiPassword(const String& targetSsid, const String& password);

// NTP sync

// Lightweight NTP sync after WiFi connects; returns silently. `timezoneOffset`
// is the firmware's configured offset (seconds).
void autoNtpSync(int timezoneOffset);

} // namespace Wifi
