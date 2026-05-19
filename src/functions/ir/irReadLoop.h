// PID::IR_READ

#include <IRremote.hpp>

static bool irReceiverStarted = false;

void irReadLoop() {
    if (isSetup()) {
        irReceiverStarted = false;

        IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
        irReceiverStarted = true;

        String lines[] = {
            "IR Receiver",
            "Waiting...",
        };
        centeredPrintRows(lines, 2, MEDIUM_TEXT);
        Serial.println("IR: receiver started on pin " + String(IR_RECEIVE_PIN));
    }

    if (IrReceiver.decode()) {
        bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;

        uint32_t rawCode  = IrReceiver.decodedIRData.decodedRawData;
        uint16_t address  = IrReceiver.decodedIRData.address;
        uint16_t command  = IrReceiver.decodedIRData.command;
        String   protocol = getProtocolString(IrReceiver.decodedIRData.protocol);

        IrReceiver.resume();

        if (isRepeat) return;

        Serial.printf("IR: proto=%s addr=0x%04X cmd=0x%04X raw=0x%08X\n", protocol.c_str(), address, command, rawCode);

        DEVICE.Speaker.tone(2000, 80);

        char rawHex[11];
        snprintf(rawHex, sizeof(rawHex), "0x%08lX", (unsigned long)rawCode);

        String lines[] = {
            protocol,
            "Addr: 0x" + String(address, HEX),
            "Cmd:  0x" + String(command, HEX),
            "Raw:  " + String(rawHex),
        };
        centeredPrintRows(lines, 4, MEDIUM_TEXT);
    }

    if (checkExit()) {
        if (irReceiverStarted) {
            IrReceiver.end();
            irReceiverStarted = false;
        }
    }
}
