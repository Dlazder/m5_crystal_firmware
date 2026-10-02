// PID::FILE_VIEW

#include <AnimatedGIF.h>

static int _rfScrollOffset = 0;
static int _rfTotalLines = 0;
static String* _rfLines = nullptr;
static bool _rfIsImage = false;
static float _rfZoom = 0.0f; // 0 = auto-fit; >=1.0 = zoom multiplier
static int32_t _rfPanX = 0; // pan offset in rendered pixels
static int32_t _rfPanY = 0;

// Animated GIF state. Allocated on the heap only while a GIF is open so its
// ~40 KB decoder workspace doesn't sit in BSS on the no-PSRAM devices.
static AnimatedGIF* _rfGif = nullptr;
static bool _rfIsGif = false;
static bool _rfGifUseLittleFS = false;
static uint32_t _rfGifNextFrame = 0;
static uint8_t _rfGifLine[480]; // RGB332 scratch line (AnimatedGIF MAX_WIDTH = 480)
static uint8_t* _rfGifFrame = nullptr; // full frame at the GIF's native resolution (RGB332)
static int _rfGifCanvasW = 0;
static int _rfGifCanvasH = 0;

void _rfFree() {
	if (_rfLines != nullptr) { delete[] _rfLines; _rfLines = nullptr; }
	_rfTotalLines = 0;
	_rfIsImage = false;
	_rfIsGif = false;
	if (_rfGif != nullptr) { _rfGif->close(); delete _rfGif; _rfGif = nullptr; }
	if (_rfGifFrame != nullptr) { delete[] _rfGifFrame; _rfGifFrame = nullptr; }
	_rfGifCanvasW = 0;
	_rfGifCanvasH = 0;
	_rfZoom = 0.0f;
	_rfPanX = 0;
	_rfPanY = 0;
}

// --- AnimatedGIF file callbacks (bridge to Storage / LittleFS-or-SD) ---

static void* _rfGifFileOpen(const char* path, int32_t* pSize) {
	File f = Storage::open(path, "r", _rfGifUseLittleFS);
	if (!f) return nullptr;
	File* fp = new File(f);
	*pSize = fp->size();
	return fp;
}

static void _rfGifFileClose(void* handle) {
	if (handle != nullptr) {
		File* fp = (File*)handle;
		fp->close();
		delete fp;
	}
}

static int32_t _rfGifFileRead(GIFFILE* pFile, uint8_t* pBuf, int32_t iLen) {
	File* fp = (File*)pFile->fHandle;
	int32_t iBytesRead = iLen;
	if ((pFile->iSize - pFile->iPos) < iLen)
		iBytesRead = pFile->iSize - pFile->iPos;
	if (iBytesRead <= 0)
		return 0;
	fp->seek((uint32_t)pFile->iPos); // realign: AnimatedGIF tracks iPos, not File
	iBytesRead = (int32_t)fp->read(pBuf, (size_t)iBytesRead);
	pFile->iPos = (int32_t)fp->position();
	return iBytesRead;
}

static int32_t _rfGifFileSeek(GIFFILE* pFile, int32_t iPos) {
	File* fp = (File*)pFile->fHandle;
	fp->seek((uint32_t)iPos);
	pFile->iPos = iPos;
	return iPos;
}

// Converts a native RGB565 value to the canvas's RGB332 format (the canvas is
// RGB332; pushing uint8_t RGB332 data sidesteps M5GFX's byte-swap logic).
static uint8_t _rf565to332(uint16_t c) {
	return (uint8_t)(((((c >> 13) & 7) << 3) + ((c >> 8) & 7)) << 2) + ((c >> 3) & 3);
}

// Decodes one line of the current GIF frame (8-bit palette indices through the
// RGB565 palette) into the full-frame RGB332 buffer at the GIF's native
// resolution. Transparency leaves the previous frame's pixels in place, so the
// buffer accumulates the composed frame.
static void _rfGifDraw(GIFDRAW* pDraw) {
	if (_rfGifFrame == nullptr) return;

	int gw = _rfGifCanvasW;
	int gh = _rfGifCanvasH;

	int x = pDraw->iX;
	int y = pDraw->iY + pDraw->y;
	if (y < 0 || y >= gh) return;

	uint8_t* s = pDraw->pPixels;
	uint16_t* pal = pDraw->pPalette;
	int w = pDraw->iWidth;

	// Clip to the GIF canvas.
	if (x >= gw) return;
	if (x < 0) { s += -x; w += x; x = 0; }
	if (w <= 0) return;
	if (x + w > gw) w = gw - x;

	uint8_t* dst = &_rfGifFrame[y * gw + x];

	// Disposal method 2 (restore to background): substitute the background
	// color for transparent pixels and treat the line as fully opaque.
	if (pDraw->ucDisposalMethod == 2) {
		for (int i = 0; i < w; i++)
			if (s[i] == pDraw->ucTransparent) s[i] = pDraw->ucBackground;
		pDraw->ucHasTransparency = 0;
	}

	if (pDraw->ucHasTransparency) {
		uint8_t uc = pDraw->ucTransparent;
		for (int i = 0; i < w; i++)
			if (s[i] != uc) dst[i] = _rf565to332(pal[s[i]]);
	} else {
		for (int i = 0; i < w; i++)
			dst[i] = _rf565to332(pal[s[i]]);
	}
}

