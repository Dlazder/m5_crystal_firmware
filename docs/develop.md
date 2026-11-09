# M5 Crystal Firmware — Developer Guide

This file is intended to help developers understand how the project works, but unfortunately it is not complete and not the most up-to-date.

## Simple and extensible

Unlike many projects, the global structure is intuitive and fundamentally very simple. Adding your own logic or modifying existing one is quite simple, and I tried to document it.

<img src="../images/architecture.png" width="500" alt="Firmware architecture"/>

You can easily add a new function using the [documentation](#documentation) and by looking on already created funcs in `functions/` folder.

## Process managment

In the main loop, two processes are continuously running: the status bar process (if it is not turned off) and the running function process.
Each process independently processes button clicks and draws the interface, be it a menu or a unique UI. Each process except the status bar process has its own unique process ID (PID).

The global variable `process` stores the current PID. Based on it, `switcher` is determined which process function to run.
As soon as the value of this variable changes, the process will be changed. However, you should use functions from `utils/procManagmentUtils.h`. They reset various global variables, ensuring stable operation.

Check out the full list of utilities in the [documentation](./utils.md)

## Configuration & Settings

The configuration file is stored in the file `system/conf.h`
The file stores the default firmware settings.
Variable values ​​are overwritten by saved settings in the device.

Preferences are loaded using the `loadPreferences` function from the `system/loadPreferences.h` file. This function replaces the values ​​of variables from globals.h and conf.h with those stored in the device memory. If there is no value saved, the default one from conf.h/globals.h is used instead.

```cpp
volume = getData("volume", volume);
Serial.printf("Volume: %d\n", volume / volumeDividor);
DEVICE.Speaker.setVolume(volume);
```

## Display

| Parameter | Value |
| - | - |
| Display width | 240 px |
| Display height | 135 px |
| Status bar height | 20 px |
| Scroll bar width | 5px |

## Fonts and text sizes

| Variable | Base multiplier |
| - | - |
| `TINY_TEXT` | 1x |
| `SMALL_TEXT` | 1.5x |
| `MEDIUM_TEXT` | 2x |
| `BIG_TEXT` | 3x |
| `HUGE_TEXT` | 4x |
| `MENU_TEXT` | 1.8x (for menu items) |

Text sizes are multiplied by a per-font scale factor from `fontScales[]` to normalize line height across fonts.

To switch fonts call `applyFont(index)` — it sets the font on all canvases and recalculates the text size variables. Everything is declared in `src/system/globals.h`.

## Colors

Throughout the firmware, two primary colors are used, `BGCOLOR` and `FGCOLOR`, sometimes an additional `TFT_DARKGREY`.
The colors array is declared in `src/system/globals.h`.

FGCOLOR can be changed in the firmware settings `settings -> colors`.

These colors, already defined in M5GFX, were chosen as the main ones.

| Color | Color name |
| - | - |
| TFT_WHITE | WHITE |
| TFT_RED | RED |
| TFT_ORANGE | ORANGE |
| TFT_YELLOW | YELLOW |
| TFT_GREEN | GREEN |
| TFT_CYAN | CYAN |
| TFT_BLUE | BLUE |
| TFT_VIOLET | VIOLET |
| TFT_MAGENTA | MAGENTA |

In the future, it is possible to add a third color in the form of a darker version of the colors described above, or the ability to select a color from the full RGB spectrum.

## Dimming

Screen dimming allows you to save battery power by dimming the screen when the device is not active.
Dimming is adjustable between the values: 30s, 1m, 3m, 5m, disabled.
By default, dimming is enabled at 30s.

In the main loop, the dimmerUpdate function is called at each iteration. It checks the timer and automatically resets it when the buttons are pressed.
You can also wake up the screen manually by calling the `dimmerWake` function.
This is used in flashLightLoop, flashLightFlickerLoop and levelToolLoop.