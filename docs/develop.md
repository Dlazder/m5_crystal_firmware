# M5 Crystal Firmware — Developer Guide

## Display

| Parameter | Value |
| - | - |
| Display width | 240 px |
| Display height | 135 px |
| Status bar height | 20 px |

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
