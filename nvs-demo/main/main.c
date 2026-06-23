#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#define BUTTON_GPIO GPIO_NUM_5

#define BUTTON_PRESSED 1
#define BUTTON_NOT_PRESSED 0

static const char *TAG = "main";
static const char *nvs_key = "counter-key";

void app_main(void)
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(TAG, "Erasing NVS flash due to no free pages or new version found");
        ESP_ERROR_CHECK(nvs_flash_erase());

        err = nvs_flash_init();
    }

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(err));
        return;
    }

    nvs_handle_t handle;

    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return;
    }

    uint32_t counter = 0;

    err = nvs_get_u32(handle, nvs_key, &counter);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        counter = 0;
        ESP_LOGW(TAG, "Key 'counter' not found in NVS, initializing to 0");
    }
    else if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error reading 'counter' from NVS: %s", esp_err_to_name(err));
        counter = 0;
    }

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };

    gpio_config(&io_conf);

    ESP_LOGI(TAG, "ESP32 Started");
    ESP_LOGI(TAG, "Counter = %lu", (unsigned long)counter);

    int last_state = BUTTON_NOT_PRESSED;

    while (1)
    {
        int current_state = gpio_get_level(BUTTON_GPIO);

        if ((last_state == BUTTON_NOT_PRESSED) &&
            (current_state == BUTTON_PRESSED))
        {
            counter++;

            err = nvs_set_u32(handle, nvs_key, counter);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Error writing 'counter' to NVS: %s", esp_err_to_name(err));
            }

            err = nvs_commit(handle);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Error committing NVS changes: %s", esp_err_to_name(err));
            }

            ESP_LOGI(TAG, "Counter = %lu", (unsigned long)counter);

            vTaskDelay(pdMS_TO_TICKS(200));
        }

        last_state = current_state;

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    nvs_close(handle);
}