# How to build and flash firmware

The easiest way to install firmware on your device is to use m5 burner. But if you want to modify the firmware, you need to build it yourself. This file describes possible build methods. If something doesn’t work out, ask questions via issues.

- [platformio in vs code (recommended)](#building-using-platformio-in-vs-code)
- [platformio in terminal](#building-using-platformio-in-terminal)
- [using binary file from github](#how-to-flash-firmware-once-you-get-binary-file)
- [arduino-cli](#building-using-arduino-cli)
- [VS Code arduino-community-edition (arduino-cli in VS Code)](#building-in-vs-code-using-arduino-community-edition-extension-deprecated)

## Building using PlatformIO in VS Code

Install platfromio extension, select device and port in bottom panel, then press upload through PlatformIO panel, or use command "PlatformIO: Upload" in command pallete, or use Ctrl + Alt + U hotkey

## Building using PlatformIO in terminal

#### Install PlatformIO

```bash
pip install platformio
```

#### Compile

```bash
# M5StickC Plus2
pio run -e m5stick-c-plus2

# Cardputer-ADV
pio run -e cardputer-adv
```

#### Flash firmware

```bash
# M5StickC Plus2
pio run -e m5stick-c-plus2 --target upload --upload-port COM3

# Cardputer
pio run -e cardputer-adv --target upload --upload-port COM4
```

#### Merge binaries

```bash
# M5StickC Plus2 (ESP32)
esptool.py --chip esp32 merge_bin \
  --output firmware_m5stick-c-plus2.bin \
  0x1000 .pio/build/m5stick-c-plus2/bootloader.bin \
  0x8000 .pio/build/m5stick-c-plus2/partitions.bin \
  0xe000 ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin \
  0x10000 .pio/build/m5stick-c-plus2/firmware.bin

# Cardputer (ESP32-S3)
esptool.py --chip esp32s3 merge_bin \
  --output firmware_cardputer-adv.bin \
  0x0000 .pio/build/cardputer-adv/bootloader.bin \
  0x8000 .pio/build/cardputer-adv/partitions.bin \
  0xe000 ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin \
  0x10000 .pio/build/cardputer-adv/firmware.bin
```

#### Flash firmware

```bash
esptool write_flash 0 build/firmware.bin
```

## Building using arduino cli

#### Install libraries

```bash
arduino-cli config set library.enable_unsafe_install true
arduino-cli config add board_manager.additional_urls https://static-cdn.m5stack.com/resource/arduino/package_m5stack_index.json

arduino-cli core update-index
arduino-cli core install m5stack:esp32@2.1.2
arduino-cli lib install M5GFX M5Unified "Adafruit PN532" U8g2 NTPClient
arduino-cli lib install --git-url https://github.com/blackketter/ESP32-BLE-Combo.git
```

#### Compile (This may take a few minutes)

```bash
arduino-cli compile --fqbn m5stack:esp32:m5stack_stickc_plus2 --build-path ./build -e --build-property build.partitions=huge_app --build-property upload.maximum_size=3145728 ./m5stick_crystal_firmware.ino
```

#### Merge binaries

```bash
# M5StickC Plus2 (ESP32)
esptool.py --chip esp32 merge_bin \
  --output firmware_m5stick-c-plus2.bin \
  0x1000 .pio/build/m5stick-c-plus2/bootloader.bin \
  0x8000 .pio/build/m5stick-c-plus2/partitions.bin \
  0xe000 ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin \
  0x10000 .pio/build/m5stick-c-plus2/firmware.bin

# Cardputer (ESP32-S3)
esptool.py --chip esp32s3 merge_bin \
  --output firmware_cardputer-adv.bin \
  0x0000 .pio/build/cardputer-adv/bootloader.bin \
  0x8000 .pio/build/cardputer-adv/partitions.bin \
  0xe000 ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin \
  0x10000 .pio/build/cardputer-adv/firmware.bin
```

#### Flash firmware

```bash
esptool write_flash 0 build/firmware.bin
```

## Building in vs code using arduino-community-edition extension (deprecated)

You can use the vs code extension [arduino community edition](https://marketplace.visualstudio.com/items?itemName=vscode-arduino.vscode-arduino-community) for build and upload firmware in **VS Code** without arduino-ide or arduino-cli directly.

Here is example of configuration files for this extension:

*.vscode/arduino.json*

```json
{
  "sketch": "m5stick_crystal_firmware.ino",
  "port": "/dev/ttyACM0",
  "board": "m5stack:esp32:m5stack_stickc_plus2",
  "output": "./build",
  "programmer": "esptool",
  "configuration": "PSRAM=enabled,PartitionScheme=default_8MB,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=8M,UploadSpeed=1500000,LoopCore=1,EventsCore=1,DebugLevel=none,EraseFlash=none"
}
```

*.vscode/settings.json*

```json
{
    // --------------------------
    // Arduino extension settings
    "arduino.enableUSBDetection": true,
    "arduino.additionalUrls": ["https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json"],
    "arduino.disableIntelliSenseAutoGen": true,
}
```

Open main file `m5stick_crystal_firmware.ino` and press upload button at the top. This will build and flash your firmware.


## How to flash firmware once you get binary file?

#### Install esptool if you don't have it

```bash
pip install esptool
```

#### Flash the binary file

```bash
esptool write_flash 0 firmware.bin
```

Replace firmware.bin with your correct file name.
