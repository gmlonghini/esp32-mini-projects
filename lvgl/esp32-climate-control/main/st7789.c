#include "st7789.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
static uint8_t current_rotation;

static void spi_send(const void *data, size_t len, bool dc)
{
    gpio_set_level(lcd.pin_dc, dc);

    spi_transaction_t transaction = {
        .length = len * 8,
        .tx_buffer = data,
    };
    spi_device_transmit(spi, &transaction);
}

static inline void write_cmd(uint8_t cmd)
{
    spi_send(&cmd, 1, false);
}

static inline void write_data(const void *data, size_t len)
{
    spi_send(data, len, true);
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

    gpio_config_t io = {
        .pin_bit_mask = (1ULL << lcd.pin_dc) |
                        (1ULL << lcd.pin_rst) |
                        (1ULL << lcd.pin_cs),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io);

    spi_bus_config_t bus_config = {
        .mosi_io_num = lcd.pin_mosi,
        .miso_io_num = lcd.pin_miso,
        .sclk_io_num = lcd.pin_sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_device_interface_config_t device_config = {
        .mode = 0,
        .clock_speed_hz = lcd.spi_clock_hz,
        .spics_io_num = lcd.pin_cs,
        .queue_size = 1,
    };

    panel_width = lcd.width;
    panel_height = lcd.height;

    spi_bus_initialize(lcd.spi_host, &bus_config, SPI_DMA_CH_AUTO);
    spi_bus_add_device(lcd.spi_host, &device_config, &spi);

    reset_display();

    write_cmd(ST7789_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));

    write_cmd(ST7789_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));

    uint8_t color_mode = 0x55;
    write_cmd(ST7789_COLMOD);
    write_data(&color_mode, 1);

    write_cmd(ST7789_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
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
    const uint16_t *source = data;
    static uint16_t dma_buffer[1024];
    size_t pixels = len / 2;

    while (pixels)
    {
        size_t batch = pixels > 1024 ? 1024 : pixels;

        for (size_t i = 0; i < batch; i++)
        {
            uint16_t color = source[i];
            dma_buffer[i] = (color >> 8) | (color << 8);
        }

        write_data(dma_buffer, batch * 2);

        source += batch;
        pixels -= batch;
    }
}

void st7789_set_rotation(uint8_t rotation)
{
    current_rotation = rotation % 4;

    uint8_t madctl = 0;

    switch (current_rotation)
    {
    case 0:
        madctl |= MADCTL_MX | MADCTL_MY;
        lcd.width = panel_width;
        lcd.height = panel_height;
        break;

    case 1:
        madctl |= MADCTL_MY | MADCTL_MV;
        lcd.width = panel_height;
        lcd.height = panel_width;
        break;

    case 2:
        lcd.width = panel_width;
        lcd.height = panel_height;
        break;

    case 3:
        madctl |= MADCTL_MX | MADCTL_MV;
        lcd.width = panel_height;
        lcd.height = panel_width;
        break;
    }

    write_cmd(ST7789_MADCTL);
    write_data(&madctl, 1);
}
