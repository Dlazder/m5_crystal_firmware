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
	<tr>
		<td><code>cursorOnTop</code><br>Places the cursor at the top of the accessible area
 * (depending on the presence of the status bar)</td>
		<td><code>cursorOnTop();
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
MENU exampleMenu[] = {
	{1, "clock"},
	{3, "settings"},
	{13, "Wi-Fi"},
	{14, "Bluetooth"},
	{20, "level tool"},
	{2, "Battery info"},
};
int mainMenuSize = sizeof(mainMenu) / sizeof(MENU);

void mainMenuLoop() {
	if (isSetup()) {
		cursorOnTop();
		drawMenu(mainMenu, mainMenuSize);
	}
	menuLoop(mainMenu, mainMenuSize);
}
```


## LittleFS utils

*Utilities for working with the LittleFS filesystem. Use these to browse and select files stored on the device.*

<table>
	<tr>
		<td><code>lfsPickFile</code><br>Redirects to the file picker process (PID 44) if no file is selected yet. Returns <code>true</code> if redirected. Use with <code>return</code> to stop setup until a file is chosen.</td>
		<td><code>if (lfsPickFile()) return;</code></td>
	</tr>
	<tr>
		<td><code>lfsSelectedFile</code><br>Global variable. Contains the full path of the file selected in the picker (e.g. <code>"/notepad.txt"</code>). Empty string if no file has been selected.</td>
		<td><code>if (lfsSelectedFile != "") { ... }</code></td>
	</tr>
</table>

### *Basic LittleFS usage example*

```cpp
void myLoop() {
	if (isSetup()) {
		lfsSelectedFile = "";
	}

	if (lfsPickFile()) return; // goes to picker, comes back here after selection

	// lfsSelectedFile now contains the chosen path
	File f = LittleFS.open(lfsSelectedFile, "r");
}
```

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