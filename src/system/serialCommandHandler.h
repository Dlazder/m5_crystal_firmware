void handleSerialCommands() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();

  if (cmd == "/screenshot") {
    screenshotCommand();
  } else {
    Serial.println("Command not found!");
  }
}
