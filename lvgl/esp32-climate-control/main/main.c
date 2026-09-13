#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_err.h"

#include "lvgl.h"
#include "st7789.h"

#include "demo_lvgl_ambiental.h"
#include "environmentaldashboard_gen.h"

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

#define PIN_MOSI 11
#define PIN_MISO -1
#define PIN_SCLK 12
#define PIN_CS 10
#define PIN_DC 9
#define PIN_RST 14

static SemaphoreHandle_t lvgl_mutex;

static void lvgl_tick_cb(void *arg)
{
    (void)arg;
    lv_tick_inc(1);
}

static void lvgl_flush_cb(lv_display_t *display,
                          const lv_area_t *area,
                          uint8_t *px_map)
{
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;

    uint16_t width = x2 - x1 + 1;
    uint16_t height = y2 - y1 + 1;

    st7789_set_window(x1, y1, x2, y2);
    st7789_push_color(px_map, width * height * 2);

    lv_display_flush_ready(display);
}

static void setup_lvgl(void)
{
    lv_init();

    const esp_timer_create_args_t lvgl_tick = {
        .callback = &lvgl_tick_cb,
        .name = "lvgl_tick",
    };

    esp_timer_handle_t lvgl_timer;

    ESP_ERROR_CHECK(
        esp_timer_create(&lvgl_tick, &lvgl_timer));

    ESP_ERROR_CHECK(
        esp_timer_start_periodic(lvgl_timer, 1000));

    lv_display_t *display =
        lv_display_create(DISPLAY_HEIGHT, DISPLAY_WIDTH);

    static lv_color_t display_buffer[DISPLAY_WIDTH * 32];

    lv_display_set_buffers(
        display,
        display_buffer,
        NULL,
        sizeof(display_buffer),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(display, lvgl_flush_cb);
}

static void setup_ui(void)
{
    demo_lvgl_ambiental_init("");
    lv_obj_t *screen = environmentaldashboard_create();
    lv_screen_load(screen);
}

static void lvgl_task(void *arg)
{
    (void)arg;

    while (true)
    {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);

        lv_timer_handler();

        xSemaphoreGive(lvgl_mutex);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    lvgl_mutex = xSemaphoreCreateMutex();

    st7789_config_t lcd_config = {
        .spi_host = SPI2_HOST,

        .pin_mosi = PIN_MOSI,
        .pin_miso = PIN_MISO,
        .pin_sclk = PIN_SCLK,
        .pin_cs = PIN_CS,
        .pin_dc = PIN_DC,
        .pin_rst = PIN_RST,

        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,

        .colstart = 0,
        .rowstart = 0,

        .spi_clock_hz = 80 * 1000 * 1000,
    };

    st7789_init(&lcd_config);
    st7789_set_rotation(1);
    setup_lvgl();
    setup_ui();
    xTaskCreate(lvgl_task, "lvgl_task", 16384, NULL, 5, NULL);
}
