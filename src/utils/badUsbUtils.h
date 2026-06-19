#ifdef ESP32S3

unsigned long badUsbDelayUntil = 0;

// Script lines loaded via readFileLines and the index of the line to run next.
// Used to render the script on screen with the current line highlighted.
String* badUsbLines = nullptr;
int badUsbLineCount = 0;
int badUsbCurrentLine = 0;

/**
 * Frees the loaded script lines.
 */
void badUsbFreeLines() {
  if (badUsbLines != nullptr) { delete[] badUsbLines; badUsbLines = nullptr; }
  badUsbLineCount = 0;
  badUsbCurrentLine = 0;
}

/**
 * Loads the selected script file and resets execution state to the first line.
 * @return true if loaded successfully, false if the file could not be read.
 */
bool badUsbSetScript() {
  badUsbDelayUntil = 0;
  badUsbFreeLines();
  return readFileLines(selectedFilePath, badUsbLines, badUsbLineCount);
}

/**
 * Returns true if the parser is currently waiting for a DELAY command to expire.
 * Call this before badUsbNextLine() to avoid executing lines during a delay.
 * @return true if delay is still active, false if ready to proceed
 */
bool badUsbIsDelaying() {
  return millis() < badUsbDelayUntil;
}

/**
 * Parses and executes the next line of the active script.
 * Supports: DELAY, STRING, KEY, COMBO, REM, //.
 * Key names are resolved through badBleResolveKey: the BLE HID keycodes it
 * returns are standard USB HID usages, so the native keyboard accepts them.
 * @return true if there are more lines to execute, false when script is finished
 */
bool badUsbNextLine() {
  if (badUsbLines == nullptr || badUsbCurrentLine >= badUsbLineCount) return false;

  // The line about to run; badUsbCurrentLine drives the on-screen highlight.
  String line = badUsbLines[badUsbCurrentLine];
  badUsbCurrentLine++;

  if (line.length() == 0 || line.startsWith("REM") || line.startsWith("//"))
    return true;

  if (line.startsWith("DELAY ")) {
    badUsbDelayUntil = millis() + (unsigned long)line.substring(6).toInt();
  } else if (line.startsWith("STRING ")) {
    String text = line.substring(7);
    for (int i = 0; i < (int)text.length(); i++) {
      usbKeyboard.write((uint8_t)text[i]);
      delay(10);
    }
  } else if (line.startsWith("KEY ")) {
    String keyName = line.substring(4);
    keyName.trim();
    uint8_t k = badBleResolveKey(keyName);
    if (k) usbKeyboard.write(k);
  } else if (line.startsWith("COMBO ")) {
    String args = line.substring(6);
    args.trim();
    while (args.length() > 0) {
      int sp = args.indexOf(' ');
      String token = (sp == -1) ? args : args.substring(0, sp);
      token.trim();
      uint8_t k = badBleResolveKey(token);
      if (!k && token.length() == 1) k = (uint8_t)token[0];
      if (k) usbKeyboard.press(k);
      args = (sp == -1) ? "" : args.substring(sp + 1);
    }
    delay(50);
    usbKeyboard.releaseAll();
  }

  return true;
}

#endif // ESP32S3
