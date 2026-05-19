// PID::IR_READ

#include <IRremote.hpp>

static bool irReceiverStarted = false;
static bool irKbActive = false;

static String irLastProtocol = "";
static uint16_t irLastAddress = 0;
static uint16_t irLastCommand = 0;
static uint32_t irLastRaw = 0;
static bool irHasSignal = false;

void _irDrawUi() {
	String lines[] = {
		irLastProtocol,
		"Addr: 0x" + String(irLastAddress, HEX),
		"Cmd: 0x" + String(irLastCommand, HEX),
		L->TXT_IR_PRESS_A_SAVE,
	};
	centeredPrintRows(lines, 4, MEDIUM_TEXT);
}

void _irSaveToLFS(const char* filename) {
	if (!lfsBegin()) {
		centeredPrint("LittleFS error", MEDIUM_TEXT);
		delay(800);
		return;
	}

	char rawHex[11];
	snprintf(rawHex, sizeof(rawHex), "0x%08lX", (unsigned long)irLastRaw);

	String path = "/" + String(filename) + ".ir";
	File f = LittleFS.open(path.c_str(), "w");
	if (!f) {
		centeredPrint(L->TXT_IR_SAVE_ERROR, MEDIUM_TEXT);
		delay(800);
		return;
	}

	f.println("protocol=" + irLastProtocol);
	f.println("address=0x" + String(irLastAddress, HEX));
	f.println("command=0x" + String(irLastCommand, HEX));
	f.println("raw=" + String(rawHex));
	f.close();

	centeredPrint("Saved to littleFS", MEDIUM_TEXT);
	Serial.println("IR: saved to " + path);
	delay(1000);
}

void _irRestoreReceiver() {
	IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
	irReceiverStarted = true;
	if (irHasSignal) {
		_irDrawUi();
	} else {
		String lines[] = { "IR Receiver", "Waiting..." };
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
	}
}

void irReadLoop() {
	if (isSetup()) {
		irReceiverStarted = false;
		irKbActive = false;
		irHasSignal = false;

		IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
		irReceiverStarted = true;

		String lines[] = { "IR Receiver", "Waiting..." };
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		Serial.println("IR: receiver started on pin " + String(IR_RECEIVE_PIN));
	}

	// Keyboard phase
	if (irKbActive) {
		keyboardLoop(
			[]() {
				irKbActive = false;
				kbEnd();
				_irRestoreReceiver();
			},
			[](const char* buf) {
				if (kbLen > 0) _irSaveToLFS(buf);
				irKbActive = false;
				kbEnd();
				_irRestoreReceiver();
			},
			nullptr
		);
		return;
	}

	// Receive phase
	if (IrReceiver.decode()) {
		bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;

		irLastRaw = IrReceiver.decodedIRData.decodedRawData;
		irLastAddress = IrReceiver.decodedIRData.address;
		irLastCommand = IrReceiver.decodedIRData.command;
		irLastProtocol = getProtocolString(IrReceiver.decodedIRData.protocol);

		IrReceiver.resume();

		if (isRepeat) return;

		irHasSignal = true;
		Serial.printf("IR: proto=%s addr=0x%04X cmd=0x%04X raw=0x%08lX\n",
			irLastProtocol.c_str(), irLastAddress, irLastCommand, (unsigned long)irLastRaw);

		DEVICE.Speaker.tone(2000, 80);
		_irDrawUi();
	}

	if (isBtnAWasPressed() && irHasSignal) {
		IrReceiver.end();
		irReceiverStarted = false;
		irKbActive = true;
		kbReset();
		drawKeyboardUi();
		return;
	}

	if (checkExit()) {
		if (irReceiverStarted) {
			IrReceiver.end();
			irReceiverStarted = false;
		}
	}
}
