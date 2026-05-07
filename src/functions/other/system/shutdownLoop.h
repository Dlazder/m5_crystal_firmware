// pid 33

void shutdownLoop() {
	if (isSetup()) {
		const char* farewells[] = {
			"See you!",
			"Goodbye!",
			"Farewell!",
			"Take care!",
			"Bye bye!",
			"Peace!",
			"Adios!"
		};
		int farewellCount = sizeof(farewells) / sizeof(farewells[0]);
		int randomIndex = random(0, farewellCount);
		centeredPrint(farewells[randomIndex], BIG_TEXT);
		updateTimer();
	}

	if (checkTimer(500)) {
		DEVICE.Power.powerOff();
	}
}