#include "st7789.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define ST7789_SWRESET 0x01
#define ST7789_SLPOUT 0x11
#define ST7789_COLMOD 0x3A
#define ST7789_MADCTL 0x36
#define ST7789_CASET 0x2A
#define ST7789_RASET 0x2B
#define ST7789_RAMWR 0x2C
#define ST7789_DISPON 0x29

#define MADCTL_MX 0x40
#define MADCTL_MY 0x80
#define MADCTL_MV 0x20

static st7789_config_t lcd;
static spi_device_handle_t spi;

static uint16_t panel_width;
static uint16_t panel_height;

static void spi_send(const void *data, size_t len, bool dc)
{
    gpio_set_level(lcd.pin_dc, dc);

    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data,
    };

    spi_device_transmit(spi, &t);
}

static inline void write_cmd(uint8_t cmd)
{
    spi_send(&cmd, 1, 0);
}

static inline void write_data(const void *data, size_t len)
{
    spi_send(data, len, 1);
}

static void reset_display(void)
{
    gpio_set_level(lcd.pin_rst, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(lcd.pin_rst, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
}

void st7789_init(const st7789_config_t *cfg)
{
    memcpy(&lcd, cfg, sizeof(st7789_config_t));

    panel_width = lcd.width;
    panel_height = lcd.height;

    gpio_config_t io = {
        .pin_bit_mask = (1ULL << lcd.pin_dc) |
                        (1ULL << lcd.pin_rst) |
                        (1ULL << lcd.pin_cs),
        .mode = GPIO_MODE_OUTPUT};
    gpio_config(&io);

    spi_bus_config_t buscfg = {
        .mosi_io_num = lcd.pin_mosi,
        .miso_io_num = -1,
        .sclk_io_num = lcd.pin_sclk,
    };

    spi_device_interface_config_t devcfg = {
        .mode = 0,
        .clock_speed_hz = lcd.spi_clock_hz,
        .spics_io_num = lcd.pin_cs,
        .queue_size = 1,
    };

    spi_bus_initialize(lcd.spi_host, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(lcd.spi_host, &devcfg, &spi);

    reset_display();

    write_cmd(ST7789_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));

    write_cmd(ST7789_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));

    uint8_t madctl = 0x00;
    write_cmd(ST7789_MADCTL);
    write_data(&madctl, 1);

    uint8_t colmod = 0x55;
    write_cmd(ST7789_COLMOD);
    write_data(&colmod, 1);

    write_cmd(ST7789_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void st7789_set_rotation(uint8_t rotation)
{
    rotation %= 4;

    uint8_t madctl = 0x00;

    lcd.colstart = 0;
    lcd.rowstart = 0;

    switch (rotation)
    {
    case 0:
        madctl = 0x00;
        lcd.width = panel_width;
        lcd.height = panel_height;
        break;

    case 1:
        madctl = MADCTL_MV;
        lcd.width = panel_height;
        lcd.height = panel_width;
        break;

    case 2:
        madctl = MADCTL_MX | MADCTL_MY;
        lcd.width = panel_width;
        lcd.height = panel_height;
        break;

    case 3:
        madctl = MADCTL_MV | MADCTL_MX | MADCTL_MY;
        lcd.width = panel_height;
        lcd.height = panel_width;
        break;
    }

    write_cmd(ST7789_MADCTL);
    write_data(&madctl, 1);
}

void st7789_set_window(uint16_t x0, uint16_t y0,
                       uint16_t x1, uint16_t y1)
{
    x0 += lcd.colstart;
    x1 += lcd.colstart;
    y0 += lcd.rowstart;
    y1 += lcd.rowstart;

    uint8_t data[4];

    write_cmd(ST7789_CASET);
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;
    write_data(data, 4);

    write_cmd(ST7789_RASET);
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;
    write_data(data, 4);

    write_cmd(ST7789_RAMWR);
}

void st7789_push_color(const void *data, size_t len)
{
    const uint16_t *src = (const uint16_t *)data;

    static uint16_t dma_buf[1024];

    size_t pixels = len / 2;

    while (pixels)
    {
        size_t batch = (pixels > 1024) ? 1024 : pixels;

        for (size_t i = 0; i < batch; i++)
        {
            dma_buf[i] = __builtin_bswap16(src[i]);
        }

        write_data(dma_buf, batch * 2);

        src += batch;
        pixels -= batch;
    }
}

void st7789_fill_rect(uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h,
                      uint16_t color)
{
    if (x >= lcd.width || y >= lcd.height)
        return;

    if (x + w > lcd.width)
        w = lcd.width - x;
    if (y + h > lcd.height)
        h = lcd.height - y;

    st7789_set_window(x, y, x + w - 1, y + h - 1);

    uint16_t c = __builtin_bswap16(color);
    uint8_t pixel[2] = {c >> 8, c & 0xFF};

    for (uint32_t i = 0; i < w * h; i++)
        write_data(pixel, 2);
}