#include <stdio.h>
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "st7789.h"

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

#define PIN_MOSI 23
#define PIN_SCLK 18
#define PIN_CS 5
#define PIN_DC 21
#define PIN_RST 22

// static void show_gif()
// {
//     LV_IMAGE_DECLARE(rato_array);
//     lv_obj_t *img;

//     img = lv_gif_create(lv_screen_active());
//     lv_gif_set_color_format(img, LV_COLOR_FORMAT_ARGB8888);
//     lv_gif_set_src(img, &rato_array);

//     // zoom 200%
//     lv_img_set_zoom(img, 512);

//     lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
// }

static void show_gif(void)
{
    LV_IMAGE_DECLARE(rato_array);

    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t *img = lv_gif_create(scr);
    lv_gif_set_color_format(img, LV_COLOR_FORMAT_ARGB8888);
    lv_gif_set_src(img, &rato_array);

    lv_img_set_zoom(img, 512);

    lv_obj_align(img, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "LVGL + ESP32 + IDF\nGIF rodando");

    lv_obj_set_style_text_color(label, lv_color_hex(0x22C55E), LV_PART_MAIN);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 12);
}

static void lvgl_tick_cb(void *arg)
{
    (void)arg;
    lv_tick_inc(1);
}

static void lvgl_flush_cb(lv_display_t *disp,
                          const lv_area_t *area,
                          lv_color_t *px_map)
{
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;

    uint16_t w = x2 - x1 + 1;
    uint16_t h = y2 - y1 + 1;

    st7789_set_window(x1, y1, x2, y2);
    st7789_push_color(px_map, w * h * 2);

    lv_display_flush_ready(disp);
}

void app_main(void)
{
    st7789_config_t lcd_cfg = {
        .spi_host = VSPI_HOST,
        .pin_mosi = PIN_MOSI,
        .pin_sclk = PIN_SCLK,
        .pin_cs = PIN_CS,
        .pin_dc = PIN_DC,
        .pin_rst = PIN_RST,

        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,

        .colstart = 0,
        .rowstart = 0,

        .spi_clock_hz = 20 * 1000 * 1000};

    st7789_init(&lcd_cfg);

    lv_init();

    const esp_timer_create_args_t lvgl_tick = {
        .callback = &lvgl_tick_cb,
        .name = "lvgl_tick"};

    esp_timer_handle_t lvgl_timer;
    esp_timer_create(&lvgl_tick, &lvgl_timer);
    esp_timer_start_periodic(lvgl_timer, 1000);

    lv_display_t *display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    static lv_color_t buf[DISPLAY_WIDTH * 32];
    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, lvgl_flush_cb);

    show_gif();

    while (true)
    {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
