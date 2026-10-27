# Module Connection Guide

- [NFC (PN532)](#nfc-pn532)
- [NFC (RC522)](#nfc-rc522)
- [SD Card](#sd-card)
- [IR reciever](#ir-reciever)

---

## NFC (PN532)

Connect the PN532 module to the GROVE port. Switch the module to **I2C mode**. Select it in **NFC → module**; the I2C pins below are the defaults and can be changed in **NFC → configure pins → PN532**.

<img src="../images/NFC_I2C.png" width="200" alt="PN532 I2C mode"/>

### M5StickC Plus 2

| M5StickCPlus2 (GROVE) | PN532 |
| - | - |
| G | GND |
| 5V | VCC |
| G32 | SDA |
| G33 | SCL |

### M5StickS3

| M5StickS3 (GROVE) | PN532 |
| - | - |
| G | GND |
| 5V | VCC |
| G9 | SDA |
| G10 | SCL |

<img src="../images/NFC_connection_example.png" alt="PN532 connection example"/>

### Cardputer-ADV

| Cardputer-ADV (GROVE) | PN532 |
| - | - |
| GND | GND |
| 5V | VCC |
| G2 | SDA |
| G1 | SCL |

## NFC (RC522)

Connect the RC522 module over SPI. Power it from **3.3V only** (5V can damage the module). The cardputer does not have built-in 3V, use a converter. The IRQ pin is not used and stays unconnected.

Select the module in **NFC → module** (PN532 or RC522); reading UID then works the same way with either. The pins below are the defaults and can be changed in **NFC → configure pins → RC522**. If the module is not found, the screen shows the pins currently in use.

### M5StickC Plus 2

| M5StickCPlus2 | RC522 |
| - | - |
| G32 | SDA (SS) |
| G0 | SCK |
| G26 | MOSI |
| G36 | MISO |
| G33 | RST |
| 3V3 | 3.3V |
| GND | GND |

### M5StickS3

| M5StickS3 | RC522 |
| - | - |
| G8 | SDA (SS) |
| G5 | SCK |
| G6 | MOSI |
| G4 | MISO |
| G3 | RST |
| 3V3 | 3.3V |
| GND | GND |

### Cardputer-ADV

| Cardputer-ADV | RC522 |
| - | - |
| G15 | SDA (SS) |
| G5 | SCK |
| G6 | MOSI |
| G4 | MISO |
| G3 | RST |
| 3V3 | 3.3V |
| GND | GND |

Reading shows the tag type and UID. The tag is saved as a Flipper `.nfc` file (UID, ATQA and SAK only: the RC522 does not dump sectors or pages like the PN532 does). Writing a UID or Mifare Classic sectors works with both modules; the UID can only be changed on magic (CUID/gen2) cards, same as with the PN532.

## SD Card

### M5StickC Plus 2

| M5StickCPlus2 | SD module |
| - | - |
| GND | GND |
| G36 / G25 | MISO |
| G0 | CLK |
| G26 | MOSI |
| G (grove) | CS |
| 3V3 | 3V3 |

### M5StickS3

| M5StickS3 | SD module |
| - | - |
| GND | GND |
| G4 | MISO |
| G5 | CLK |
| G6 | MOSI |
| G7 | CS |
| 3V3 | 3V3 |

## IR reciever

### M5StickC Plus 2

| M5StickCPlus2 | IR Reciever module |
| - | - |
| GND | GND |
| 5V | VCC |
| G26 | OUT |

## Cardputer-ADV

| Cardputer | IR Reciever module |
| - | - |
| GND | GND |
| 5V OUT/IN | VCC |
| G3 | OUT |
