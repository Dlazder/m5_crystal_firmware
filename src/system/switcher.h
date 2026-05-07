struct ProcessEntry {
	int processId;
	void (*loopFunction)();
};

const ProcessEntry processEntries[] = {
	#define X(name, id, fn) {PID::name, fn},
	PROCESS_LIST
	#undef X
};

void switcher() {
	for (const ProcessEntry& entry : processEntries) {
		if (entry.processId == process) {
			entry.loopFunction();
			return;
		}
	}
	defaultLoop();
}
