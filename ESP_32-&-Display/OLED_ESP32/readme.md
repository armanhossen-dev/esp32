# ESP32 Wi-Fi Real-Time OLED Clock

A lightweight, Wi-Fi-enabled digital clock built using an **ESP32 microcontroller** and a **0.96" SSD1306 I²C OLED Display**.

The device connects to Wi-Fi on boot, fetches accurate local time from an **NTP (Network Time Protocol) server**, and updates the display dynamically in a **12-hour format** alongside the current date.

---

## 🛠️ Hardware Requirements

- ESP32 Development Board (CH340 / CP2102)
- 0.96" OLED Display (SSD1306, 128×64 resolution, I²C)
- Jumper Wires
- Micro-USB to USB Cable

---

## 🔌 Pin Connection Guide

| OLED Display Pin | ESP32 Pin | Function / Wire Note |
|---|---|---|
| GND | GND | Ground |
| VCC | 3V3 | Power (3.3V) |
| SCL | GPIO 22 | Serial Clock Line (I²C) |
| SDA | GPIO 21 | Serial Data Line (I²C) |

---

## 📚 Required Arduino Libraries

Ensure the following libraries are installed in **Arduino IDE**:

**Tools → Manage Libraries...**

- **Adafruit SSD1306** — by Adafruit
- **Adafruit GFX Library** — by Adafruit
- **Adafruit BusIO** — Dependency installed automatically

---

## ⚙️ Configuration Notes

### 🌍 Timezone Offset

The `gmtOffset_sec` value is set to:

```cpp
gmtOffset_sec = 21600;