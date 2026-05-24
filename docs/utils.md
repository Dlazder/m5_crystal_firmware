# Description of utilities

*All the main utilities that will help you create your own programs are described here.*

## Display utils (interface)

<table>
	<tr>
		<td><code>centeredPrint</code><br>print text on center of the screen</td>
		<td><code>centeredPrint("Hello world!", MEDIUM_TEXT);</code></td>
	</tr>
	<tr>
		<td><code>centeredPrintRows</code><br>prints several lines in the center of the screen</td>
		<td><code>String lines[] = {
			"Text 1",
			"text 2"
		};
		centeredPrintRows(lines, 2, MEDIUM_TEXT);
		</code></td>
	</tr>
</table>

## Button utils

<table>
	<tr>
		<td><code>isBtnBWasPressed</code><br><code>isBtnAWasPressed</code><br>Checks physical button clicks (and clicks from the web interface)</td>
		<td><code>if (isBtnAWasPressed()) {Serial.print("btn A pressed!")}</code></td>
	</tr>
	<tr>
		<td><code>pressBtnB</code><br><code>pressBtnA</code><br>Emulates a button press.</td>
		<td><code>pressBtnA();<br>if (isBtnAWasPressed()) {Serial.print("btn A pressed!")}</code></td>
	</tr>
	<tr>
</table>

## Process managment utils

<table>
	<tr>
		<td><code>isSetup</code><br>Ensures that code within a condition is executed only once when the program starts.</td>
		<td><code>if (isSetup()) {Serial.print("run only ones!")}</code></td>
	</tr>
	<tr>
		<td><code>checkExit</code><br>Allows you to handle exit from your program. If the user pressed button B it will go back to the previous process or you can pass the process id manually.</td>
		<td><code>checkExit();</code></td>
	</tr>
	<tr>
		<td><code>changeProcess</code><br>Switches to the process passed in parameters</td>
		<td><code>changeProcess(0);</code></td>
	</tr>
</table>

## Storage utils

<table>
	<tr>
		<td><code>setData</code><br>Writes data to internal memory. Pass the key and value.</td>
		<td><code>setData("brightness", 10);</code></td>
	</tr>
	<tr>
		<td><code>getData</code><br>Get data from internal memory. Pass a key, and a default value that will be returned if the data does not exist.</td>
		<td><code>int brightness = 1;<br>brightness = getData("brightness", brightness)</code></td>
	</tr>
</table>

## Timer utils

<table>
	<tr>
		<td><code>checkTimer</code><br>Checks whether the specified time has passed since the timer was last updated.
		Useful for preventing accidental clicks when starting your program, or if you just need to perform an action after a certain period of time.</td>
		<td><code>checkTimer(1000); // 1 second</code></td>
	</tr>
	<tr>
		<td><code>updateTimer</code><br>Updates the timer, it starts running again from 0. Used in conjunction with <code>checkTimer</code></td>
		<td><code>updateTimer();</code></td>
	</tr>
</table>

## Menu utils

*If you want to create your own menu through which you can launch other processes, you need to use these simple utilities.*

<table>
	<tr>
		<td><code>drawMenu</code><br>Draws the system menu. Takes a MENU object and its size.</td>
		<td><code>drawMenu(mainMenu, mainMenuSize);</code></td>
	</tr>
	<tr>
		<td><code>menuLoop</code><br>Completely handles the rendering of the system menu and navigation through it. Call in main loop of your menu process.</td>
		<td><code>menuLoop(mainMenu, mainMenuSize);</code></td>
	</tr>
</table>

### *Basic menu usage example*

```cpp
void exampleMenuLoop() {
	MENU exampleMenu[] = {
		{PID::CLOCK,     "clock"},
		{PID::SETTINGS,  "settings"},
		{PID::WIFI,      "Wi-Fi"},
		{PID::BLUETOOTH, "Bluetooth"},
	};
	int exampleMenuSize = sizeof(exampleMenu) / sizeof(MENU);

	if (isSetup()) {
		drawMenu(exampleMenu, exampleMenuSize);
	}
	menuLoop(exampleMenu, exampleMenuSize);
}
```


## File picker utils

*Utilities for browsing and selecting files stored on the device (LittleFS or SD card).*

<table>
	<tr>
		<td><code>filePickerSetup(cancelPid)</code><br>Opens the file picker. Shows a source selection menu (LittleFS / SD card). <code>cancelPid</code> is the process to return to if the user cancels.</td>
		<td><code>filePickerSetup(PID::MY_PROCESS);</code></td>
	</tr>
	<tr>
		<td><code>filePickerLoop()</code><br>Handles file picker input. Call every loop tick while <code>fpActive</code> is true. Returns <code>true</code> while the picker is still running. On selection sets <code>selectedFilePath</code>; on cancel switches process to <code>cancelPid</code>.</td>
		<td><code>if (fpActive) { if (filePickerLoop()) return; }</code></td>
	</tr>
	<tr>
		<td><code>fpActive</code><br>Global flag. <code>true</code> while the file picker is open.</td>
		<td><code>if (fpActive) { ... }</code></td>
	</tr>
	<tr>
		<td><code>selectedFilePath</code><br>Global variable. Contains the full path of the selected file (e.g. <code>"/script.txt"</code>). Empty string if no file has been selected.</td>
		<td><code>if (selectedFilePath != "") { ... }</code></td>
	</tr>
