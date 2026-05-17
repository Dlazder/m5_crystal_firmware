## Example VS Code configuration

If you are using the platformio extension you do not need this configuration

*.vscode/settings.json*

```json
{
		"C_Cpp.intelliSenseEngine": "default",
		"C_Cpp.default.includePath": [
				// [Windows] (Adjust path for your locations)
				"${env:USERPROFILE}/AppData/Local/Arduino15/packages/**",
				"${env:USERPROFILE}/AppData/Local/Arduino15/**",
				"${env:USERPROFILE}/OneDrive/Documents/Arduino/libraries/**",
				
				// Linux
				// Later
		],
		// For memcpy function recognition
		"C_Cpp.default.defines": [
				"_GLIBXX_HAVE_MEMCPY",
		],

		"arduino.enableUSBDetection": true,
		"arduino.additionalUrls": ["https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json"],
		"arduino.disableIntelliSenseAutoGen": true,
}
```
