// PID::IR_SEND

#ifdef IR_USE_RMT
#include "../../hal/irRmtHal.h"
#else
#include <IRremote.hpp>
#endif

#ifdef IR_USE_RMT
// ── RMT-based (internal HAL) for M5StickS3 ──
// _rev8() is provided by irReadLoop.h (included earlier)

static irproto irSendProtocol = NEC;
static uint16_t irSendAddress = 0;
static uint16_t irSendCommand = 0;
static uint32_t irSendCode = 0;
static uint8_t irSendBits = 32;
static uint32_t irSendRaw = 0;
static bool irHasRaw = false;

irproto _irProtocolFromString(const String& name) {
	if (name == "NEC") return NEC;
	if (name == "SONY") return SONY;
	if (name == "SAMSUNG") return SAM;
	if (name == "RC5") return RC5;
	return NEC;
}

const char* _irProtocolName(irproto p) {
	return proto[p].name;
}

bool _irParseFile(const String& path) {
	File f = Storage::open(path.c_str(), "r", true);
	if (!f) return false;
	irSendProtocol = NEC; irSendAddress = 0; irSendCommand = 0; irSendBits = 32;
	irHasRaw = false; irSendRaw = 0;
	while (f.available()) {
		String line = f.readStringUntil('\n'); line.trim();
		if (line.startsWith("protocol=")) {
			irSendProtocol = _irProtocolFromString(line.substring(9));
			if (irSendProtocol == SONY) irSendBits = 12;
		} else if (line.startsWith("address=")) {
			irSendAddress = (uint16_t)strtoul(line.substring(8).c_str(), nullptr, 16);
		} else if (line.startsWith("command=")) {
			irSendCommand = (uint16_t)strtoul(line.substring(8).c_str(), nullptr, 16);
		} else if (line.startsWith("raw=")) {
			irSendRaw = (uint32_t)strtoul(line.substring(4).c_str(), nullptr, 16);
			irHasRaw = true;
		}
	}
	f.close();
	// Build transmission-order code.
	// Prefer raw field when available -- preserves exact received signal
	// (e.g. non-standard NEC remotes with unusual complement bytes).
	if (irSendProtocol == NEC) {
		if (irHasRaw) {
			irSendCode = irNecFromDisplay(irSendRaw);
		} else {
			irSendCode = irBuildNEC(irSendAddress & 0xFF, irSendCommand & 0xFF);
		}
		irSendBits = 32;
	} else {
		irSendCode = irSendAddress | ((uint32_t)irSendCommand << 8);
	}
	return true;
}

#else
// ── GPIO-based (IRremote) for other boards ──

static decode_type_t irSendProtocol = UNKNOWN;
static uint16_t irSendAddress = 0;
static uint16_t irSendCommand = 0;

decode_type_t _irProtocolFromString(const String& name) {
	if (name == "NEC")       return NEC;
	if (name == "NEC2")      return NEC2;
	if (name == "SAMSUNG")   return SAMSUNG;
	if (name == "SAMSUNG48") return SAMSUNG48;
	if (name == "SAMSUNGLG") return SAMSUNGLG;
	if (name == "SONY")      return SONY;
	if (name == "RC5")       return RC5;
	if (name == "RC6")       return RC6;
	if (name == "LG")        return LG;
	if (name == "PANASONIC") return PANASONIC;
	if (name == "KASEIKYO")  return KASEIKYO;
	if (name == "DENON")     return DENON;
	if (name == "SHARP")     return SHARP;
	if (name == "JVC")       return JVC;
	if (name == "APPLE")     return APPLE;
	if (name == "ONKYO")     return ONKYO;
	return UNKNOWN;
}

bool _irParseFile(const String& path) {
	File f = Storage::open(path.c_str(), "r", true);
	if (!f) return false;
	irSendProtocol = UNKNOWN; irSendAddress = 0; irSendCommand = 0;
	while (f.available()) {
		String line = f.readStringUntil('\n'); line.trim();
		if (line.startsWith("protocol=")) {
			irSendProtocol = _irProtocolFromString(line.substring(9));
		} else if (line.startsWith("address=")) {
			irSendAddress = (uint16_t)strtoul(line.substring(8).c_str(), nullptr, 16);
		} else if (line.startsWith("command=")) {
			irSendCommand = (uint16_t)strtoul(line.substring(8).c_str(), nullptr, 16);
		}
	}
	f.close();
	return true;
}