// Scales the accumulated full-frame buffer to fit the canvas (preserving aspect
// ratio, centered) and pushes it to the display.
static void _rfGifPresent() {
	int cw = canvas.width();
	int ch = canvas.height();
	int gw = _rfGifCanvasW;
	int gh = _rfGifCanvasH;
	if (gw <= 0 || gh <= 0 || _rfGifFrame == nullptr) return;

	// Uniform scale to fit, like the static image viewer's auto-fit.
	float s = min((float)cw / gw, (float)ch / gh);
	int outW = (int)(gw * s);
	int outH = (int)(gh * s);
	if (outW < 1) outW = 1;
	if (outH < 1) outH = 1;
	int offX = (cw - outW) / 2;
	int offY = (ch - outH) / 2;

	canvas.clear();
	for (int dy = 0; dy < outH; dy++) {
		int sy = (gh * dy) / outH; // nearest source row
		const uint8_t* src = &_rfGifFrame[sy * gw];
		for (int dx = 0; dx < outW; dx++) {
			int sx = (gw * dx) / outW;
			_rfGifLine[dx] = src[sx];
		}
		canvas.pushImage(offX, offY + dy, outW, 1, _rfGifLine);
	}
	canvas.pushSprite(0, getStatusBarOffset());
}

enum {
	_RF_GIF_OK = 0,
	_RF_GIF_ERR_OPEN,      // file can't be opened / not found
	_RF_GIF_ERR_BAD,       // not a valid GIF file
	_RF_GIF_ERR_TOO_WIDE,  // width > 480 px (AnimatedGIF MAX_WIDTH)
	_RF_GIF_ERR_TOO_LARGE, // width*height exceeds the frame-buffer RAM cap
	_RF_GIF_ERR_MEM,       // heap allocation failed
	_RF_GIF_ERR_DECODE,    // other decode error
};

static const char* _rfGifErrorMessage(int code) {
	switch (code) {
		case _RF_GIF_ERR_OPEN:      return "Can't open GIF file";
		case _RF_GIF_ERR_BAD:       return "Invalid GIF file";
		case _RF_GIF_ERR_TOO_WIDE:  return "GIF too wide (max 480px)";
		case _RF_GIF_ERR_TOO_LARGE: return "GIF too large";
		case _RF_GIF_ERR_MEM:       return "Not enough memory";
		default:                    return "Can't display GIF";
	}
}

static int _rfGifOpen() {
	if (_rfGif != nullptr) { _rfGif->close(); delete _rfGif; _rfGif = nullptr; }
	if (_rfGifFrame != nullptr) { delete[] _rfGifFrame; _rfGifFrame = nullptr; }
	_rfGifCanvasW = 0;
	_rfGifCanvasH = 0;
	_rfGifUseLittleFS = (selectedFileSourcePid != PID::FILE_PICKER_SD);
	_rfGif = new AnimatedGIF();
	if (_rfGif == nullptr) return _RF_GIF_ERR_MEM;
	_rfGif->begin(GIF_PALETTE_RGB565_LE);
	if (!_rfGif->open(selectedFilePath.c_str(), _rfGifFileOpen, _rfGifFileClose,
		              _rfGifFileRead, _rfGifFileSeek, _rfGifDraw)) {
		int err = _rfGif->getLastError();
		delete _rfGif;
		_rfGif = nullptr;
		if (err == GIF_TOO_WIDE)      return _RF_GIF_ERR_TOO_WIDE;
		if (err == GIF_FILE_NOT_OPEN) return _RF_GIF_ERR_OPEN;
		if (err == GIF_BAD_FILE || err == GIF_EARLY_EOF) return _RF_GIF_ERR_BAD;
		return _RF_GIF_ERR_DECODE;
	}
	_rfGifCanvasW = _rfGif->getCanvasWidth();
	_rfGifCanvasH = _rfGif->getCanvasHeight();
	if (_rfGifCanvasW <= 0 || _rfGifCanvasH <= 0
	    || (long)_rfGifCanvasW * _rfGifCanvasH > 480L * 480L) {
		_rfGif->close();
		delete _rfGif;
		_rfGif = nullptr;
		_rfGifCanvasW = 0;
		_rfGifCanvasH = 0;
		return _RF_GIF_ERR_TOO_LARGE;
	}
	_rfGifFrame = new uint8_t[_rfGifCanvasW * _rfGifCanvasH];
	if (_rfGifFrame == nullptr) {
		_rfGif->close();
		delete _rfGif;
		_rfGif = nullptr;
		_rfGifCanvasW = 0;
		_rfGifCanvasH = 0;
		return _RF_GIF_ERR_MEM;
	}
	memset(_rfGifFrame, 0, _rfGifCanvasW * _rfGifCanvasH);
	_rfGifNextFrame = 0;
	return _RF_GIF_OK;
}

