#include <stdio.h>
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define LDR_ADC_CHANNEL ADC_CHANNEL_6
#define WARNING_GPIO GPIO_NUM_19
#define LDR_THRESHOLD 450

adc_oneshot_unit_handle_t adc_handle;

static void adc_init()
{
    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &adc_handle));

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_10};

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, LDR_ADC_CHANNEL, &channel_config));
}

void app_main(void)
{
    gpio_config_t warning_cfg = {
        .pin_bit_mask = (1ULL << WARNING_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&warning_cfg);
    adc_init();

    int ldr_adc_read = 0;

    while (1)
    {
        adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &ldr_adc_read);
        ESP_LOGI("laser-alarm", "LDR ADC Read = [%d]", ldr_adc_read);

        if (ldr_adc_read < LDR_THRESHOLD)
        {
            gpio_set_level(WARNING_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(150));
            gpio_set_level(WARNING_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(150));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}
