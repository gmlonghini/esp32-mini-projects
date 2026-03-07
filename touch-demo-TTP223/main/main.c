#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "led_strip.h"
#include "driver/gpio.h"
#include <math.h>

#define LED_GPIO 18
#define LED_COUNT 12

static led_strip_handle_t led_strip;
static SemaphoreHandle_t led_mutex;

TaskHandle_t animation_task_handle = NULL;

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

void animation_plasma_rainbow(led_strip_handle_t strip)
{
    static float t = 0;

    for (int i = 0; i < LED_COUNT; i++)
    {
        float x = i * 0.3;

        float v = sinf(x + t) +
                  sinf((x * 0.5) + t * 1.3) +
                  sinf((x * 0.8) + t * 0.7);

        v = (v + 3.0) / 6.0; // normaliza para 0..1

        uint8_t r = (sinf(v * 6.28) * 127) + 128;
        uint8_t g = (sinf(v * 6.28 + 2.09) * 127) + 128;
        uint8_t b = (sinf(v * 6.28 + 4.18) * 127) + 128;

        if (xSemaphoreTake(led_mutex, portMAX_DELAY))
        {
            led_strip_set_pixel(strip, i, r, g, b);
            xSemaphoreGive(led_mutex);
        }
    }

    if (xSemaphoreTake(led_mutex, portMAX_DELAY))
    {
        led_strip_refresh(strip);
        xSemaphoreGive(led_mutex);
    }

    t += 0.15; // velocidade da animação
}

void gpio_input_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_4) | (1ULL << GPIO_NUM_15),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&io_conf);
}

void animation_task(void *pvParameters)
{
    while (1)
    {
        animation_plasma_rainbow(led_strip);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void)
{
    gpio_input_init();

    led_mutex = xSemaphoreCreateMutex();

    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = LED_COUNT,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags.invert_out = false,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .mem_block_symbols = 64,
        .flags.with_dma = false,
    };

    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);

    xTaskCreate(animation_task, "animation_task", 4096, NULL, 5, &animation_task_handle);

    static bool last_state_reset = false;

    while (1)
    {
        printf("GPIO4: %d | GPIO15: %d\n",
               gpio_get_level(GPIO_NUM_4),
               gpio_get_level(GPIO_NUM_15));

        if (gpio_get_level(GPIO_NUM_4) == 1 && last_state_reset)
        {
            vTaskResume(animation_task_handle);
            last_state_reset = false;
        }

        if (gpio_get_level(GPIO_NUM_15) == 1 && !last_state_reset)
        {
            last_state_reset = true;

            if (xSemaphoreTake(led_mutex, portMAX_DELAY))
            {
                vTaskSuspend(animation_task_handle);

                for (int i = 0; i < LED_COUNT; i++)
                {
                    led_strip_set_pixel(led_strip, i, 0, 0, 0);
                }

                led_strip_refresh(led_strip);

                xSemaphoreGive(led_mutex);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
