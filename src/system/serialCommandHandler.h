void handleSerialCommands() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();

  if (cmd == "/screenshot") {
    screenshotCommand();
  } else if (cmd == "/info") {
    infoCommand();
  } else {
    Serial.println("Command not found!");
  }
}
