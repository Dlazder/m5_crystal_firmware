// WiFi credential storage on LittleFS: /wifi_passwords.csv (ssid,password)

#define WIFI_PASSWORDS_FILE "/wifi_passwords.csv"

/**
 * @brief Wraps a CSV field in double quotes if it contains commas, newlines, or quotes.
 *        Internal double quotes are escaped as "". Fields without special chars are returned as-is.
 * @param s Source string to escape.
 * @return Escaped CSV field.
 */
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

/**
 * @brief Removes CSV quoting from a single field.
 *        Expects the field to start and end with '"'; "" sequences become ".
 *        Unquoted fields are returned unchanged.
 * @param s Possibly-quoted CSV field.
 * @return Unescaped string value.
 */
static String _wsCsvUnescape(const String& s) {
	if (s.length() < 2 || s[0] != '"') return s;
	String out;
	for (int i = 1; i < (int)s.length() - 1; i++) {
		if (s[i] == '"' && s[i + 1] == '"') { out += '"'; i++; }
		else out += s[i];
	}
	return out;
}

/**
 * @brief Parses one line from wifi_passwords.csv into its two fields.
 *        Handles both plain and double-quoted fields (RFC 4180 subset).
 * @param line Raw CSV line (no trailing newline).
 * @param outSsid Receives the unescaped SSID field.
 * @param outPass Receives the unescaped password field.
 * @return true on success, false if the line is empty or malformed.
 */
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
	outPass  = _wsCsvUnescape(line.substring(sep + 1));
	return true;
}

/**
 * @brief Looks up the saved password for a given SSID in wifi_passwords.csv.
 * @param targetSsid Full SSID string to search for (case-sensitive, any length).
 * @return Saved password, or empty string if not found or file does not exist.
 */
String loadWifiPassword(const String& targetSsid) {
	if (!LittleFS.exists(WIFI_PASSWORDS_FILE)) return "";
	File f = LittleFS.open(WIFI_PASSWORDS_FILE, "r");
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

/**
 * @brief Saves or updates the password for a given SSID in wifi_passwords.csv.
 *        If the SSID already has an entry it is overwritten in place; otherwise
 *        a new line is appended. The entire file is rewritten on each call.
 * @param targetSsid Full SSID string (any length, special chars are CSV-escaped).
 * @param password Password to store.
 */
void saveWifiPassword(const String& targetSsid, const String& password) {
	String newContent;
	bool found = false;

	if (LittleFS.exists(WIFI_PASSWORDS_FILE)) {
		File f = LittleFS.open(WIFI_PASSWORDS_FILE, "r");
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

	File f = LittleFS.open(WIFI_PASSWORDS_FILE, "w");
	if (f) {
		f.print(newContent);
		f.close();
	}
}
