#include <stdio.h>

#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_random.h"
#include "lib_ssd1680.h"
#include "ssd1680_fonts.h"

#define EPAPER_HOST SPI2_HOST
#define EPAPER_WIDTH 122
#define EPAPER_HEIGHT 250
#define EPAPER_ORIENTATION SSD1680_90_DEG

#define PIN_EPAPER_MOSI GPIO_NUM_23
#define PIN_EPAPER_SCLK GPIO_NUM_18
#define PIN_EPAPER_CS GPIO_NUM_5
#define PIN_EPAPER_BUSY GPIO_NUM_4
#define PIN_EPAPER_RESET GPIO_NUM_16
#define PIN_EPAPER_DC GPIO_NUM_17

static const char *TAG = "epaper_demo";

void app_main(void)
{
    const spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_EPAPER_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_EPAPER_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
        .flags = SPICOMMON_BUSFLAG_MASTER,
    };

    const ssd1680_pinmap_t pinmap = {
        .busy = PIN_EPAPER_BUSY,
        .reset = PIN_EPAPER_RESET,
        .dc = PIN_EPAPER_DC,
        .cs = PIN_EPAPER_CS,
    };

    ESP_LOGI(TAG, "Initializing SPI bus");
    ESP_ERROR_CHECK(spi_bus_initialize(EPAPER_HOST, &bus_config,
                                       SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Initializing SSD1680 in landscape mode (250 x 122)");
    ssd1680_t *display = ssd1680_init(EPAPER_HOST, pinmap,
                                      EPAPER_WIDTH, EPAPER_HEIGHT,
                                      EPAPER_ORIENTATION);
    if (display == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize display");
        ESP_ERROR_CHECK(spi_bus_free(EPAPER_HOST));
        return;
    }

    ssd1680_fill(display, SSD1680_WHITE);

    ssd1680_display_string(display, &font_terminal_14pt,
                           60, 3, "ESP32 E-PAPER", SSD1680_BLACK);
    ssd1680_display_string(display, &font_terminal_9pt_bold,
                           75, 25, "WEACT 2.13 | SSD1680", SSD1680_BLACK);
    ssd1680_draw_line(display, 6, 40, 243, 40, SSD1680_BLACK);
    ssd1680_draw_line(display, 6, 42, 243, 42, SSD1680_BLACK);

    ssd1680_display_string(display, &font_terminal_9pt_bold,
                           8, 47, "PROJECT", SSD1680_BLACK);
    ssd1680_display_string(display, &font_terminal_9pt,
                           8, 58, "EPAPER-DEMO", SSD1680_BLACK);

    ssd1680_display_string(display, &font_terminal_9pt_bold,
                           8, 72, "HARDWARE", SSD1680_BLACK);
    ssd1680_display_string(display, &font_terminal_9pt,
                           8, 83, "ESP32 | SPI", SSD1680_BLACK);

    ssd1680_display_string(display, &font_terminal_9pt_bold,
                           8, 97, "DISPLAY", SSD1680_BLACK);
    ssd1680_display_string(display, &font_terminal_9pt,
                           8, 108, "SSD1680 | 250x122", SSD1680_BLACK);

    char random_text[5];
    unsigned long random_value = (unsigned long)(esp_random() % 10000U);
    snprintf(random_text, sizeof(random_text), "%04lu", random_value);

    ssd1680_draw_line(display, 144, 47, 243, 47, SSD1680_BLACK);
    ssd1680_draw_line(display, 144, 47, 144, 117, SSD1680_BLACK);
    ssd1680_draw_line(display, 243, 47, 243, 117, SSD1680_BLACK);
    ssd1680_draw_line(display, 144, 117, 243, 117, SSD1680_BLACK);
    ssd1680_display_string(display, &font_terminal_9pt_bold,
                           171, 53, "RANDOM ID", SSD1680_BLACK);
    ssd1680_display_string(display, &font_consolas_22pt,
                           160, 71, random_text, SSD1680_BLACK);
    ssd1680_display_string(display, &font_terminal_9pt,
                           166, 104, "BOOT SAMPLE", SSD1680_BLACK);

    ESP_LOGI(TAG, "Random ID for this boot: %s", random_text);

    ESP_LOGI(TAG, "Sending framebuffer and refreshing display");
    ssd1680_send_framebuffer(display);
    ssd1680_refresh(display, FULL_REFRESH);

    ssd1680_deinit(display);
    ESP_ERROR_CHECK(spi_bus_free(EPAPER_HOST));
    ESP_LOGI(TAG, "Done; display is in deep sleep");
}
