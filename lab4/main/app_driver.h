/*
 * LED Control Driver Header
 */

#pragma once

#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the LED driver and hardware
 */
void app_driver_init(void);

/**
 * @brief Set the LED state using HSV values
 * 
 * @param hue 0-360
 * @param saturation 0-100
 * @param brightness 0-100
 * @return esp_err_t 
 */
esp_err_t app_light_set_led(uint32_t hue, uint32_t saturation, uint32_t brightness);

/**
 * @brief Set the power state of the LED
 * 
 * @param power true for ON, false for OFF
 * @return esp_err_t 
 */
esp_err_t app_light_set_power(bool power);

/**
 * @brief Set individual RGB components (mapped to internal HSV state)
 * 
 * @param red Maps to Hue (0-360 recommended)
 * @param green Maps to Brightness (0-100)
 * @param blue Maps to Saturation (0-100)
 * @return esp_err_t 
 */
esp_err_t app_light_set_rgb_values(uint16_t red, uint16_t green, uint16_t blue);

#ifdef __cplusplus
}
#endif
