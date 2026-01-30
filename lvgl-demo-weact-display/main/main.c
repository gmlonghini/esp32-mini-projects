#include <stdio.h>
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

#define PIN_MOSI 23
#define PIN_SCLK 18
#define PIN_CS 5
#define PIN_DC 21
#define PIN_RST 22

static const char *TAG = "lvgl-demo";
static esp_lcd_panel_handle_t panel_handle = NULL;

static void lvgl_tick_cb(void *arg)
{
    (void)arg;
    lv_tick_inc(1);
}

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    int x1 = area->x1;
    int y1 = area->y1;
    int x2 = area->x2 + 1;
    int y2 = area->y2 + 1;

    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2, y2, px_map);

    lv_display_flush_ready(disp);
}

void app_main(void)
{
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_SCLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = DISPLAY_HEIGHT * DISPLAY_WIDTH * 2};

    ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_spi_cfg = {
        .dc_gpio_num = PIN_DC,
        .cs_gpio_num = PIN_CS,
        .pclk_hz = 20 * 1000 * 1000,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)VSPI_HOST, &io_spi_cfg, &io_handle));

    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = PIN_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_cfg, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    lv_init();

    const esp_timer_create_args_t lvgl_tick = {
        .callback = &lvgl_tick_cb,
        .name = "lvgl_tick"};

    const esp_timer_handle_t lvgl_timer;
    esp_timer_create(&lvgl_tick, &lvgl_timer);
    esp_timer_start_periodic(lvgl_timer, 1000);

    lv_display_t *display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    static lv_color_t buf[DISPLAY_WIDTH * 40];
    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, lvgl_flush_cb);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello World!");

    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    while (true)
    {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
