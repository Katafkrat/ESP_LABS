# Lab Work #1: ESP-IDF Environment Setup and GPIO Control

## Project Description
This project is part of the Microcontrollers course. It demonstrates how to set up the development environment for the **ESP32-S3** and perform basic hardware interactions using the **ESP-IDF** framework.

The application implements:
1.  **Console Output:** Sequential numbering from 0 to 9.
2.  **LED Control:** Blinking of the onboard RGB LED (GPIO 48) using the RMT peripheral.
3.  **Interrupt Handling:** Monitoring the BOOT button (GPIO 0) to trigger a specific event (displaying the variant number and changing LED color).


### Program Logic
- **Idle State:** The system loops through digits 0-9 with a 1-second delay. The LED blinks with a low-intensity white light.
- **Button Pressed State:** If GPIO 0 is pulled LOW (button pressed), the console logs the student's variant number (Variant #2) and the LED turns **Red**.

