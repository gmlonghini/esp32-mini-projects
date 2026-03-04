#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_sta.h"
#include "web_server.h"

#define WIFI_SSID "SSID"
#define WIFI_PWD "PWD"

#define LED_GPIO 18
#define LED_COUNT 12

static const char *TAG = "WS2812";

static rgb_color_t current_color = {0};
static led_strip_handle_t led_strip;

void set_color_callback(rgb_color_t color)
{
    current_color = color;

    for (int i = 0; i < LED_COUNT; i++)
    {
        led_strip_set_pixel(led_strip, i, color.r, color.g, color.b);
    }
    led_strip_refresh(led_strip);
}

rgb_color_t get_color_callback(void)
{
    return current_color;
}

static void wheel(uint8_t pos, uint8_t *r, uint8_t *g, uint8_t *b)
{
    pos = 255 - pos;
    if (pos < 85)
    {
        *r = 255 - pos * 3;
        *g = 0;
        *b = pos * 3;
    }
    else if (pos < 170)
    {
        pos -= 85;
        *r = 0;
        *g = pos * 3;
        *b = 255 - pos * 3;
    }
    else
    {
        pos -= 170;
        *r = pos * 3;
        *g = 255 - pos * 3;
        *b = 0;
    }
}

void animation_rainbow_cycle(led_strip_handle_t led_strip)
{
    uint8_t r, g, b;

    for (int j = 0; j < 256; j++)
    {
        for (int i = 0; i < LED_COUNT; i++)
        {

            uint8_t pixel_index = (i * 256 / LED_COUNT + j) & 0xFF;
            wheel(pixel_index, &r, &g, &b);

            led_strip_set_pixel(led_strip, i, r, g, b);
        }

        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_station_init(WIFI_SSID, WIFI_PWD);

    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = LED_COUNT,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags.invert_out = false,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .mem_block_symbols = 64,
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    web_server_start(set_color_callback, get_color_callback);
    // ESP_LOGI(TAG, "Starting blink...");
    animation_rainbow_cycle(led_strip);

    while (1)
    {

        // animation_color_wipe(led_strip);
        // animation_rainbow_cycle(led_strip);

        // for (int i = 0; i < LED_COUNT; i++)
        // {
        //     ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 5, 5, 5)); // index, r, g,b
        //     led_strip_refresh(led_strip);
        //     vTaskDelay(pdMS_TO_TICKS(50));
        // }

        // led_strip_clear(led_strip);
        // led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}