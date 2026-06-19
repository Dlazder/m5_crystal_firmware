void drawScrollbar(int currentCursor, int totalItems, int visibleItems) {
	static int SCROLLBAR_WIDTH = 5;
	int SCROLLBAR_X = canvas.width() - SCROLLBAR_WIDTH;

	if (totalItems <= visibleItems) {
		// canvas.fillRect(SCROLLBAR_X, 0, SCROLLBAR_WIDTH, canvas.height(), BGCOLOR);
		return;
	}

	int sliderHeight = (canvas.height() * visibleItems) / totalItems;
	if (sliderHeight < 6) sliderHeight = 6;

	int availableSpace = canvas.height() - sliderHeight;
	int sliderY = (currentCursor * availableSpace) / (totalItems - 1);

	canvas.fillRect(SCROLLBAR_X, 0, SCROLLBAR_WIDTH, canvas.height(), BGCOLOR);
	canvas.fillRect(SCROLLBAR_X, sliderY, SCROLLBAR_WIDTH, sliderHeight, FGCOLOR);
	// canvas.fillRect(SCROLLBAR_X, 0, 1, canvas.height(), FGCOLOR);
	// canvas.fillRect(SCROLLBAR_X + 1, 0, 1, canvas.height(), BGCOLOR);
}

const int MENU_LABEL_MAX_CHARS = 18;

void drawMenu(MENU menu[], int size) {
	if (cursor == size) cursor = cursor % size;
	if (cursor < 0) cursor = size - 1;
	if (cursor > 2 && size <= 2) cursor = 0;

	canvas.clear();
	// Custom text size for perfect placement of menu items
	canvas.setTextSize(MENU_TEXT);
	canvas.setCursor(0, 0);

	int start = (cursor > 2) ? (cursor - 2) : 0;

	int lineHeight = canvas.fontHeight();
	for (int i = start; i < size; i++) {
		int y = (i - start) * lineHeight;
		bool selected = (cursor == i);
		canvas.fillRect(0, y, canvas.width(), lineHeight, selected ? FGCOLOR : BGCOLOR);
		canvas.setTextColor(selected ? BGCOLOR : FGCOLOR, selected ? FGCOLOR : BGCOLOR);
		canvas.setCursor(5, y);
		canvas.print(menu[i].name.substring(0, MENU_LABEL_MAX_CHARS).c_str());
	}

	drawScrollbar(cursor, size, 5);
	canvas.pushSprite(0, getStatusBarOffset());
}

void menuLoop(MENU menu[], int size) {
	if (isBtnBWasPressed() || isKbDownPressed() || isWebControlDownWasPressed()) {
		cursor++;
		drawMenu(menu, size);
	}
	if (isBtnAWasPressed() || isKbEnterPressed()) {
		changeProcess(menu[cursor].command);
		return;
	}
	if (isBtnPWRWasPressed() || isKbUpPressed() || isWebControlUpWasPressed()) {
		cursor--;
		drawMenu(menu, size);
	}
	if (isWebDataRequested()) {
		webData = generateWebData("menu", generateMenuString(menu, size));
	}
}