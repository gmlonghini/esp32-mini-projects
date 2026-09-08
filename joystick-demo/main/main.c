#include <stdbool.h>
#include <stdint.h>

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define JOYSTICK_VX_CHANNEL ADC_CHANNEL_6 // GPIO34
#define JOYSTICK_VY_CHANNEL ADC_CHANNEL_7 // GPIO35
#define NUM_SAMPLES 32

static const char *TAG = "joystick-demo";

static int read_average(adc_oneshot_unit_handle_t adc, adc_channel_t channel)
{
    uint32_t sum = 0;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        int value;
        ESP_ERROR_CHECK(adc_oneshot_read(adc, channel, &value));
        sum += value;
    }

    return sum / NUM_SAMPLES;
}

void app_main(void)
{
    adc_oneshot_unit_handle_t adc;
    adc_oneshot_unit_init_cfg_t adc_init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_chan_cfg_t adc_channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_init_config, &adc));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, JOYSTICK_VX_CHANNEL,
                                               &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, JOYSTICK_VY_CHANNEL,
                                               &adc_channel_config));

    while (true)
    {
        int vx = read_average(adc, JOYSTICK_VX_CHANNEL);
        int vy = read_average(adc, JOYSTICK_VY_CHANNEL);

        ESP_LOGI(TAG, "VX: %d | VY: %d", vx, vy);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
