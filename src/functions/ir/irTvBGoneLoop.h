// PID::IR_TV_B_GONE

#include "irTvBGoneCodes.h"

static int  tvbIndex = 0;
static bool tvbRunning = false;
static bool tvbDone = false;

void _tvbSendCurrent() {
	const TvbCode& c = TVB_CODES[tvbIndex];

	switch (c.protocol) {
		case NEC:       IrSender.sendNEC(c.address, c.command, 0);                   break;
		case SAMSUNG:   IrSender.sendSamsung(c.address, c.command, 0);               break;
		case SONY:      IrSender.sendSony(c.address, c.command, 0, c.bits);          break;
		case RC5:       IrSender.sendRC5(c.address, c.command, 0);                   break;
		case RC6:       IrSender.sendRC6(c.address, c.command, 0);                   break;
		case LG:        IrSender.sendLG(c.address, c.command, 0);                    break;
		case PANASONIC: IrSender.sendPanasonic(c.address, c.command, 0);             break;
		case KASEIKYO:  IrSender.sendKaseikyo(c.address, c.command, 0, 0);           break;
		case DENON:     IrSender.sendDenon(c.address, c.command, 0);                 break;
		case SHARP:     IrSender.sendSharp(c.address, c.command, 0);                 break;
		case JVC:       IrSender.sendJVC((uint8_t)c.address, (uint8_t)c.command, 0); break;
		case ONKYO:     IrSender.sendOnkyo(c.address, c.command, 0);                 break;
		default: break;
	}

	Serial.printf("TVB [%d/%d] proto=%s addr=0x%04X cmd=0x%04X\n",
		tvbIndex + 1, TVB_COUNT,
		getProtocolString(c.protocol), c.address, c.command);
}

void irTvBGoneLoop() {
	if (isSetup()) {
		tvbIndex = 0;
		tvbRunning = false;
		tvbDone = false;

		IrSender.begin(IR_SEND_PIN);

		String lines[] = { "TV-B-Gone", L->TXT_PRESS_A };
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
	}

	if (tvbDone) {
		checkExit();
		return;
	}

	// Start / stop on button A
	if (isBtnAWasPressed() || isKbEnterPressed()) {
		if (tvbRunning) {
			tvbRunning = false;
			String lines[] = { "TV-B-Gone", L->TXT_PRESS_A };
			centeredPrintRows(lines, 2, MEDIUM_TEXT);
		} else {
			tvbIndex = 0;
			tvbRunning = true;
			updateTimer();
		}
	}

	if (tvbRunning && checkTimer(100)) {
		_tvbSendCurrent();

		String progress = String(tvbIndex + 1) + " / " + String(TVB_COUNT);
		String lines[] = { "TV-B-Gone", L->TXT_IR_TVB_RUNNING, progress };
		centeredPrintRows(lines, 3, MEDIUM_TEXT);

		tvbIndex++;
		if (tvbIndex >= TVB_COUNT) {
			tvbRunning = false;
			tvbDone = true;
			soundSuccess();
			String lines2[] = { "TV-B-Gone", L->TXT_IR_TVB_DONE };
			centeredPrintRows(lines2, 2, MEDIUM_TEXT);
		}
		updateTimer();
	}

	if (checkExit()) {
		tvbRunning = false;
		tvbDone = false;
	}
}
