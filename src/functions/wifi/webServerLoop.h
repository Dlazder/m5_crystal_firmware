// pid PID::WEB_SERVER

#include <LittleFS.h>

IPAddress WEB_SERVER_GATEWAY(172, 0, 0, 1);
IPAddress WEB_SERVER_SUBNET(255, 255, 255, 0);

static void _webServerServeFsFile(const String& uri, const String& htmlPath, bool useSd) {
	String path;
	if (uri == "/") {
		path = htmlPath;
	} else {
		// Resolve the requested URI relative to the HTML file's directory
		// (the directory becomes the effective web root). This also prevents
		// path traversal (e.g. "/../creds.txt") via normalization.
		String baseDir = _getDirPath(htmlPath);
		path = _resolveFsPath(uri, baseDir);
		if (path.length() == 0) {
			// Invalid path or traversal attempt — redirect to root
			webServer.sendHeader("Location", "http://" + WEB_SERVER_GATEWAY.toString() + "/", true);
			webServer.send(302, "text/plain", "");
			return;
		}
	}

	File f;
	f = useSd ? SD.open(path) : LittleFS.open(path);
	if (f) {
		webServer.streamFile(f, getContentType(path));
		f.close();
	} else {
		webServer.send(404, "text/plain", "404 Not Found");
	}
}

void webServerLoop() {
	static bool webServerDone = false;

	if (isSetup()) {
		webServerDone = false;
		wifiSsid = getDataString("wifiSsid", "Free WiFi");

		if (selectedFilePath == "") {
			filePickerSetup(PID::WIFI);
		}
	}

	// File picker phase
	if (fpActive) {
		if (filePickerLoop()) return;
		// file selected - fall through to web server setup
	}

	// Web server setup (runs once)
	if (!webServerDone) {
		// AP + DNS
		WiFi.mode(WIFI_AP);
		WiFi.softAP(wifiSsid.c_str(), nullptr); // open network
		WiFi.softAPConfig(WEB_SERVER_GATEWAY, WEB_SERVER_GATEWAY, WEB_SERVER_SUBNET);
		dnsServer.start(53, "*", WEB_SERVER_GATEWAY);

		// Serve files from the selected file's directory
		String htmlPath = selectedFilePath;
		bool useSd = fpSelectedSd;

		webServer.on("/", [htmlPath, useSd]() {
			_webServerServeFsFile("/", htmlPath, useSd);
		});
		webServer.onNotFound([htmlPath, useSd]() {
			_webServerServeFsFile(webServer.uri(), htmlPath, useSd);
		});

		webServer.begin();
		webServerDone = true;
		isWebInterfaceEnabled = true;

		String lines[] = {
			L->TXT_WIFI_WEB_SERVER_RUNNING,
			"SSID: " + wifiSsid,
			"FS: " + selectedFilePath,
			"IP: " + WEB_SERVER_GATEWAY.toString(),
		};
		centeredPrintRows(lines, 4, SMALL_TEXT);
	}

	dnsServer.processNextRequest();
	webServer.handleClient();

	if (checkExit()) {
		selectedFilePath = "";
		fpActive = false;
		webServerDone = false;
	}
}
