// PID::USB_STORAGE

// Exposes the SD card to the host PC as a USB Mass Storage device (removable
// drive). The host gets raw block-level access, so while MSC is active the
// firmware must NOT touch the SD card itself or the filesystem will corrupt:
// we end our own SD mount, hand the card to USBMSC, and only the PC reads/writes.

#if defined(ESP32S3) && HAS_SD

#include "USB.h"
#include "USBMSC.h"

static USBMSC usbMsc;
static bool usbMscStarted = false;

// USBMSC read/write callbacks — backed by raw SD sector access (SD.readRAW /
// SD.writeRAW). bufsize is always a whole number of 512-byte sectors.
static int32_t usbMscOnWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
	uint32_t sectors = bufsize / 512;
	for (uint32_t i = 0; i < sectors; i++) {
		if (!SD.writeRAW(buffer + i * 512, lba + i)) return -1;
	}
	return bufsize;
}

static int32_t usbMscOnRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
	uint32_t sectors = bufsize / 512;
	uint8_t* buf = (uint8_t*)buffer;
	for (uint32_t i = 0; i < sectors; i++) {
		if (!SD.readRAW(buf + i * 512, lba + i)) return -1;
	}
	return bufsize;
}

// start/stop (eject) request from the host. Allow it.
static bool usbMscOnStartStop(uint8_t power_condition, bool start, bool load_eject) {
	return true;
}

void usbStorageLoop() {
	if (isSetup()) {
		// (Re)mount the SD just to read its geometry, then release it so the
		// host owns the card while MSC is active.
		if (!sdBegin()) {
			centeredPrint(L->TXT_USB_STORAGE_NO_SD, MEDIUM_TEXT);
			usbMscStarted = false;
			return;
		}

		uint32_t sectorCount = SD.numSectors();
		uint16_t sectorSize  = SD.sectorSize();
		if (sectorCount == 0 || sectorSize == 0) {
			sdEnd();
			centeredPrint(L->TXT_USB_STORAGE_NO_SD, MEDIUM_TEXT);
			usbMscStarted = false;
			return;
		}

		usbMsc.vendorID("M5");
		usbMsc.productID("Crystal SD");
		usbMsc.productRevision("1.0");
		usbMsc.onRead(usbMscOnRead);
		usbMsc.onWrite(usbMscOnWrite);
		usbMsc.onStartStop(usbMscOnStartStop);
		usbMsc.mediaPresent(true);
		usbMsc.begin(sectorCount, sectorSize);

		if (!usbHidBegan) {
			USB.begin();
			usbHidBegan = true;
		}
		usbMscStarted = true;

		String rows[] = { L->TXT_USB_STORAGE_ACTIVE, L->TXT_USB_STORAGE_HINT };
		centeredPrintRows(rows, 2, MEDIUM_TEXT);
		soundSuccess();
	}

	if (checkExit()) {
		if (usbMscStarted) {
			usbMsc.end();
			usbMscStarted = false;
			// Re-mount so the firmware's own file browser works again.
			sdEnd();
			sdBegin();
		}
	}
}

#else // native USB device mode unavailable, or no SD card

void usbStorageLoop() {
	if (isSetup()) {
		centeredPrint(L->TXT_USB_HID_UNSOPPORTED, MEDIUM_TEXT);
	}
	checkExit();
}

#endif
