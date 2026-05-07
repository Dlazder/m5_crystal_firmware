// PID::WEATHER

#include <WiFiClient.h>

static String weatherCity = "";
static String weatherTemp = "";
static String weatherDesc = "";
static String weatherHumidity = "";
static bool weatherLoading = false;
static bool weatherLoaded = false;
static bool weatherNoWifi = false;

static String httpGet(const char* host, const char* path) {
	WiFiClient client;
	if (!client.connect(host, 80)) {
		Serial.printf("[weather] connect failed: %s\n", host);
		return "";
	}

	client.print(String("GET ") + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n");

	unsigned long timeout = millis();
	while (client.connected()) {
		if (millis() - timeout > 8000) { client.stop(); return ""; }
		if (client.available()) {
			String line = client.readStringUntil('\n');
			if (line == "\r") break; // end of headers
		}
	}

	String body = "";
	timeout = millis();
	while (client.connected() || client.available()) {
		if (millis() - timeout > 8000) break;
		if (client.available()) {
			body += client.readStringUntil('\n');
		}
	}
	client.stop();
	Serial.printf("[weather] %s response len: %d\n", host, body.length());
	Serial.println(body.substring(0, 200));
	return body;
}

static String extractJson(const String& body, const char* key) {
	String search = String("\"") + key + "\": \"";
	int idx = body.indexOf(search);
	if (idx < 0) {
		search = String("\"") + key + "\":\"";
		idx = body.indexOf(search);
	}
	if (idx < 0) return "";
	idx += search.length();
	return body.substring(idx, body.indexOf("\"", idx));
}

static void fetchWeather() {
	// get city from IP
	String geoBody = httpGet("ip-api.com", "/json/?fields=city");
	weatherCity = extractJson(geoBody, "city");
	if (weatherCity.length() == 0) weatherCity = "Unknown";

	// get weather from wttr.in
	String wttrPath = String("/") + weatherCity + "?format=j1";
	wttrPath.replace(" ", "+");
	String wttrBody = httpGet("wttr.in", wttrPath.c_str());

	weatherTemp = extractJson(wttrBody, "temp_C");
	if (weatherTemp.length() > 0) weatherTemp = "+" + weatherTemp + " C";

	weatherDesc = extractJson(wttrBody, "value");
	if (weatherDesc.length() > 25) weatherDesc = weatherDesc.substring(0, 22) + "...";

	weatherHumidity = extractJson(wttrBody, "humidity");
	if (weatherHumidity.length() > 0) weatherHumidity = "Hum: " + weatherHumidity + "%";

	weatherLoaded = true;
}

void weatherLoop() {
	if (isSetup()) {
		weatherCity = "";
		weatherTemp = "";
		weatherDesc = "";
		weatherHumidity = "";
		weatherLoading = false;
		weatherLoaded = false;
		weatherNoWifi = WiFi.status() != WL_CONNECTED;

		if (weatherNoWifi) {
			centeredPrint("No WiFi", MEDIUM_TEXT);
		} else {
			centeredPrint("Loading...", MEDIUM_TEXT);
		}
	}

	if (!weatherNoWifi && !weatherLoading) {
		weatherLoading = true;
		fetchWeather();
	}

	if (weatherLoaded) {
		String lines[] = { weatherCity, weatherTemp, weatherDesc, weatherHumidity };
		centeredPrintRows(lines, 4, SMALL_TEXT);
		weatherLoaded = false;
	}

	checkExit();
}