</table>

### *Basic file picker usage example*

```cpp
void myLoop() {
	if (isSetup()) {
		filePickerSetup(PID::MAIN_MENU);
	}

	// File picker phase
	if (fpActive) {
		if (filePickerLoop()) return;
		if (selectedFilePath == "") return; // cancelled
	}

	// selectedFilePath is now set — open and work with the file
	File f = LittleFS.open(selectedFilePath, "r");
}
```

## Keyboard utils

*On-screen keyboard for text input. On devices with a physical keyboard — uses it directly; on others — navigated by buttons or gyroscope.*

<table>
	<tr>
		<td><code>kbReset</code><br>Clears the input buffer and resets all keyboard state. Call in setup before showing the keyboard.</td>
		<td><code>kbReset();</code></td>
	</tr>
	<tr>
		<td><code>kbEnd</code><br>Exits text input mode. Call when the keyboard is no longer needed (e.g. after the user confirms input).</td>
		<td><code>kbEnd();</code></td>
	</tr>
	<tr>
		<td><code>drawKeyboardUi</code><br>Renders the keyboard on screen. Call once in setup to show it initially.</td>
		<td><code>drawKeyboardUi();</code></td>
	</tr>
	<tr>
		<td><code>keyboardLoop(onExit, onEnter, onChar)</code><br>Handles keyboard input. Call every loop tick while the keyboard is active. Returns <code>true</code> when the user confirms or exits.<br><br>
		<b>onExit</b> — called when the user cancels (exit key).<br>
		<b>onEnter</b> — called with the final <code>const char* buf</code> when the user confirms.<br>
		<b>onChar</b> — called on each character typed (can be <code>nullptr</code>).</td>
		<td><code>if (keyboardLoop(onExit, onEnter, nullptr)) return;</code></td>
	</tr>
</table>

### *Basic keyboard usage example*

```cpp
static bool inputDone = false;
static String inputResult = "";

void myLoop() {
	if (isSetup()) {
		inputDone = false;
		kbReset();
		drawKeyboardUi();
	}

	if (!inputDone) {
		if (keyboardLoop(
			[]() { changeProcess(PID::MAIN_MENU); }, // onExit — cancel
			[](const char* buf) {                    // onEnter — confirm
				inputResult = String(buf);
				inputDone = true;
				kbEnd();
			},
			nullptr
		)) return;
		return;
	}

	// inputResult now contains the typed text
}
```

## Sound utils

*Ready-made sound signals for common events. All functions play a short melody via the built-in speaker.*

<table>
	<tr>
		<td><code>soundStartup</code><br>Plays the startup melody (three ascending tones). Respects the <code>startupSound</code> setting — silent if disabled.</td>
		<td><code>soundStartup();</code></td>
	</tr>
	<tr>
		<td><code>soundConnected</code><br>Two ascending tones. Use when a connection is established (Wi-Fi, Bluetooth, etc.).</td>
		<td><code>soundConnected();</code></td>
	</tr>
	<tr>
		<td><code>soundDisconnected</code><br>Two descending tones. Use when a connection is lost.</td>
		<td><code>soundDisconnected();</code></td>
	</tr>
	<tr>
		<td><code>soundSuccess</code><br>Two ascending tones. Use to confirm a successful operation.</td>
		<td><code>soundSuccess();</code></td>
	</tr>
	<tr>
		<td><code>soundError</code><br>Two low descending tones. Use to signal an error or failure.</td>
		<td><code>soundError();</code></td>
	</tr>
	<tr>
		<td><code>soundBeep</code><br>Single short beep. Use for UI feedback (button press, selection, etc.).</td>
		<td><code>soundBeep();</code></td>
	</tr>
</table>

## Web interface utils

#### Will be described later when the web interface is fully implemented

<!-- <table>
<tr>
		<td><code>isWebDataRequested</code><br>Checks whether a new line should be generated describing the current state of the interface. You will most likely need to pass the string "function" as the first argument. The second argument is a string that describes the contents of the interface.</td>
		<td><code>if (isWebDataRequested()) {webData = generateWebData("function", generateFunctionElement("text", MEDIUM_TEXT, "center"));}</code></td>
	</tr>
	<tr>
		<td><code>generateFunctionElement</code><br>Creates and returns a string that describes the text parameters to be displayed inside the web interface. The first parameter is a line with text, the second is size, the third is centering.
		The resulting strings can be combined.</td>
		<td><code>generateFunctionElement("text", MEDIUM_TEXT, "center")<br> // return  "text,2,center;"</code></td>
	</tr>
</table> -->