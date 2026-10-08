// ============================================================================
// Universal error "throw" — a modal error screen any code can route to.
// ============================================================================
//
// GOAL
//   Let mount/IO failures be reported from ONE place (the Storage service)
//   without writing `centeredPrint("LittleFS error"); return;` at every call
//   site, and without making the Storage service depend on the UI.
//
// THE PROBLEM
//   * C++ exceptions are disabled (`-fno-exceptions`), and `setjmp`/`longjmp`
//     across C++ stack frames is UB — so a literal throw/catch is impossible.
//   * `changeProcess(pid)` only sets a global; it does NOT unwind the stack,
//     so the caller must still `return` after "throwing".
//   * storage.cpp is a SEPARATE translation unit that deliberately does not
//     `#include "globals.h"` (doing so would duplicate the globals and break
//     the native/emulation build). It therefore cannot reach `L`, `canvas`,
//     `centeredPrint`, or the process globals that showing an error needs.
//
// HOW IT WORKS — two halves joined by a weak-symbol hook:
//
//   (A) STORAGE SIDE (no UI dependency) — src/services/storage/
//       * mountLittleFS() / mountSD() stay SILENT probes: they answer "did it
//         mount?" and are called from non-UI contexts (autoDetect, fsFor
//         before every open/remove/..., menu isSD()/isLittleFS() checks).
//         They MUST NOT report errors — that would pop an error screen during
//         ordinary probing.
//       * requireLittleFS(returnPid) / requireSD(returnPid) are the public
//         "mount or report" wrappers: mount, and on failure call
//         reportMountError(...) and return false (the caller must `return`).
//       * reportMountError(...) is declared `__attribute__((weak))` with a
//         no-op default in storage.cpp. At link time the strong UI definition
//         below OVERRIDES it (same trick as Arduino's weak setup()/loop()).
//         If the UI layer is absent (native build), the weak no-op keeps
//         silent probes silent.
//
//   (B) UI SIDE (this file) — pulled into the single firmware translation unit
//       via system/utils.h.
//       * throwError(msg, returnPid) sets the globals that the switcher picks
//         up: errorMessage, errorReturnPid (= previousProcess when negative),
//         process = PID::ERROR_SCREEN, isSwitching = true.
//       * Storage::reportMountError(...) is the STRONG definition: it routes
//         a mount failure through throwError() with a localized message.
//
//   The switcher sees `process == PID::ERROR_SCREEN` and runs errorLoop()
//   (src/functions/errorLoop.h), which prints errorMessage and, on exit,
//   `checkExit(errorReturnPid)` returns to the recorded target process.
//
// PIECES (grep these to follow the flow):
//   throwError / reportMountError(strong)   src/utils/errorUtils.h
//   requireLittleFS / requireSD / weak hook  src/services/storage/storage.{h,cpp}
//   errorMessage / errorReturnPid           src/system/globals.h
//   PID::ERROR_SCREEN                       src/system/processes.h
//   errorLoop()                             src/functions/errorLoop.h
//   TXT_STORAGE_LITTLEFS_ERROR/_SD_ERROR    src/system/locale/*.cpp
//   Callers of require*: filePickerLFSLoop.h, filePickerSDLoop.h,
//                        filePickerUtils.h, irReadLoop.h
//
// HOW TO REVERT / CHANGE
//   * Remove a call site: replace `if (!Storage::requireX(p)) return;` with
//     the old `if (!Storage::mountX()) { centeredPrint(...); return; }`.
//   * Remove the whole mechanism: delete errorUtils.h + errorLoop.h, drop the
//     `#include` lines from system/utils.h and system/functions.h, remove
//     PID::ERROR_SCREEN from processes.h, the errorMessage/errorReturnPid
//     globals from globals.h, and the require*/reportMountError declarations
//     from storage.{h,cpp}, then restore each call site.
//   * Change the message text: edit TXT_STORAGE_LITTLEFS_ERROR / _SD_ERROR in
//     the 12 locale files (and locale.h's struct fields).
//   * Change where errors return to: pass a different `returnPid` to require*,
//     or a negative value to mean previousProcess.
// ============================================================================

void throwError(const String& msg, int returnPid = -1) {
	errorMessage = msg;
	errorReturnPid = (returnPid < 0) ? previousProcess : returnPid;
	process = PID::ERROR_SCREEN;
	isSwitching = true;
	clearKbFlags();
	btnAWasPressed = false;
	btnBWasPressed = false;
}

void Storage::reportMountError(bool useLittleFS, int returnPid) {
	throwError(useLittleFS ? L->TXT_STORAGE_LITTLEFS_ERROR : L->TXT_STORAGE_SD_ERROR, returnPid);
}
