// PID::WIFI_TIME_SYNC

#include <WiFiUdp.h>
#include <NTPClient.h>

static WiFiUDP ntpUDP;
static NTPClient* ntpClient = nullptr;

static bool ntpSyncing = false;
static bool ntpDone = false;
static bool ntpFailed = false;

static void NtpSync() {
	ntpClient->begin();
	ntpClient->setTimeOffset(0);

	unsigned long start = millis();
	bool updated = false;
	while (millis() - start < 10000) {
		if (ntpClient->forceUpdate()) { updated = true; break; }
		delay(200);
	}

	if (!updated) {
		ntpFailed = true;
		ntpDone = true;
		return;
	}

	unsigned long epoch = ntpClient->getEpochTime();
	struct timeval tv = { (time_t)epoch, 0 };
	settimeofday(&tv, nullptr);

	ntpDone = true;
}

void wifiTimeSyncLoop() {
	if (isSetup()) {
		ntpSyncing = false;
		ntpDone = false;
		ntpFailed = false;
		if (!WiFi.isConnected()) {
			centeredPrint(L->TXT_WIFI_NOT_CONNECTED, MEDIUM_TEXT);
			return;
		}

		if (ntpClient) {
			ntpClient->end();
			delete ntpClient;
		}
		ntpClient = new NTPClient(ntpUDP, "pool.ntp.org");

		centeredPrint(L->TXT_CONNECTING, MEDIUM_TEXT);
	}

	if (!ntpSyncing && ntpClient) {
		ntpSyncing = true;
		NtpSync();
	}

	if (ntpDone) {
		if (ntpFailed) {
			centeredPrint(L->TXT_WIFI_SYNC_FAILED, MEDIUM_TEXT);
		} else {
			centeredPrint(L->TXT_WIFI_TIME_SYNCED, MEDIUM_TEXT);
		}
		ntpDone = false;
	}

	checkExit();
}
