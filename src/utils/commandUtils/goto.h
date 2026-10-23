// Switches the device to another process/screen by name or numeric PID,
// e.g. "/goto WIFI_SCAN" or "/goto 201". Names match the X(NAME, ID, fn)
// entries in PROCESS_LIST (see system/processes.h) and are case-insensitive.
void gotoCommand(String arg, Print& out) {
	arg.trim();
	if (arg == "") {
		out.println("Usage: /goto <name|id>");
		return;
	}

	bool isNumeric = true;
	for (unsigned int i = 0; i < arg.length(); i++) {
		if (!isDigit(arg[i])) {
			isNumeric = false;
			break;
		}
	}

	if (isNumeric) {
		int pid = arg.toInt();
		for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
			if (PROCESS_TABLE[i].id == pid) {
				out.printf("Switching to %s (%d)\n", PROCESS_TABLE[i].name, pid);
				changeProcess(pid);
				return;
			}
		}
		out.printf("Unknown process id: %d\n", pid);
		return;
	}

	String upper = arg;
	upper.toUpperCase();
	for (int i = 0; i < PROCESS_TABLE_SIZE; i++) {
		if (upper == PROCESS_TABLE[i].name) {
			out.printf("Switching to %s (%d)\n", PROCESS_TABLE[i].name, PROCESS_TABLE[i].id);
			changeProcess(PROCESS_TABLE[i].id);
			return;
		}
	}
	out.printf("Unknown process name: %s\n", arg.c_str());
}
