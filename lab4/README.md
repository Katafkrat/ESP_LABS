# Wi-Fi SoftAP & Web Server (ESP32-S3)

This project implements a Wi-Fi Access Point (SoftAP) functionality using the ESP-IDF framework. It allows the ESP32-S3 to act as a local server, enabling other devices (smartphones, laptops) to connect and interact with it via a web interface.

## 📋 Project Overview
- **Hardware:** ESP32-S3
- **Framework:** ESP-IDF v5.x
- **Network Mode:** SoftAP (Access Point)
- **File System:** SPIFFS (used for storing HTML, JS, CSS, and assets)
- **Security:** WPA2 support with Protected Management Frames (PMF)

---

## 🛠 Configuration & Build Fixes

### 1. Fixing the "Object Name Too Long" Error (SPIFFS)
Modern frontend builds (like Vue, React, or Vite) often generate long filenames containing hashes (e.g., `chunk-vendors.a152bbaa.js.map`). By default, SPIFFS limits filenames to 32 characters, which causes a `RuntimeError: object name too long` during the build process.

**Solution:**
1. Open the configuration menu: `idf.py menuconfig`.
2. Navigate to: **Component config** → **SPIFFS**.
3. Locate the setting: **SPIFFS_OBJ_NAME_LEN**.
4. Increase the value from `32` to **64** (or higher if needed).
5. Save (`S`) and Exit (`Esc`).

### 2. Wi-Fi Configuration
In the same `menuconfig` menu, go to:
- **Example Configuration**:
  - Set **WiFi SSID** (The name of your network).
  - Set **WiFi Password** (Minimum 8 characters).

---

## 🚀 Build and Flash

To ensure all SPIFFS settings and configuration changes are applied, it is recommended to perform a clean build:

```bash
# Clean the project cache
idf.py fullclean

# Build, Flash, and Monitor
# Replace PORT with your actual port (e.g., COM3 or /dev/ttyUSB0)
idf.py -p PORT flash monitor