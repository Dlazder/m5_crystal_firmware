// PID::IR_READ

void _irDrawWaiting() {
	centeredPrint(L->TXT_IR_WAITING, MEDIUM_TEXT, true);
	drawSpinner();
	canvas.pushSprite(0, getStatusBarHeight());
}

#ifdef IR_USE_RMT
#include "../../hal/irRmtHal.h"

// ── RMT-based (internal HAL) for M5StickS3 ──

static bool irReceiverStarted = false;
static bool irKbActive = false;

static volatile bool irNewData = false;
static volatile irproto irBrand = NEC;
static volatile uint32_t irCode = 0;
static volatile size_t irBits = 0;

static uint16_t irLastAddress = 0;
static uint8_t irLastCommand = 0;
static uint32_t irLastRaw = 0;
static String irLastProtocol = "";
static bool irHasSignal = false;

void irReceived(irproto brand, uint32_t code, size_t len, rmt_symbol_word_t *item) {
	if (code) {
		irBrand = brand;
		irCode = code;
		irBits = len;
		irNewData = true;
	}
}

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
	delay(1000);
}

void _irRestoreReceiver() {
	DEVICE.Speaker.end();
	irStopReceiver(); // kill any previous RX task before re-creating
	M5.Power.setExtOutput(true, m5::ext_none);
	delay(50); // let external power stabilize before RMT init
	pinMode(irRxPin, INPUT_PULLUP); // receiver output idles high via pull-up
	delay(10);
	xTaskCreatePinnedToCore(recvIR, "recvIR", 4096, NULL, 10, NULL, 1);
	irReceiverStarted = true;
	if (irHasSignal) { _irDrawUi(); } else { _irDrawWaiting(); }
}

void irReadLoop() {
	if (isSetup()) {
		irReceiverStarted = false;
		irKbActive = false;
		irHasSignal = false;
		irNewData = false;

		irStopReceiver(); // kill any previous RX task and free RMT

		_irDrawWaiting();

		DEVICE.Speaker.end();
		M5.Power.setExtOutput(true, m5::ext_none);
		delay(50); // let external power stabilize before RMT init

		pinMode(irRxPin, INPUT_PULLUP); // receiver output idles high via pull-up
		delay(10);

		xTaskCreatePinnedToCore(recvIR, "recvIR", 4096, NULL, 10, NULL, 1);
		irReceiverStarted = true;

		Serial.println("IR: RMT ready (NEC/Sony/Samsung/RC5)");
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

	if (irNewData) {
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
				return;
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
		_irDrawUi();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && irHasSignal) {
		irStopReceiver(); // stop RX task while keyboard is active
		DEVICE.Speaker.begin();
		M5.Power.setExtOutput(false, m5::ext_none);
		irReceiverStarted = false;
		irKbActive = true;
		kbReset();
		drawKeyboardUi();
		return;
	}

	if (!irHasSignal) _irDrawWaiting();

	if (checkExit()) {
		if (irReceiverStarted) {
			irStopReceiver();
			DEVICE.Speaker.begin();
			M5.Power.setExtOutput(false, m5::ext_none);
			irReceiverStarted = false;
		}
	}
}

#else // !IR_USE_RMT — standard GPIO-based IRremote
#include <IRremote.hpp>

static bool irReceiverStarted = false;
static bool irKbActive = false;

static String irLastProtocol = "";
static uint16_t irLastAddress = 0;
static uint16_t irLastCommand = 0;
static uint32_t irLastRaw = 0;
static bool irHasSignal = false;

void _irDrawUi() {
	char rawHex[11];
	snprintf(rawHex, sizeof(rawHex), "0x%08lX", (unsigned long)irLastRaw);
	String lines[] = {
		irLastProtocol,
		"Addr: 0x" + String(irLastAddress, HEX),
		"Cmd: 0x" + String(irLastCommand, HEX),
		String("Raw: ") + rawHex,
	};
	centeredPrintRows(lines, 4, MEDIUM_TEXT);
	drawHintCustom("enter: save", "A: save");
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
	delay(1000);
}

void _irRestoreReceiver() {
	IrReceiver.begin(irRxPin, DISABLE_LED_FEEDBACK);
	irReceiverStarted = true;
	if (irHasSignal) { _irDrawUi(); } else { _irDrawWaiting(); }
}

void irReadLoop() {
	if (isSetup()) {
		irReceiverStarted = false;
		irKbActive = false;
		irHasSignal = false;
		_irDrawWaiting();
		IrReceiver.begin(irRxPin, DISABLE_LED_FEEDBACK);
		irReceiverStarted = true;
		Serial.println("IR: GPIO receiver ready on pin " + String(irRxPin));
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

	if (IrReceiver.decode()) {
		bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;
		irLastRaw = IrReceiver.decodedIRData.decodedRawData;
		irLastAddress = IrReceiver.decodedIRData.address;
		irLastCommand = IrReceiver.decodedIRData.command;
		irLastProtocol = getProtocolString(IrReceiver.decodedIRData.protocol);
		IrReceiver.resume();
		if (isRepeat) return;

		irHasSignal = true;
		Serial.printf("IR: %s addr=0x%04X cmd=0x%04X\n",
			irLastProtocol.c_str(), irLastAddress, irLastCommand);
		soundBeep();
		_irDrawUi();
	}

	if ((isBtnAWasPressed() || isKbEnterPressed()) && irHasSignal) {
		IrReceiver.end();
		irReceiverStarted = false;
		irKbActive = true;
		kbReset();
		drawKeyboardUi();
		return;
	}

	if (!irHasSignal) _irDrawWaiting();

	if (checkExit()) {
		if (irReceiverStarted) {
			IrReceiver.end();
			irReceiverStarted = false;
		}
	}
}

#endif // IR_USE_RMT
