/* Лабораторна робота №1 - Варіант №2
   Виконано для ESP32-S3 без зовнішнього стенда.
   Вивід цифр 0-9 та номера варіанта здійснюється в консоль.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "lab1_variant4";

// Налаштування пінів для вашої плати
#define BLINK_GPIO 48      // Вбудований RGB світлодіод [cite: 191, 207]
#define BUTTON_GPIO 0      // Кнопка BOOT на платі ESP32-S3

static uint8_t s_led_state = 0;
static led_strip_handle_t led_strip;

/**
 * @brief Функція для імітації виводу цифр у консоль (замість матриці)
 */
void display_number_in_console(int number) {
    ESP_LOGI(TAG, "===============================");
    ESP_LOGI(TAG, "      ВІДОБРАЖЕННЯ ЦИФРИ: %d", number);
    ESP_LOGI(TAG, "===============================");
}

/**
 * @brief Ініціалізація периферії: RGB світлодіода та кнопки BOOT
 */
static void configure_peripherals(void)
{
    ESP_LOGI(TAG, "Налаштування периферії...");

    // 1. Налаштування вбудованого адресного світлодіода (LED Strip) [cite: 206]
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, 
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    // 2. Налаштування кнопки BOOT (GPIO 0) як вхід з підтяжкою 
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
   gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    led_strip_clear(led_strip);
}

void app_main(void)
{
    // Початкова конфігурація
    configure_peripherals();
    
    int current_digit = 0;

    while (1) {
        // Перевірка натискання кнопки BOOT (0 - натиснуто)
        if (gpio_get_level(BUTTON_GPIO) == 0) {
            // Завдання: Вивід номера варіанта (4) при натисканні 
            ESP_LOGW(TAG, "КНОПКА НАТИСНУТА! Мій номер у списку:");
            display_number_in_console(2); 
            
            // Підсвічуємо світлодіод червоним при натисканні
            led_strip_set_pixel(led_strip, 0, 255, 0, 0); 
            led_strip_refresh(led_strip);
            
            // Невелика затримка для стабілізації
            vTaskDelay(pdMS_TO_TICKS(500)); 
        } 
        else {
            // Завдання: Циклічний вивід чисел від 0 до 9 
            display_number_in_console(current_digit);
            
            // Логіка звичайного блимання світлодіодом 
            if (s_led_state) {
                // Вмикаємо білий колір низької яскравості
                led_strip_set_pixel(led_strip, 0, 10, 10, 10);
            } else {
                led_strip_clear(led_strip);
            }
            
            led_strip_refresh(led_strip);
            s_led_state = !s_led_state;

            // Перехід до наступної цифри (0-9)
            current_digit = (current_digit + 1) % 10;
        }
        
        // Затримка 1 секунда між ітераціями
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}