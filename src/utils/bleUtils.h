// UI-side hook for the BLE service (src/services/ble/) — same weak/strong
// pattern as Storage::reportMountError in errorUtils.h.
//
// Ble::begin() is UI-free; it calls the weak Ble::reportBeginWaiting() no-op
// in ble.cpp. This STRONG definition overrides it at link time and shows the
// "waiting for connection" screen that every BLE HID loop (bad BLE, keyboard,
// mouse, mouse jiggler, presenter, shutter) expects right after begin().

void Ble::reportBeginWaiting() {
	centeredPrint(L->TXT_WAITING_CONNECTION, MEDIUM_TEXT);
	updateTimer();
}
