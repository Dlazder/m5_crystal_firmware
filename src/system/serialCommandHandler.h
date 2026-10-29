// Executes a single command line, writing any response to `out`.
// Lines that don't start with '/' are ignored, so raw UART traffic passes through untouched.
void handleSerialCommandLine(String cmd, Print& out) {
  // Check the raw line before trimming: indented output (e.g. the help text)
  // starts with a space, so it is never re-read as a command when UART RX is
  // looped back to TX.
  if (!cmd.startsWith("/")) return;

  cmd.trim();

  // Split off the command word from its (optional) argument, e.g.
  // "/goto WIFI_SCAN" -> command="/goto", arg="WIFI_SCAN".
  int spaceIdx = cmd.indexOf(' ');
  String command = (spaceIdx == -1) ? cmd : cmd.substring(0, spaceIdx);
  String arg = (spaceIdx == -1) ? "" : cmd.substring(spaceIdx + 1);

  if (command == "/screenshot") {
    screenshotCommand(out);
  } else if (command == "/info") {
    infoCommand(out);
  } else if (command == "/help") {
    helpCommand(out);
  } else if (command == "/shutdown") {
    shutdownCommand(out);
  } else if (command == "/goto") {
    gotoCommand(arg, out);
  } else if (command == "/up") {
    upCommand(out);
  } else if (command == "/down") {
    downCommand(out);
  } else if (command == "/select") {
    selectCommand(out);
  } else if (command == "/back") {
    backCommand(out);
  } else {
    out.println("Command not found!");
  }
}

// Reads one line from `port` and hands it to handleSerialCommandLine.
void handleSerialCommands(Stream& port) {
  if (!port.available()) return;

  String cmd = port.readStringUntil('\n');
  handleSerialCommandLine(cmd, port);
}
