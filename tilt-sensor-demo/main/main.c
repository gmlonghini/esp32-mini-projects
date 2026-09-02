#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED_GPIO GPIO_NUM_2
#define TILT_SENSOR_GPIO GPIO_NUM_5
#define DEBOUNCE_TIME_MS 50

static const char *TAG = "TILT_SENSOR";

void app_main(void)
{
    gpio_config_t led_config = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&led_config);

    gpio_config_t sensor_config = {
        .pin_bit_mask = (1ULL << TILT_SENSOR_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&sensor_config);
    gpio_set_level(LED_GPIO, 0);

    ESP_LOGI(TAG, "Tilt sensor demo started");
    ESP_LOGI(TAG, "Tilt sensor GPIO: %d", TILT_SENSOR_GPIO);
    ESP_LOGI(TAG, "LED GPIO: %d", LED_GPIO);

    int previous_state = gpio_get_level(TILT_SENSOR_GPIO);

    while (1) {
        int current_state = gpio_get_level(TILT_SENSOR_GPIO);

        if (current_state != previous_state) {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));
            current_state = gpio_get_level(TILT_SENSOR_GPIO);

            if (current_state != previous_state) {
                previous_state = current_state;

                if (current_state == 0) {
                    ESP_LOGW(TAG, "TILT DETECTED!");
                    gpio_set_level(LED_GPIO, 1);
                } else {
                    ESP_LOGI(TAG, "Position normal");
                    gpio_set_level(LED_GPIO, 0);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
