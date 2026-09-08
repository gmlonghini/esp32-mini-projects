#include <stdbool.h>
#include <stdint.h>

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define JOYSTICK_VX_CHANNEL ADC_CHANNEL_6 // GPIO34
#define JOYSTICK_VY_CHANNEL ADC_CHANNEL_7 // GPIO35
#define NUM_SAMPLES 32
#define ADC_MAX_VALUE 4095

#define RGB_RED_GPIO 32
#define RGB_GREEN_GPIO 33
#define RGB_BLUE_GPIO 25
#define RGB_CHANNEL_COUNT 3
#define RGB_PWM_FREQUENCY_HZ 5000
#define RGB_COMPONENT_MAX UINT8_MAX

#define HUE_MAX_DEGREES 359
#define HUE_SECTOR_DEGREES 60
#define PERCENT_MAX 100

#define CONTROL_PERIOD_MS 20

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

static void rgb_init(void)
{
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = RGB_PWM_FREQUENCY_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    const int gpios[] = {RGB_RED_GPIO, RGB_GREEN_GPIO, RGB_BLUE_GPIO};
    const ledc_channel_t channels[] = {
        LEDC_CHANNEL_0,
        LEDC_CHANNEL_1,
        LEDC_CHANNEL_2,
    };

    for (int i = 0; i < RGB_CHANNEL_COUNT; i++)
    {
        ledc_channel_config_t channel_config = {
            .gpio_num = gpios[i],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = channels[i],
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0,
        };
        ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
    }
}

static void rgb_set(uint8_t red, uint8_t green, uint8_t blue)
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
                                  red));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1,
                                  green));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2,
                                  blue));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));
}

static void hsv_to_rgb(uint16_t hue, uint8_t brightness,
                       uint8_t *red_output, uint8_t *green_output,
                       uint8_t *blue_output)
{
    uint8_t hue_sector = hue / HUE_SECTOR_DEGREES;
    uint8_t position_in_sector =
        (hue % HUE_SECTOR_DEGREES) * RGB_COMPONENT_MAX /
        HUE_SECTOR_DEGREES;
    uint8_t decreasing_component = RGB_COMPONENT_MAX - position_in_sector;
    uint8_t increasing_component = position_in_sector;
    uint8_t base_red;
    uint8_t base_green;
    uint8_t base_blue;

    switch (hue_sector)
    {
    case 0:
        base_red = RGB_COMPONENT_MAX;
        base_green = increasing_component;
        base_blue = 0;
        break;
    case 1:
        base_red = decreasing_component;
        base_green = RGB_COMPONENT_MAX;
        base_blue = 0;
        break;
    case 2:
        base_red = 0;
        base_green = RGB_COMPONENT_MAX;
        base_blue = increasing_component;
        break;
    case 3:
        base_red = 0;
        base_green = decreasing_component;
        base_blue = RGB_COMPONENT_MAX;
        break;
    case 4:
        base_red = increasing_component;
        base_green = 0;
        base_blue = RGB_COMPONENT_MAX;
        break;
    default:
        base_red = RGB_COMPONENT_MAX;
        base_green = 0;
        base_blue = decreasing_component;
        break;
    }

    *red_output = (uint16_t)base_red * brightness / RGB_COMPONENT_MAX;
    *green_output = (uint16_t)base_green * brightness / RGB_COMPONENT_MAX;
    *blue_output = (uint16_t)base_blue * brightness / RGB_COMPONENT_MAX;
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

    rgb_init();

    while (true)
    {
        int vx = read_average(adc, JOYSTICK_VX_CHANNEL);
        int vy = read_average(adc, JOYSTICK_VY_CHANNEL);
        uint16_t hue = (uint32_t)vx * HUE_MAX_DEGREES / ADC_MAX_VALUE;
        uint8_t brightness =
            (uint32_t)vy * RGB_COMPONENT_MAX / ADC_MAX_VALUE;
        uint8_t red;
        uint8_t green;
        uint8_t blue;

        hsv_to_rgb(hue, brightness, &red, &green, &blue);
        rgb_set(red, green, blue);

        ESP_LOGI(TAG, "VX: %d | VY: %d | cor: %u graus | brilho: %u%%",
                 vx, vy, hue,
                 (unsigned)brightness * PERCENT_MAX / RGB_COMPONENT_MAX);

        vTaskDelay(pdMS_TO_TICKS(CONTROL_PERIOD_MS));
    }
}
