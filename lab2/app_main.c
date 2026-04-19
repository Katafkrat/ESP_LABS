#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "led_strip.h"

static const char *TAG = "mqtt_senyk_lab";

#define RGB_LED_GPIO 48 
#define BUTTON_GPIO  0   // Кнопка BOOT на платі
static led_strip_handle_t led_strip;
static esp_mqtt_client_handle_t mqtt_client;

// 1. Функція для формування та відправки JSON події натиску кнопки
static void send_button_event_json(void)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "event", "button_pressed");
    cJSON_AddStringToObject(root, "user", "Senyk");
    cJSON_AddNumberToObject(root, "uptime", esp_log_timestamp());

    char *post_data = cJSON_PrintUnformatted(root);
    esp_mqtt_client_publish(mqtt_client, "/Senyk/button_event", post_data, 0, 1, 0);
    
    ESP_LOGW(TAG, "Відправлено JSON на брокер: %s", post_data);

    cJSON_Delete(root);
    free(post_data);
}

// Задача для моніторингу кнопки
void button_task(void *pvParameter)
{
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    while(1) {
        if (gpio_get_level(BUTTON_GPIO) == 0) { // Якщо кнопка натиснута
            send_button_event_json();
            vTaskDelay(pdMS_TO_TICKS(500)); // Антибрязк
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void configure_led(void) {
    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_LED_GPIO,
        .max_leds = 1,
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    
    // Початковий колір - жовтий
    led_strip_set_pixel(led_strip, 0, 50, 50, 0);
    led_strip_refresh(led_strip);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    mqtt_client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT ПІДКЛЮЧЕНО (Senyk)");
            // Підписка на топік з прізвищем
            esp_mqtt_client_subscribe(mqtt_client, "/Senyk/led_control", 0);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Отримано дані в топік %.*s", event->topic_len, event->topic);
            
            // 2. Зчитування з брокера стану світлодіодів (RGB формат)
            cJSON *json = cJSON_ParseWithLength(event->data, event->data_len);
            if (json) {
                cJSON *r = cJSON_GetObjectItem(json, "r");
                cJSON *g = cJSON_GetObjectItem(json, "g");
                cJSON *b = cJSON_GetObjectItem(json, "b");

                if (cJSON_IsNumber(r) && cJSON_IsNumber(g) && cJSON_IsNumber(b)) {
                    // Встановлення значень на пристрої
                    led_strip_set_pixel(led_strip, 0, r->valueint, g->valueint, b->valueint);
                    led_strip_refresh(led_strip);
                    ESP_LOGI(TAG, "Колір змінено: R:%d G:%d B:%d", r->valueint, g->valueint, b->valueint);
                }
                cJSON_Delete(json);
            }
            break;
        default:
            break;
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    configure_led();
    ESP_ERROR_CHECK(example_connect());

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    // Запускаємо задачу для кнопки
    xTaskCreate(button_task, "button_task", 2048, NULL, 5, NULL);
}