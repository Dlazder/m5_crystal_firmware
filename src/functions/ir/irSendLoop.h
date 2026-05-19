// PID::IR_SEND

static decode_type_t irSendProtocol = UNKNOWN;
static uint16_t irSendAddress = 0;
static uint16_t irSendCommand = 0;
static bool irFileLoaded = false;

// IRremote has no reverse lookup — match string to enum manually
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
	if (!lfsBegin()) return false;

	File f = LittleFS.open(path.c_str(), "r");
	if (!f) return false;

	irSendProtocol = UNKNOWN;
	irSendAddress = 0;
	irSendCommand = 0;

	while (f.available()) {
		String line = f.readStringUntil('\n');
		line.trim();

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

void _irShowLoaded() {
	String lines[] = {
		String(getProtocolString(irSendProtocol)),
		"Addr: 0x" + String(irSendAddress, HEX),
		"Cmd: 0x" + String(irSendCommand, HEX),
		L->TXT_IR_PRESS_A_SEND,
	};
	centeredPrintRows(lines, 4, MEDIUM_TEXT);
}

void irSendLoop() {
	if (isSetup()) {
		irFileLoaded = false;
		selectedFilePath = "";
		filePickerSetup(PID::IR);
		updateTimer();
	}

	// File picker phase
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

	// Send on button A
	if (isBtnAWasPressed() || isKbEnterPressed()) {
		IrSender.begin(IR_SEND_PIN);

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
			Serial.printf("IR: sent proto=%s addr=0x%04X cmd=0x%04X\n",
				getProtocolString(irSendProtocol), irSendAddress, irSendCommand);
			DEVICE.Speaker.tone(2000, 80);
		} else {
			centeredPrint("Unknown proto", MEDIUM_TEXT);
			delay(800);
		}

		_irShowLoaded();
	}

	if (checkExit()) {
		irFileLoaded = false;
		selectedFilePath = "";
		fpActive = false;
	}
}
