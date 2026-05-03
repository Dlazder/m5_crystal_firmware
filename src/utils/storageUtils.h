/**
 * Writes data to internal memory.
 * @param key data key
 * @param data integer value
 */
void setData(const char* key, int data) {
	preferences.putUInt(key, data);
}


/**
 * Get data from internal memory.
 * @param key data key
 * @param defaultValue value that will be returned if the data does not exist
 */
int getData(const char* key, int defaultValue) {
	return preferences.getUInt(key, defaultValue);
}


/**
 * Writes a string value to internal memory.
 * @param key data key
 * @param data string value
 */
void setDataString(const char* key, const char* data) {
	preferences.putString(key, data);
}


/**
 * Get a string value from internal memory.
 * @param key data key
 * @param defaultValue value that will be returned if the data does not exist
 */
String getDataString(const char* key, const char* defaultValue = "") {
	return preferences.getString(key, defaultValue);
}