# ESP32-S3 MQTT Control Project (Senyk Lab)

This project demonstrates **MQTT** protocol implementation using the **ESP32-S3** microcontroller. The application allows controlling an addressable RGB LED via JSON messages and publishing telemetry data to a broker when a physical button is pressed.

---

## 🛠 Hardware Specifications
* **MCU:** ESP32-S3
* **RGB LED:** GPIO 48 (Built-in WS2812)
* **Button:** GPIO 0 (BOOT button)

## 📡 MQTT Configuration
Use the following topics to interact with the device:

### 1. LED Control (Subscriber)
The device is subscribed to: `/Senyk/led_control`  
Send a JSON payload to change the color:
```json
{
  "r": 255, 
  "g": 0, 
  "b": 0
}
```
### 2. Button Events (Publisher)
When the button is pressed, the device publishes data to: `/Senyk/button_event`  
Example payload:
```json
{
  "event": "button_pressed",
  "user": "Senyk",
  "uptime": 45230
}
```