void _rfLoad() {
	_rfFree();
	_rfScrollOffset = 0;

	bool useLittleFS = (selectedFileSourcePid != PID::FILE_PICKER_SD);
	File f = Storage::open(selectedFilePath.c_str(), "r", useLittleFS);

	if (!f) { centeredPrint("open error", MEDIUM_TEXT); return; }

	int count = 0;
	while (f.available()) {
		f.readStringUntil('\n');
		count++;
	}
	_rfTotalLines = count;

	canvas.setTextSize(TINY_TEXT);
	int maxW = canvas.width() - 5 - 5 - 5; // left pad + scrollbar + margin
	int charW = canvas.textWidth("W");
	int maxChars = (charW > 0) ? (maxW / charW) : 32;

	_rfLines = new String[count];
	f.seek(0);
	for (int i = 0; i < count && f.available(); i++) {
		_rfLines[i] = f.readStringUntil('\n');
		_rfLines[i].trim();
		if (int(_rfLines[i].length()) > maxChars)
			_rfLines[i] = _rfLines[i].substring(0, maxChars);
	}
	f.close();
}

void _rfDraw() {
	canvas.clear();
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(TINY_TEXT);
	canvas.setCursor(0, 0);

	int paddingX = 5;
	int lineHeight = canvas.fontHeight();
	int visibleLines = (canvas.height() - paddingX) / lineHeight;

	for (int i = 0; i < visibleLines; i++) {
		int idx = _rfScrollOffset + i;
		if (idx >= _rfTotalLines) break;
		canvas.setCursor(paddingX, i * lineHeight);
		canvas.print(_rfLines[idx].c_str());
	}

	// scrollbar: slider height = visible/total, position = offset/scrollable
	if (_rfTotalLines > visibleLines) {
		int sbW = 5;
		int sbX = canvas.width() - sbW;
		int sbH = canvas.height();
		int sliderH = max(6, sbH * visibleLines / _rfTotalLines);
		int scrollable = _rfTotalLines - visibleLines;
		int sliderY = (_rfScrollOffset * (sbH - sliderH)) / scrollable;
		canvas.fillRect(sbX, 0, sbW, sbH, BGCOLOR);
		canvas.fillRect(sbX, sliderY, sbW, sliderH, FGCOLOR);
	}
	canvas.pushSprite(0, getStatusBarOffset());
}

static bool _rfIsImageExt(const String& path) {
	int dot = path.lastIndexOf('.');
	if (dot < 0) return false;
	String ext = path.substring(dot);
	ext.toLowerCase();
	return ext == ".jpg" || ext == ".jpeg" || ext == ".png"
		|| ext == ".bmp" || ext == ".qoi";
}

static bool _rfIsGifExt(const String& path) {
	int dot = path.lastIndexOf('.');
	if (dot < 0) return false;
	String ext = path.substring(dot);
	ext.toLowerCase();
	return ext == ".gif";
}

