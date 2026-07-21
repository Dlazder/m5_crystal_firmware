#include "../system/settingHandlers.h"

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

const int MENU_LABEL_MAX_CHARS = 22;
const int MENU_ICON_TEXT_GAP = 5;  // pixels between icon and text
const int MENU_CHECKBOX_SIZE = 10; // 10x10 pixels for toggle checkboxes
const int MENU_CHECKBOX_SCROLLBAR_GAP = 3; // gap between checkbox and scrollbar

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

		bool hasIcon = (menu[i].icon != nullptr);
		int textX;
		if (hasIcon && iconsEnabled) {
			int iconY = y + (lineHeight - MENU_ICON_H) / 2;
			canvas.drawBitmap(5, iconY,
				menu[i].icon,
				MENU_ICON_W, MENU_ICON_H,
				selected ? BGCOLOR : FGCOLOR);
			textX = 5 + MENU_ICON_W + MENU_ICON_TEXT_GAP;
		} else {
			textX = 5;
		}
		canvas.setCursor(textX, y);
		canvas.print(menu[i].name.substring(0, MENU_LABEL_MAX_CHARS).c_str());

		// Draw toggle checkbox for settings items
		if (menu[i].settingKey != nullptr) {
			int cbX = canvas.width() - 5 /*SCROLLBAR_WIDTH*/ - MENU_CHECKBOX_SCROLLBAR_GAP - MENU_CHECKBOX_SIZE;
			int cbY = y + (lineHeight - MENU_CHECKBOX_SIZE) / 2;
			int cbColor = selected ? BGCOLOR : FGCOLOR;
			bool isChecked = (getData(menu[i].settingKey, 0) == 1);
			canvas.drawRect(cbX, cbY, MENU_CHECKBOX_SIZE, MENU_CHECKBOX_SIZE, cbColor);
			if (isChecked) {
				canvas.fillRect(cbX + 2, cbY + 2, MENU_CHECKBOX_SIZE - 4, MENU_CHECKBOX_SIZE - 4, cbColor);
			}
		}
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
		if (menu[cursor].settingKey != nullptr) {
			// toggle settings
			handleSettingToggle(menu[cursor].settingKey);
			drawMenu(menu, size);
			return;
		} else {
			// toggle process (default behavior)
			changeProcess(menu[cursor].command);
			return;
		}
	}
	if (isBtnPWRWasPressed() || isKbUpPressed() || isWebControlUpWasPressed()) {
		cursor--;
		drawMenu(menu, size);
	}
	if (isWebDataRequested()) {
		webData = generateWebData("menu", generateMenuString(menu, size));
	}
}