#endif // IR_USE_RMT

static bool irFileLoaded = false;

void _irShowLoaded() {
#ifdef IR_USE_RMT
	String lines[] = {
		String(_irProtocolName(irSendProtocol)),
		"Addr: 0x" + String(irSendAddress, HEX),
		"Cmd: 0x" + String(irSendCommand, HEX),
	};
#else
	String lines[] = {
		String(getProtocolString(irSendProtocol)),
		"Addr: 0x" + String(irSendAddress, HEX),
		"Cmd: 0x" + String(irSendCommand, HEX),
	};
#endif
	centeredPrintRows(lines, 3, MEDIUM_TEXT, true);
	drawHintCustom("enter: send", "A: send");
}

void irSendLoop() {
	if (isSetup()) {
		irFileLoaded = false;
		selectedFilePath = "";
		M5.Power.setExtOutput(true, m5::ext_none);
		filePickerSetup(PID::IR);
		updateTimer();
	}

	if (fpActive) {
		if (filePickerLoop()) return;
		if (selectedFilePath == "") return;
		if (!_irParseFile(selectedFilePath)) {
			centeredPrint(L->TXT_IR_PARSE_ERROR, MEDIUM_TEXT);
			delay(800);
			selectedFilePath = "";
			filePickerSetup(PID::IR);
			return;
		}
		irFileLoaded = true;
		_irShowLoaded();
		return;
	}

	if (!irFileLoaded) return;

	if (isBtnAWasPressed() || isKbEnterPressed()) {
#ifdef IR_USE_RMT
		sendIR(irSendProtocol, irSendCode, irSendBits, 1, 1);
		Serial.printf("IR: sent %s code=0x%08lX\n",
			_irProtocolName(irSendProtocol), (unsigned long)irSendCode);
#else
		IrSender.begin(irTxPin);
		bool ok = true;
		switch (irSendProtocol) {
			case NEC:       IrSender.sendNEC(irSendAddress, irSendCommand, 0);        break;
			case NEC2:      IrSender.sendNEC2(irSendAddress, irSendCommand, 0);       break;
			case SAMSUNG:   IrSender.sendSamsung(irSendAddress, irSendCommand, 0);    break;
			case SAMSUNG48: IrSender.sendSamsung48(irSendAddress, irSendCommand, 0);  break;
			case SAMSUNGLG: IrSender.sendSamsungLG(irSendAddress, irSendCommand, 0);  break;
			case SONY:      IrSender.sendSony(irSendAddress, irSendCommand, 0, 12);   break;
			case RC5:       IrSender.sendRC5(irSendAddress, irSendCommand, 0);        break;
			case RC6:       IrSender.sendRC6(irSendAddress, irSendCommand, 0);        break;
			case LG:        IrSender.sendLG(irSendAddress, irSendCommand, 0);         break;
			case PANASONIC: IrSender.sendPanasonic(irSendAddress, irSendCommand, 0);  break;
			case KASEIKYO:  IrSender.sendKaseikyo(irSendAddress, irSendCommand, 0, 0); break;
			case DENON:     IrSender.sendDenon(irSendAddress, irSendCommand, 0);      break;
			case SHARP:     IrSender.sendSharp(irSendAddress, irSendCommand, 0);      break;
			case JVC:       IrSender.sendJVC((uint8_t)irSendAddress, (uint8_t)irSendCommand, 0); break;
			case APPLE:     IrSender.sendApple(irSendAddress, irSendCommand, 0);      break;
			case ONKYO:     IrSender.sendOnkyo(irSendAddress, irSendCommand, 0);      break;
			default:        ok = false; break;
		}
		if (ok) {
			Serial.printf("IR: sent %s addr=0x%04X cmd=0x%04X\n",
				getProtocolString(irSendProtocol), irSendAddress, irSendCommand);
		} else {
			centeredPrint("Unknown proto", MEDIUM_TEXT);
			soundError();
			delay(800);
		}
#endif
		soundBeep();
		_irShowLoaded();
	}

	if (checkExit()) {
		irFileLoaded = false;
		selectedFilePath = "";
		fpActive = false;
	}
}
