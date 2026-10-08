#include "wifi.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <sys/time.h>

namespace Wifi {

void autoNtpSync(int timezoneOffset) {
	if (!WiFi.isConnected()) return;

	WiFiUDP udp;
	NTPClient client(udp, "pool.ntp.org");
	client.begin();
	client.setTimeOffset(timezoneOffset);

	for (int i = 0; i < 5; i++) {
		if (client.forceUpdate()) {
			unsigned long epoch = client.getEpochTime();
			struct timeval tv = { (time_t)epoch, 0 };
			settimeofday(&tv, nullptr);
			Serial.println("NTP auto-sync OK");
			break;
		}
		delay(500);
	}
	client.end();
}

}
