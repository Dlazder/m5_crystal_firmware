void helpCommand(Print& out) {
	out.println("Available commands:");
	out.println("  /help       - Display this help message.");
	out.println("  /screenshot - Take a screenshot and save it to the SD card.");
	out.println("  /info       - Display device information.");
	out.println("  /shutdown   - Shut down the device.");
	out.println("  /goto <name|id> - Switch to another process/screen (e.g. /goto WIFI_SCAN).");
	out.println("  /up & /down - Move the menu cursor.");
	out.println("  /select     - Confirm / open the selected item (BtnA/enter).");
	out.println("  /back       - Exit to the previous screen.");
}