static bool _rfDrawImage() {
	canvas.clear();

	// Must pass fs::FS& (not sd::SDFS& or LittleFSFS&) so the M5GFX template
	// instantiates DataWrapperT<fs::FS>, which has a working specialization.
	fs::FS& filesys = Storage::getFS(selectedFileSourcePid != PID::FILE_PICKER_SD);

	String ext = selectedFilePath.substring(selectedFilePath.lastIndexOf('.'));
	ext.toLowerCase();

	// Zoom: when _rfZoom > 0, scale the effective fit-area so M5GFX auto-fit
	// produces a larger zoom. Pan: offX/offY skip rendered pixels from edges.
	int32_t maxW = canvas.width();
	int32_t maxH = canvas.height();
	float scaleX = 0.0f; // 0 = auto-fit in M5GFX

	if (_rfZoom > 0.001f) {
		maxW = roundf(canvas.width() * _rfZoom);
		maxH = roundf(canvas.height() * _rfZoom);
		scaleX = 0.0f;
	}

	bool ok = false;

	if (ext == ".jpg" || ext == ".jpeg") {
		ok = canvas.drawJpgFile(filesys, selectedFilePath.c_str(), 0, 0, maxW, maxH, _rfPanX, _rfPanY, scaleX);
	} else if (ext == ".png") {
		ok = canvas.drawPngFile(filesys, selectedFilePath.c_str(), 0, 0, maxW, maxH, _rfPanX, _rfPanY, scaleX);
	} else if (ext == ".bmp") {
		ok = canvas.drawBmpFile(filesys, selectedFilePath.c_str(), 0, 0, maxW, maxH, _rfPanX, _rfPanY, scaleX);
	} else if (ext == ".qoi") {
		ok = canvas.drawQoiFile(filesys, selectedFilePath.c_str(), 0, 0, maxW, maxH, _rfPanX, _rfPanY, scaleX);
	}

	if (ok) canvas.pushSprite(0, getStatusBarOffset());
	return ok;
}

void readFileLoop() {
	if (isSetup()) {
		if (_rfIsGifExt(selectedFilePath)) {
			_rfIsGif = true;
			int err = _rfGifOpen();
			if (err != _RF_GIF_OK)
				centeredPrint(_rfGifErrorMessage(err), MEDIUM_TEXT);
		} else if (_rfIsImageExt(selectedFilePath)) {
			_rfIsImage = true;
			_rfZoom = 0.0f;
			_rfPanX = 0;
			_rfPanY = 0;
			if (!_rfDrawImage()) {
				centeredPrint("Can't display image", MEDIUM_TEXT);
			}
		} else {
			_rfLoad();
			if (_rfLines == nullptr) centeredPrint("File empty", MEDIUM_TEXT);
			else _rfDraw();
		}
	}

	// GIF
	if (_rfIsGif) {
		if (_rfGif != nullptr && millis() >= _rfGifNextFrame) {
			int delayMs = 0;
			if (_rfGif->playFrame(false, &delayMs) >= 0)
				_rfGifPresent();
			// Frames without a Graphic Control Extension report 0 delay; enforce
			// a minimum so the animation doesn't spin at loop speed.
			_rfGifNextFrame = millis() + (delayMs > 0 ? delayMs : 100);
		}
		if (checkExit()) _rfFree();
		return;
	}

	// Image
	if (_rfIsImage) {
		// Zoom
		if (isKbPlusPressed() || isBtnAWasPressed()) {
			_rfZoom = (_rfZoom < 1.0f) ? 1.0f : _rfZoom * 1.25f;
			if (_rfZoom > 8.0f) _rfZoom = 8.0f;
			_rfDrawImage();
		}
		if (isKbMinusPressed() || isBtnPWRWasPressed()) {
			if (_rfZoom > 0.001f) {
				_rfZoom /= 1.25f;
				if (_rfZoom < 1.0f) _rfZoom = 0.0f;
				_rfDrawImage();
			}
		}

		// Pan
		if (_rfZoom > 0.001f) {
			int step = max(10, int(roundf(20.0f * _rfZoom)));
			if (isKbLeftPressed() && _rfPanX > 0) { _rfPanX -= step; _rfDrawImage(); }
			if (isKbRightPressed()) { _rfPanX += step; _rfDrawImage(); }
			if (isKbUpPressed() && _rfPanY > 0) { _rfPanY -= step; _rfDrawImage(); }
			if (isKbDownPressed()) { _rfPanY += step; _rfDrawImage(); }
		} else {
			_rfPanX = 0;
			_rfPanY = 0;
		}

		checkExit();
		return;
	}

	// Text
	if (_rfLines == nullptr) {
		checkExit();
		return;
	}

	int visibleLines = canvas.height() / canvas.fontHeight();

	if (isBtnAWasPressed() || isKbDownPressed()) {
		if (_rfScrollOffset + visibleLines < _rfTotalLines) {
			_rfScrollOffset++;
			_rfDraw();
		}
	} else if (isBtnPWRWasPressed() || isKbUpPressed()) {
		if (_rfScrollOffset > 0) {
			_rfScrollOffset--;
			_rfDraw();
		}
	}

	if (checkExit()) {
		_rfFree();
	}
}
