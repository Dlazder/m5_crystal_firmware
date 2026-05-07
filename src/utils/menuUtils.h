void drawScrollbar(int currentCursor, int totalItems, int visibleItems) {
	static int SCROLLBAR_WIDTH = 5;
	int SCROLLBAR_X = canvas.width() - SCROLLBAR_WIDTH;
	int SCROLLBAR_HEIGHT = canvas.height() - getStatusBarOffset();

	if (totalItems <= visibleItems) {
		// canvas.fillRect(SCROLLBAR_X, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT, BGCOLOR);
		return;
	}

	int sliderHeight = (SCROLLBAR_HEIGHT * visibleItems) / totalItems;
	if (sliderHeight < 6) sliderHeight = 6;

	int availableSpace = SCROLLBAR_HEIGHT - sliderHeight;
	int sliderY = (currentCursor * availableSpace) / (totalItems - 1);

	canvas.fillRect(SCROLLBAR_X, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT, BGCOLOR);
	canvas.fillRect(SCROLLBAR_X, sliderY, SCROLLBAR_WIDTH, sliderHeight, FGCOLOR);
}

void drawMenu(MENU menu[], int size) {
	if (cursor == size) cursor = cursor % size;
	if (cursor < 0) cursor = size - 1;
	if (cursor > 2 && size <= 2) cursor = 0;

	canvas.clear();
	// Custom text size for perfect placement of menu items
	canvas.setTextSize(1.8);
	canvas.setCursor(0, 0);

	int start = (cursor > 2) ? (cursor - 2) : 0;

	int lineHeight = canvas.fontHeight();
	for (int i = start; i < size; i++) {
		int y = (i - start) * lineHeight;
		bool selected = (cursor == i);
		canvas.fillRect(0, y, canvas.width(), lineHeight, selected ? FGCOLOR : BGCOLOR);
		canvas.setTextColor(selected ? BGCOLOR : FGCOLOR, selected ? FGCOLOR : BGCOLOR);
		canvas.setCursor(5, y);
		canvas.print(menu[i].name.c_str());
	}

	drawScrollbar(cursor, size, 5);
	canvas.pushSprite(0, getStatusBarOffset());
}

int selectedItem = 0;
void menuLoop(MENU menu[], int size, bool updateSelectedItem = false) {
	DEVICE.update();
	if (isBtnBWasPressed() || isWebControlDownWasPressed()) {
		cursor++;
		drawMenu(menu, size);
	}
	if (isBtnAWasPressed()) {
		changeProcess(menu[cursor].command);
		if (updateSelectedItem) selectedItem = menu[cursor].command;
		return;
	}
	if (isBtnPWRWasPressed() || isWebControlUpWasPressed()) {
		cursor--;
		drawMenu(menu, size);
	}
	if (isWebDataRequested()) {
		webData = generateWebData("menu", generateMenuString(menu, size));
	}
}

bool isMenuItemSelected(int pid) {
	if (pid == selectedItem) {
		selectedItem = -1;
		return true;
	} else return false;
}