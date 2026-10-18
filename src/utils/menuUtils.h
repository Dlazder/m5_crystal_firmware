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
const int MENU_VISIBLE_ITEMS = 5; // max menu items rendered at once

// Inline editing of a pin/range setting registered in the settings service
// (MENU::settingKey found in the registry). While editing, the pending value
// lives here and is only persisted on confirm. Toggles are flipped directly.
static bool menuEditing = false;
static int menuEditValue = 0;
static int menuEditOriginal = 0; // value before editing, restored by ESC on live settings

// The registered setting bound to a menu item, or nullptr for plain items.
static const Setting* menuItemSetting(const MENU& item) {
	return item.settingKey != nullptr ? Settings::find(item.settingKey) : nullptr;
}

void drawMenu(MENU menu[], int size) {
	if (cursor >= size) cursor = cursor % size;
	if (cursor < 0) cursor = size - 1;
	if (cursor > 2 && size <= 2) cursor = 0;

	canvas.clear();
	// Custom text size for perfect placement of menu items
	canvas.setTextSize(MENU_TEXT);
	canvas.setCursor(0, 0);

	int start = (cursor > 2) ? (cursor - 2) : 0;

	int lineHeight = canvas.fontHeight();
	for (int i = start; i < size && i < start + MENU_VISIBLE_ITEMS; i++) {
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
		String label = menu[i].name.substring(0, MENU_LABEL_MAX_CHARS);

		// Value of a pin/range setting, right-aligned; "!" marks a conflicting pin,
		// "<...>" marks the item being edited.
		const Setting* setting = menuItemSetting(menu[i]);
		bool isToggle = (setting != nullptr && setting->type == SettingType::TOGGLE);
		bool hasValue = (setting != nullptr && !isToggle);
		String valueText = "";
		int valueX = 0;
		if (hasValue) {
			bool editing = menuEditing && selected;
			int v = editing ? menuEditValue : Settings::get(setting);
			valueText = Settings::format(setting, v);
			if (Settings::hasConflict(setting, v)) valueText = "!" + valueText;
			if (editing) valueText = "<" + valueText + ">";
			valueX = canvas.width() - 5 /*SCROLLBAR_WIDTH*/ - MENU_CHECKBOX_SCROLLBAR_GAP - canvas.textWidth(valueText.c_str());
			// Keep the label clear of the value.
			while (label.length() > 1 && textX + canvas.textWidth(label.c_str()) + MENU_ICON_TEXT_GAP > valueX) {
				label.remove(label.length() - 1);
			}
		}

		canvas.setCursor(textX, y);
		canvas.print(label.c_str());
		if (hasValue) {
			canvas.setCursor(valueX, y);
			canvas.print(valueText.c_str());
		}

		// Draw toggle checkbox for settings items
		if (isToggle) {
			int cbX = canvas.width() - 5 /*SCROLLBAR_WIDTH*/ - MENU_CHECKBOX_SCROLLBAR_GAP - MENU_CHECKBOX_SIZE;
			int cbY = y + (lineHeight - MENU_CHECKBOX_SIZE) / 2;
			int cbColor = selected ? BGCOLOR : FGCOLOR;
			bool isChecked = (Settings::get(setting) != 0);
			canvas.drawRect(cbX, cbY, MENU_CHECKBOX_SIZE, MENU_CHECKBOX_SIZE, cbColor);
			if (isChecked) {
				canvas.fillRect(cbX + 2, cbY + 2, MENU_CHECKBOX_SIZE - 4, MENU_CHECKBOX_SIZE - 4, cbColor);
			}
		}
	}

	drawScrollbar(cursor, size, MENU_VISIBLE_ITEMS);
	canvas.pushSprite(0, getStatusBarOffset());
}

// Input while a setting is being edited. Keyboard: left/right (or up/down) change the
// value, Enter confirms, ESC cancels. Buttons: B / PWR change it, A confirms.
static void menuEditLoop(MENU menu[], int size) {
	const Setting* s = menuItemSetting(menu[cursor]);

	if (isKbEscPressed()) {
		if (s->live) Settings::set(s, menuEditOriginal, false);
		menuEditing = false;
		drawMenu(menu, size);
		return;
	}
	if (isBtnAWasPressed() || isKbEnterPressed()) {
		// Live settings were already applied step by step: only persist.
		if (s->live) Settings::commit(s);
		else Settings::set(s, menuEditValue);
		menuEditing = false;
		drawMenu(menu, size);
		return;
	}
	int dir = 0;
	if (isBtnBWasPressed() || isKbRightPressed() || isKbDownPressed() || isWebControlDownWasPressed()) dir = 1;
	if (isBtnPWRWasPressed() || isKbLeftPressed() || isKbUpPressed() || isWebControlUpWasPressed()) dir = -1;
	if (dir != 0) {
		menuEditValue = Settings::step(s, menuEditValue, dir);
		if (s->live) Settings::set(s, menuEditValue, false);
		drawMenu(menu, size);
	}
	if (isWebDataRequested()) {
		webData = generateWebData("menu", generateMenuString(menu, size));
	}
}

void menuLoop(MENU menu[], int size) {
	// A setting is being edited: the menu navigation below is suspended.
	if (menuEditing) {
		menuEditLoop(menu, size);
		return;
	}
	// ESC returns to this menu's parent (the "Back" item at index 0).
	if (isKbEscPressed()) {
		if (process == PID::MAIN_MENU) return; // root menu has no parent
		changeProcess(menu[0].command);
		return;
	}
	if (isBtnBWasPressed() || isKbDownPressed() || isWebControlDownWasPressed()) {
		cursor++;
		drawMenu(menu, size);
	}
	if (isBtnAWasPressed() || isKbEnterPressed()) {
		const Setting* setting = menuItemSetting(menu[cursor]);
		if (setting != nullptr && setting->type == SettingType::TOGGLE) {
			Settings::set(setting, Settings::step(setting, Settings::get(setting), 1));
			drawMenu(menu, size);
			return;
		} else if (setting != nullptr) {
			// pin/range setting: start inline editing
			menuEditing = true;
			menuEditValue = menuEditOriginal = Settings::get(setting);
			drawMenu(menu, size);
			return;
		} else if (menu[cursor].settingKey != nullptr) {
			// key not in the registry (e.g. a feature compiled out): nothing to change
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