// Storage service — unified filesystem access over LittleFS and SD.
//
// Mirrors Flipper Zero's storage service: callers talk to one API and pass a
// backend flag instead of branching on LittleFS/SD themselves.
//
// Backend resolution (the `useLittleFS` flag):
// open(path, mode)				> auto: SD if a card is present, else LittleFS
// open(path, mode, true) > force LittleFS
// open(path, mode, false) > force SD (invalid File if no card)
//
// Read paths that act on a user-picked file (file picker) must pass the
// backend explicitly; only new captures/saves should rely on the auto default.

#pragma once
#include <Arduino.h>
#include <FS.h>

namespace Storage {

// Mount / unmount 

/// Mounts LittleFS (idempotent). Returns true when available.
bool mountLittleFS();

/// Unmounts LittleFS, releasing its cache. Idempotent (no-op if not mounted).
void unmountLittleFS();

/// Mounts SD over SPI (idempotent). Returns true when a card is present.
bool mountSD();

/// Unmounts SD, releasing the SPI bus and its pins.
void unmountSD();

bool isLittleFS();
bool isSD();

/// Mount-or-report wrappers for the universal error screen (see errorUtils.h
/// for the full mechanism and how to revert it). Unlike mountLittleFS/mountSD,
/// which are SILENT probes, these report a mount failure through the weak
/// `reportMountError` hook and return false so the caller can `return`.
/// `returnPid` is where to go when the user dismisses the error; a negative
/// value means `previousProcess` (pass the current `process` to stay in the
/// current screen, e.g. IR save so its captured data isn't lost).
bool requireLittleFS(int returnPid = -1);
bool requireSD(int returnPid = -1);

/// Weak mount-error hook. The no-op default lives in storage.cpp so this
/// translation unit never depends on the UI; the firmware's UI layer provides
/// a STRONG override (errorUtils.h) that shows the error screen. Do not call
/// this directly — use requireLittleFS/requireSD.
void reportMountError(bool useLittleFS, int returnPid);

/// Default backend for new captures: SD if a card is present, else LittleFS.
/// May mount on first call — this is the lazy-mount entry point.
bool autoDetect();

/// Returns the resolved `fs::FS` reference for callers that need the raw
/// backend object (e.g. M5GFX image decoders). Mounts if necessary.
fs::FS& getFS(bool useLittleFS = autoDetect());

// File operations 

/// Opens a file on the resolved backend. See header note for flag semantics.
File open(const char* path, const char* mode, bool useLittleFS = autoDetect());

bool exists(const char* path, bool useLittleFS = autoDetect());
bool remove(const char* path, bool useLittleFS = autoDetect());
bool rename(const char* from, const char* to, bool useLittleFS = autoDetect());
bool mkdir(const char* path, bool useLittleFS = autoDetect());

/// Recursively deletes a directory and everything inside it.
bool removeRecursive(const char* path, bool useLittleFS = autoDetect());

/// Scans a directory, returning sorted entry names + directory flags.
/// Directories come first, then files; each group sorted alphabetically.
/// @return number of entries, or -1 on error (caller frees both arrays).
int list(const String& dirPath, bool useLittleFS, String*& outNames, bool*& outIsDir);

/// Reads a whole file into `out`. Backend is explicit (must not auto-detect).
bool readString(const String& path, String& out, bool useLittleFS);

/// Reads a text file line-by-line into a caller-freed String[].
/// Blank lines are skipped. Backend is explicit (see readString).
bool readLines(const String& path, String*& outLines, int& outCount, bool useLittleFS);

/// Streams a file from one backend to the other (or within the same backend),
/// chunk by chunk, so RAM use is independent of file size. Both backends are
/// mounted lazily on demand and may be live simultaneously for the duration
/// of the copy.
bool copy(const char* srcPath, const char* dstPath, bool srcLittleFS, bool dstLittleFS);

/// Generates a unique path by appending "_N" before the extension.
String uniquePath(const String& basePath, const String& ext, bool useLittleFS = autoDetect());

/// Opens a new uniquely-named file for writing on the resolved backend.
File openUnique(const String& basePath, const String& ext, bool useLittleFS = autoDetect());

} // namespace Storage
