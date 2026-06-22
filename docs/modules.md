# Module Connection Guide

- [NFC (PN532)](#nfc-pn532)
- [SD Card](#sd-card)
- [IR reciever](#ir-reciever)

---

## NFC (PN532)

Connect the PN532 module to the GROVE port. Switch the module to **I2C mode**.

<img src="../images/NFC_I2C.png" width="200" alt="PN532 I2C mode"/>

### M5StickC Plus 2

| M5StickCPlus2 GROVE | PN532 |
| - | - |
| G | GND |
| 5V | VCC |
| G32 | SDA |
| G33 | SCL |

<img src="../images/NFC_connection_example.png" alt="PN532 connection example"/>

### Cardputer-ADV

| Cardputer-ADV GROVE | PN532 |
| - | - |
| GND | GND |
| 5V | VCC |
| G2 | SDA |
| G1 | SCL |

## SD Card

| M5StickCPlus2 | SD module |
| - | - |
| GND | GND |
| G36 / G25 | MISO |
| G0 | CLK |
| G26 | MOSI |
| G (grove) | CS |
| 3V3 | 3V3 |

## IR reciever

## M5StickC Plus 2

| M5StickCPlus2 | IR Reciever module |
| - | - |
| GND | GND |
| 5V | VCC |
| G26 | OUT |

## Cardputer-ADV

| Cardputer | IR Reciever module |
| - | - |
| GND | GND |
| 5V OUT | VCC |
| G8(SDA) | OUT |
