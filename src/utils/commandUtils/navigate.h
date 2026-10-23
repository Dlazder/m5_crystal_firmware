// Serial-driven menu navigation. Emulates the physical buttons that
// menuLoop() / checkExit() already read every frame, so it works
// identically for static menus and dynamic lists (Wi-Fi scan results,
// file pickers, etc.) without needing a PID for every list entry.
//
//   /up     - emulate BtnPWR (move cursor up / decrease value)
//   /down   - emulate BtnB   (move cursor down / increase value)
//   /select - emulate BtnA   (confirm / open selected item)
//   /back   - emulate BtnB's "back" role used by checkExit()-style screens
//
// Note: in the main menu list (menuLoop), BtnB means "down", not "back" -
// going back there means moving the cursor to the "Back" item (index 0)
// and pressing /select. /back is for screens that use checkExit() (lists,
// pickers, running tools), where BtnB/ESC means "exit to the previous screen".
void upCommand(Print& out) {
	pressBtnPWR();
	out.println("OK");
}

void downCommand(Print& out) {
	pressBtnB();
	out.println("OK");
}

void selectCommand(Print& out) {
	pressBtnA();
	out.println("OK");
}

void backCommand(Print& out) {
	pressBtnB();
	out.println("OK");
}
