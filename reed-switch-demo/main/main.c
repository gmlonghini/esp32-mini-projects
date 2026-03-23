#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define REED_GPIO GPIO_NUM_15
#define DEBOUNCE_TIME_MS 50
#define POLL_INTERVAL_MS 10

static const char *TAG = "reed-switch-demo";

void app_main(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << REED_GPIO),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE};

    gpio_config(&io_conf);

    int last_state = 0;

    while (1)
    {
        int current_state = gpio_get_level(REED_GPIO);

        if (current_state == 1 && last_state == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));

            if (gpio_get_level(REED_GPIO) == 1)
            {
                ESP_LOGI(TAG, "Reed switch TRIGGERED!");
            }
        }

        last_state = current_state;

        vTaskDelay(pdMS_TO_TICKS(POLL_INTERVAL_MS));
    }
}