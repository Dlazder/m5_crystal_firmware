// PID::FILE_VIEW

static int _rfScrollOffset = 0;
static int _rfTotalLines = 0;
static String* _rfLines = nullptr;

void _rfFree() {
	if (_rfLines != nullptr) { delete[] _rfLines; _rfLines = nullptr; }
	_rfTotalLines = 0;
}

void _rfLoad() {
	_rfFree();
	_rfScrollOffset = 0;

	File f;
	if (selectedFileSourcePid == PID::FILE_PICKER_SD) {
		f = SD.open(selectedFilePath.c_str());
	} else {
		f = LittleFS.open(selectedFilePath.c_str());
	}

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
		if ((int)_rfLines[i].length() > maxChars)
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

void readFileLoop() {
	if (isSetup()) {
		_rfLoad();
		if (_rfLines == nullptr) centeredPrint("File empty", MEDIUM_TEXT);
		else _rfDraw();
	}

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
