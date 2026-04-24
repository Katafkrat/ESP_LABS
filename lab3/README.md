# ESP RainMaker: LED Lightbulb Project

This project implements a smart LED lightbulb based on the **ESP32** and the **ESP RainMaker** platform. It supports control via the cloud, mobile application, and physical hardware interfaces.

## 🚀 Key Features
* **Full Color Control:** Manage power (On/Off), brightness, hue, and saturation.
* **Hardware Flexibility:** Support for both standard **RGB LEDs** and addressable **WS2812 (NeoPixel)** strips.
* **Local Control:** Physical button for toggling states and performing resets.
* **Smart Services:** Integrated support for schedules, scenes, timers, and Over-the-Air (OTA) firmware updates.
* **Command-Response:** Handles direct JSON commands for fast integration with external systems.

---

## 🛠 Hardware Configuration (GPIO)

Typical pins can be modified via `menuconfig`. The primary settings include:

| Peripherals | Default Configuration |
|-------------|-----------------------|
| **Button** | `CONFIG_EXAMPLE_BOARD_BUTTON_GPIO` |
| **RGB LED** | GPIOs for Red, Green, and Blue channels |
| **WS2812** | Single-wire Data GPIO |

**Button Functions:**
* **Short Press:** Toggle Light On/Off.
* **3-second Hold:** Reset Wi-Fi settings.
* **10-second Hold:** Factory Reset (complete wipe of settings).