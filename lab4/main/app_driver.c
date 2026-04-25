/*  LED Lightbulb demo implementation using RGB LED

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sdkconfig.h>
#include <esp_log.h>
#include <iot_button.h>
#include <button_gpio.h>

#include "app_driver.h"

#include <app_reset.h>

#include <led_indicator.h>
#include <led_convert.h>
#include "led_indicator_rgb.h"
#include "led_indicator_strips.h"

/* This is the button that is used for toggling the power */
#define BUTTON_GPIO          GPIO_NUM_2
#define BUTTON_ACTIVE_LEVEL  0

#define WIFI_RESET_BUTTON_TIMEOUT       3
#define FACTORY_RESET_BUTTON_TIMEOUT    10
#define CONFIG_WS2812_LED_GPIO      48
#define CONFIG_WS2812_LED_COUNT     1

static uint16_t g_red = 0;
static uint16_t g_blue = 0;
static uint16_t g_green = 0;
static bool g_power = true;

/* LED Indicator handle */
static led_indicator_handle_t g_led_indicator = NULL;

esp_err_t app_light_set_led(uint32_t red, uint32_t blue, uint32_t green)
{
    if (!g_power) {
        g_power = true;
    }

    if (!g_led_indicator) {
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t red_359 = red % 360;
    uint16_t blue_255 = (blue * 255) / 100;
    uint16_t green_255 = (green * 255) / 100;
    return led_indicator_set_hsv(g_led_indicator, SET_IRGB(MAX_INDEX, red_359, blue_255, green_255));
}

esp_err_t app_light_set_power(bool power)
{
    g_power = power;

    if (!g_led_indicator) {
        return ESP_ERR_INVALID_STATE;
    }

    if (power) {
        return led_indicator_set_rgb(g_led_indicator, SET_IRGB(MAX_INDEX, g_red, g_green, g_blue));
    } else {
        return led_indicator_set_rgb(g_led_indicator, SET_IRGB(MAX_INDEX, 0, 0, 0));
    }
}

esp_err_t app_light_init(void)
{
        /* Use LED Strip (WS2812) */
    led_indicator_strips_config_t strips_config = {
        .led_strip_cfg = {
            .strip_gpio_num = CONFIG_WS2812_LED_GPIO,
            .max_leds = CONFIG_WS2812_LED_COUNT,
            .led_model = LED_MODEL_WS2812,
            .flags.invert_out = false,
        },
        .led_strip_driver = LED_STRIP_RMT,
        .led_strip_rmt_cfg = {
            .clk_src = RMT_CLK_SRC_DEFAULT,
            .resolution_hz = 10 * 1000 * 1000, /* 10 MHz */
            .flags.with_dma = false,
        },
    };

    led_indicator_config_t config = {
        .blink_lists = NULL,
        .blink_list_num = 0,
    };

    esp_err_t err = led_indicator_new_strips_device(&config, &strips_config, &g_led_indicator);

#ifdef CONFIG_LED_TYPE_NONE
    /* No LED hardware - just return success */
    return ESP_OK;
#endif

    if (g_power) {
        /* Use RGB values directly */
        return led_indicator_set_rgb(g_led_indicator, SET_IRGB(MAX_INDEX, g_red, g_green, g_blue));
    } else {
        return led_indicator_set_rgb(g_led_indicator, SET_IRGB(MAX_INDEX, 0, 0, 0));
    }
}

esp_err_t app_light_set_rgb_values(uint16_t red, uint16_t green, uint16_t blue)
{
    g_red = red;
    g_green = green;
    g_blue = blue;

    if (!g_led_indicator) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!g_power) {
        g_power = true;
    } 

    return led_indicator_set_rgb(g_led_indicator, SET_IRGB(MAX_INDEX, g_red, g_green, g_blue));
}

esp_err_t app_light_set_brightness(uint16_t brightness)
{
    g_green = brightness;
    return app_light_set_led(g_red, g_blue, g_green);
}
esp_err_t app_light_set_hue(uint16_t hue)
{
    g_red = hue;
    return app_light_set_led(g_red, g_blue, g_green);
}
esp_err_t app_light_set_saturation(uint16_t saturation)
{
    g_blue = saturation;
    return app_light_set_led(g_red, g_blue, g_green);
}

static void push_btn_cb(void *arg, void *data)
{
    app_light_set_power(!g_power);
}

void app_driver_init()
{
    app_light_init();
    button_config_t btn_cfg = {
        .long_press_time = 0,  /* Use default */
        .short_press_time = 0, /* Use default */
    };
    button_gpio_config_t gpio_cfg = {
        .gpio_num = BUTTON_GPIO,
        .active_level = BUTTON_ACTIVE_LEVEL,
        .enable_power_save = false,
    };
    button_handle_t btn_handle = NULL;
    if (iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &btn_handle) == ESP_OK && btn_handle) {
        /* Register a callback for a button single click event */
        iot_button_register_cb(btn_handle, BUTTON_SINGLE_CLICK, NULL, push_btn_cb, NULL);
        /* Register Wi-Fi reset and factory reset functionality on same button */
        app_reset_button_register(btn_handle, WIFI_RESET_BUTTON_TIMEOUT, FACTORY_RESET_BUTTON_TIMEOUT);
    }
}
