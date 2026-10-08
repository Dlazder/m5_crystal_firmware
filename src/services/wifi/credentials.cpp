#include "wifi.h"
#include "../storage/storage.h"

namespace Wifi {

static const char WIFI_PASSWORDS_FILE[] = "/wifi_passwords.csv";

static String _wsCsvEscape(const String& s) {
	if (s.indexOf(',') < 0 && s.indexOf('\n') < 0 && s.indexOf('"') < 0) return s;
	String out = "\"";
	for (int i = 0; i < (int)s.length(); i++) {
		if (s[i] == '"') out += "\"\"";
		else out += s[i];
	}
	out += "\"";
	return out;
}

static String _wsCsvUnescape(const String& s) {
	if (s.length() < 2 || s[0] != '"') return s;
	String out;
	for (int i = 1; i < (int)s.length() - 1; i++) {
		if (s[i] == '"' && s[i + 1] == '"') { out += '"'; i++; }
		else out += s[i];
	}
	return out;
}

static bool _wsParseLine(const String& line, String& outSsid, String& outPass) {
	if (line.length() == 0) return false;
	int sep = -1;
	if (line[0] == '"') {
		bool inQ = true;
		int i = 1;
		while (i < (int)line.length()) {
			if (line[i] == '"') {
				if (i + 1 < (int)line.length() && line[i + 1] == '"') { i += 2; continue; }
				inQ = false; i++; break;
			}
			i++;
		}
		if (inQ || i >= (int)line.length() || line[i] != ',') return false;
		sep = i;
	} else {
		sep = line.indexOf(',');
		if (sep < 0) return false;
	}
	outSsid = _wsCsvUnescape(line.substring(0, sep));
	outPass = _wsCsvUnescape(line.substring(sep + 1));
	return true;
}

String loadWifiPassword(const String& targetSsid) {
	if (!Storage::exists(WIFI_PASSWORDS_FILE, true)) return "";

	File f = Storage::open(WIFI_PASSWORDS_FILE, "r", true);
	if (!f) return "";
	while (f.available()) {
		String line = f.readStringUntil('\n');
		line.trim();
		String s, p;
		if (_wsParseLine(line, s, p) && s == targetSsid) {
			f.close();
			return p;
		}
	}
	f.close();
	return "";
}

void saveWifiPassword(const String& targetSsid, const String& password) {
	String newContent;
	bool found = false;

	if (Storage::exists(WIFI_PASSWORDS_FILE, true)) {
		File f = Storage::open(WIFI_PASSWORDS_FILE, "r", true);
		if (f) {
			while (f.available()) {
				String line = f.readStringUntil('\n');
				line.trim();
				if (line.length() == 0) continue;
				String s, p;
				if (_wsParseLine(line, s, p) && s == targetSsid) {
					found = true;
					newContent += _wsCsvEscape(targetSsid) + "," + _wsCsvEscape(password) + "\n";
				} else {
					newContent += line + "\n";
				}
			}
			f.close();
		}
	}

	if (!found)
		newContent += _wsCsvEscape(targetSsid) + "," + _wsCsvEscape(password) + "\n";

	File f = Storage::open(WIFI_PASSWORDS_FILE, "w", true);
	if (f) {
		f.print(newContent);
		f.close();
	}
}

}