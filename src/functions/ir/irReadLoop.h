// PID::IR_READ

#include "../../hal/irGpioRx.h" // GPIO IRremote backend (isolated — see irGpioRx.h)

#ifdef IR_USE_RMT
#include "../../hal/irRmtHal.h"
#endif

void _irDrawWaiting() {
	centeredPrint(L->TXT_IR_WAITING, MEDIUM_TEXT, true);
	drawSpinner();
	canvas.pushSprite(0, getStatusBarHeight());
}

// ── Shared state (both backends) ──

static bool irReceiverStarted = false;
static bool irKbActive = false;

static String irLastProtocol = "";
static uint16_t irLastAddress = 0;
static uint16_t irLastCommand = 0;
static uint32_t irLastRaw = 0;
static bool irHasSignal = false;

#ifdef IR_USE_RMT
// ── RMT backend (internal HAL) for M5StickS3 ──

static volatile bool irNewData = false;
static volatile irproto irBrand = NEC;
static volatile uint32_t irCode = 0;
static volatile size_t irBits = 0;

void irReceived(irproto brand, uint32_t code, size_t len, rmt_symbol_word_t *item) {
	if (code) {
		irBrand = brand;
		irCode = code;
		irBits = len;
		irNewData = true;
	}
}

static void _irStartRmt() {
	irStopReceiver(); // kill any previous RX task before re-creating
	DEVICE.Speaker.end();
	M5.Power.setExtOutput(true, m5::ext_none);
	delay(50); // let external power stabilize before RMT init
	pinMode(irRmtRxPin, INPUT_PULLUP); // built-in receiver output idles high via pull-up
	delay(10);
	xTaskCreatePinnedToCore(recvIR, "recvIR", 4096, NULL, 10, NULL, 1);
	irReceiverStarted = true;
}

static void _irStopRmt() {
	irStopReceiver(); // kill the RX task and free RMT resources
	DEVICE.Speaker.begin();
	M5.Power.setExtOutput(false, m5::ext_none);
	irReceiverStarted = false;
}

// Poll the RMT task for a new signal and decode it into the shared state.
// Returns true when a new valid signal was decoded.
static bool _irPollRmt() {
	if (!irNewData) return false;
	irNewData = false;
	irLastProtocol = proto[irBrand].name;
	irLastRaw = (irBrand == NEC) ? irNecForDisplay(irCode) : irCode;

	if (irBrand == NEC) {
		// Transmission order: [addr, ~addr, cmd, ~cmd], each byte LSB-first
		irLastAddress = ir_rev8((irCode >> 24) & 0xFF);
		irLastCommand = ir_rev8((irCode >> 8) & 0xFF);
		uint8_t na = ir_rev8((irCode >> 16) & 0xFF);
		uint8_t nc = ir_rev8(irCode & 0xFF);
		if ((uint8_t)~irLastAddress != na || (uint8_t)~irLastCommand != nc) {
			// Invalid NEC frame — complement check failed (likely noise)
			Serial.printf("IR: NEC complement fail: addr=0x%02X na=0x%02X cmd=0x%02X nc=0x%02X\n",
				irLastAddress, na, irLastCommand, nc);
			return false;
		}
	} else if (irBrand == SAM) {
		// Samsung: 16-bit address + 16-bit command
		irLastAddress = ((uint16_t)ir_rev8((irCode >> 24) & 0xFF) << 8)
					| (uint16_t)ir_rev8((irCode >> 16) & 0xFF);
		irLastCommand = ((uint16_t)ir_rev8((irCode >> 8) & 0xFF) << 8)
					| (uint16_t)ir_rev8(irCode & 0xFF);
	} else {
		irLastAddress = irCode & 0xFFFF;
		irLastCommand = (irCode >> 16) & 0xFF;
	}

	irHasSignal = true;
	Serial.printf("IR: %s code=0x%08lX\n", irLastProtocol.c_str(), (unsigned long)irCode);
	return true;
}
#endif // IR_USE_RMT

