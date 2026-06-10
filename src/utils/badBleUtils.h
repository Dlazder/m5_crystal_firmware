const char* badBleScriptPtr = nullptr;
int badBleScriptPos = 0;
unsigned long badBleDelayUntil = 0;


String badBleScriptBuffer = "";

/**
 * Sets the script to execute and resets the parser state.
 * @param script null-terminated DuckyScript string
 */
void badBleSetScript(const char* script) {
  badBleScriptPtr  = script;
  badBleScriptPos  = 0;
  badBleDelayUntil = 0;
}

/**
 * Loads a DuckyScript file and sets it as the active script.
 * Uses fpSelectedSd to decide between SD and LittleFS.
 * @param path  full path to the file (e.g. "/notepad.txt")
 * @return true if loaded successfully, false if file could not be opened
 */

/**
 * Returns true if the parser is currently waiting for a DELAY command to expire.
 * Call this before badBleNextLine() to avoid executing lines during a delay.
 * @return true if delay is still active, false if ready to proceed
 */
bool badBleIsDelaying() {
  return millis() < badBleDelayUntil;
}

/**
 * Resolves a DuckyScript key name to its BLE HID keycode.
 * @param name  key name string (e.g. "ENTER", "GUI", "F1")
 * @return BLE HID keycode, or 0 if not recognized
 */
uint8_t badBleResolveKey(String name) {
  if (name == "ENTER")     return KEY_RETURN;
  if (name == "BACKSPACE") return KEY_BACKSPACE;
  if (name == "TAB")       return KEY_TAB;
  if (name == "ESC")       return KEY_ESC;
  if (name == "DELETE")    return KEY_DELETE;
  if (name == "INSERT")    return KEY_INSERT;
  if (name == "HOME")      return KEY_HOME;
  if (name == "END")       return KEY_END;
  if (name == "PAGEUP")    return KEY_PAGE_UP;
  if (name == "PAGEDOWN")  return KEY_PAGE_DOWN;
  if (name == "UP")        return KEY_UP_ARROW;
  if (name == "DOWN")      return KEY_DOWN_ARROW;
  if (name == "LEFT")      return KEY_LEFT_ARROW;
  if (name == "RIGHT")     return KEY_RIGHT_ARROW;
  if (name == "SPACE")     return ' ';
  if (name == "GUI")       return KEY_LEFT_GUI;
  if (name == "CTRL")      return KEY_LEFT_CTRL;
  if (name == "SHIFT")     return KEY_LEFT_SHIFT;
  if (name == "ALT")       return KEY_LEFT_ALT;
  if (name == "F1")        return KEY_F1;
  if (name == "F2")        return KEY_F2;
  if (name == "F3")        return KEY_F3;
  if (name == "F4")        return KEY_F4;
  if (name == "F5")        return KEY_F5;
  if (name == "F6")        return KEY_F6;
  if (name == "F7")        return KEY_F7;
  if (name == "F8")        return KEY_F8;
  if (name == "F9")        return KEY_F9;
  if (name == "F10")       return KEY_F10;
  if (name == "F11")       return KEY_F11;
  if (name == "F12")       return KEY_F12;
  return 0;
}

/**
 * Parses and executes the next line of the active script.
 * Supports: DELAY, STRING, KEY, COMBO, REM, //.
 * @return true if there are more lines to execute, false when script is finished
 */
bool badBleNextLine() {
  if (!badBleScriptPtr) return false;
  int len = strlen(badBleScriptPtr);

  while (badBleScriptPos < len && (badBleScriptPtr[badBleScriptPos] == '\n' || badBleScriptPtr[badBleScriptPos] == '\r'))
    badBleScriptPos++;

  if (badBleScriptPos >= len) return false;

  int lineStart = badBleScriptPos;
  while (badBleScriptPos < len && badBleScriptPtr[badBleScriptPos] != '\n' && badBleScriptPtr[badBleScriptPos] != '\r')
    badBleScriptPos++;

  String line = String(badBleScriptPtr).substring(lineStart, badBleScriptPos);
  line.trim();

  if (line.length() == 0 || line.startsWith("REM") || line.startsWith("//"))
    return true;

  if (line.startsWith("DELAY ")) {
    badBleDelayUntil = millis() + (unsigned long)line.substring(6).toInt();
  } else if (line.startsWith("STRING ")) {
    String text = line.substring(7);
    for (int i = 0; i < (int)text.length(); i++) {
      bleKeyboard.write((uint8_t)text[i]);
      delay(10);
    }
  } else if (line.startsWith("KEY ")) {
    String keyName = line.substring(4);
    keyName.trim();
    uint8_t k = badBleResolveKey(keyName);
    if (k) bleKeyboard.write(k);
  } else if (line.startsWith("COMBO ")) {
    String args = line.substring(6);
    args.trim();
    while (args.length() > 0) {
      int sp = args.indexOf(' ');
      String token = (sp == -1) ? args : args.substring(0, sp);
      token.trim();
      uint8_t k = badBleResolveKey(token);
      if (!k && token.length() == 1) k = (uint8_t)token[0];
      if (k) bleKeyboard.press(k);
      args = (sp == -1) ? "" : args.substring(sp + 1);
    }
    delay(50);
    bleKeyboard.releaseAll();
  }

  return true;
}