// ── GPIO backend (IRremote) ──

static void _irStartGpio() {
	irGpioRxBegin(irRxPin);
	irReceiverStarted = true;
}

static void _irStopGpio() {
	irGpioRxEnd();
	irReceiverStarted = false;
}

static bool _irPollGpio() {
	uint16_t addr = 0, cmd = 0;
	uint32_t raw = 0;
	String proto = "";
	if (!irGpioRxPoll(addr, cmd, raw, proto)) return false;
	irLastAddress = addr;
	irLastCommand = cmd;
	irLastRaw = raw;
	irLastProtocol = proto;
	irHasSignal = true;
	Serial.printf("IR: %s addr=0x%04X cmd=0x%04X\n",
		irLastProtocol.c_str(), irLastAddress, irLastCommand);
	soundBeep();
	return true;
}

// ── Shared UI / save ──

void _irDrawUi() {
	char rawHex[11];
	snprintf(rawHex, sizeof(rawHex), "0x%08lX", (unsigned long)irLastRaw);
	String lines[] = {
		irLastProtocol,
		"Addr: 0x" + String(irLastAddress, HEX),
		"Cmd: 0x" + String(irLastCommand, HEX),
		String("Raw: ") + rawHex,
	};
	centeredPrintRows(lines, 4, SMALL_TEXT);
	drawHintCustom("enter: save", "A: save");
}

void _irSaveToLFS(const char* filename) {
	if (!Storage::requireLittleFS(process)) return;
	char rawHex[11];
	snprintf(rawHex, sizeof(rawHex), "0x%08lX", (unsigned long)irLastRaw);
	String path = "/" + String(filename) + ".ir";
	File f = Storage::open(path.c_str(), "w", true);
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
	delay(1000);
}

static void _irRestoreReceiver() {
#ifdef IR_USE_RMT
	if (irRxUseGpio) _irStartGpio(); else _irStartRmt();
#else
	_irStartGpio();
#endif
	if (irHasSignal) { _irDrawUi(); } else { _irDrawWaiting(); }
}

void irReadLoop() {
	if (isSetup()) {
		irReceiverStarted = false;
		irKbActive = false;
		irHasSignal = false;

#ifdef IR_USE_RMT
		irNewData = false;
		if (irRxUseGpio) {
			_irStartGpio();
			Serial.println("IR: GPIO receiver ready on pin " + String(irRxPin));
		} else {
			_irStartRmt();
			Serial.println("IR: RMT ready (NEC/Sony/Samsung/RC5)");
		}
#else
		_irStartGpio();
		Serial.println("IR: GPIO receiver ready on pin " + String(irRxPin));
#endif

		_irDrawWaiting();
	}

	if (irKbActive) {
		keyboardLoop(
			[]() { irKbActive = false; kbEnd(); _irRestoreReceiver(); },
			[](const char* buf) {
				if (kbLen > 0) _irSaveToLFS(buf);
				irKbActive = false; kbEnd(); _irRestoreReceiver();
			},
			nullptr
		);
		return;
	}

	// Poll the active backend
	bool got = false;
#ifdef IR_USE_RMT
	if (irRxUseGpio) got = _irPollGpio(); else got = _irPollRmt();
#else
	got = _irPollGpio();
#endif
	if (got) _irDrawUi();

	if ((isBtnAWasPressed() || isKbEnterPressed()) && irHasSignal) {
		// Stop the active receiver while the keyboard is up
#ifdef IR_USE_RMT
		if (irRxUseGpio) _irStopGpio(); else _irStopRmt();
#else
		_irStopGpio();
#endif
		irKbActive = true;
		kbReset();
		drawKeyboardUi();
		return;
	}

	if (!irHasSignal) _irDrawWaiting();

	if (checkExit()) {
		if (irReceiverStarted) {
#ifdef IR_USE_RMT
			if (irRxUseGpio) _irStopGpio(); else _irStopRmt();
#else
			_irStopGpio();
#endif
		}
	}
}
