#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver_sht30.h"

#define I2C_PORT I2C_NUM_0
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQUENCY 100000
#define READ_PERIOD_MS 3000

static const char *TAG = "sht30_demo";
static i2c_master_bus_handle_t i2c_bus;
static i2c_master_dev_handle_t sht30_device;

static uint8_t app_i2c_init(void)
{
    const i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };
    const i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SHT30_ADDRESS_1 >> 1,
        .scl_speed_hz = I2C_FREQUENCY,
    };

    esp_err_t err = i2c_new_master_bus(&bus_config, &i2c_bus);
    if (err != ESP_OK)
    {
        return 1;
    }

    err = i2c_master_bus_add_device(i2c_bus, &device_config, &sht30_device);
    if (err != ESP_OK)
    {
        i2c_del_master_bus(i2c_bus);
        i2c_bus = NULL;
        return 1;
    }

    return 0;
}

static uint8_t app_i2c_deinit(void)
{
    esp_err_t device_err = i2c_master_bus_rm_device(sht30_device);
    esp_err_t bus_err = i2c_del_master_bus(i2c_bus);

    sht30_device = NULL;
    i2c_bus = NULL;

    return (device_err == ESP_OK && bus_err == ESP_OK) ? 0 : 1;
}

static uint8_t app_i2c_write_address16(uint8_t addr, uint16_t command,
                                       uint8_t *data, uint16_t length)
{
    if ((addr >> 1) != (SHT30_ADDRESS_1 >> 1))
    {
        return 1;
    }

    uint8_t buffer[2 + length];

    buffer[0] = (uint8_t)(command >> 8);
    buffer[1] = (uint8_t)command;
    for (uint16_t i = 0; i < length; i++)
    {
        buffer[2 + i] = data[i];
    }

    esp_err_t err = i2c_master_transmit(
        sht30_device, buffer, sizeof(buffer), 100);

    return (err == ESP_OK) ? 0 : 1;
}

static uint8_t app_i2c_read_address16(uint8_t addr, uint16_t command,
                                      uint8_t *data, uint16_t length)
{
    if ((addr >> 1) != (SHT30_ADDRESS_1 >> 1))
    {
        return 1;
    }

    const uint8_t command_buffer[2] = {
        (uint8_t)(command >> 8),
        (uint8_t)command,
    };

    esp_err_t err = i2c_master_transmit(
        sht30_device, command_buffer, sizeof(command_buffer), 100);
    if (err != ESP_OK)
    {
        return 1;
    }

    vTaskDelay(pdMS_TO_TICKS(20));

    err = i2c_master_receive(sht30_device, data, length, 100);

    return (err == ESP_OK) ? 0 : 1;
}

static void app_delay_ms(uint32_t milliseconds)
{
    esp_rom_delay_us(milliseconds * 1000U);
}

static void app_debug_print(const char *const format, ...)
{
    char message[256];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    ESP_LOGW(TAG, "%s", message);
}

static void app_receive_callback(uint16_t type)
{
    (void)type;
}

void app_main(void)
{
    sht30_handle_t sensor;

    DRIVER_SHT30_LINK_INIT(&sensor, sht30_handle_t);
    DRIVER_SHT30_LINK_IIC_INIT(&sensor, app_i2c_init);
    DRIVER_SHT30_LINK_IIC_DEINIT(&sensor, app_i2c_deinit);
    DRIVER_SHT30_LINK_IIC_WRITE_ADDRESS16(&sensor, app_i2c_write_address16);
    DRIVER_SHT30_LINK_IIC_READ_ADDRESS16(&sensor, app_i2c_read_address16);
    DRIVER_SHT30_LINK_DELAY_MS(&sensor, app_delay_ms);
    DRIVER_SHT30_LINK_DEBUG_PRINT(&sensor, app_debug_print);
    DRIVER_SHT30_LINK_RECEIVE_CALLBACK(&sensor, app_receive_callback);

    if (sht30_set_addr_pin(&sensor, SHT30_ADDRESS_1) != 0 ||
        sht30_init(&sensor) != 0 ||
        sht30_set_repeatability(&sensor, SHT30_REPEATABILITY_HIGH) != 0)
    {
        ESP_LOGE(TAG, "Failed to initialize the SHT30 sensor");
        return;
    }

    ESP_LOGI(TAG, "SHT30 initialized (SDA=%d, SCL=%d, address=0x45)",
             I2C_SDA_PIN, I2C_SCL_PIN);

    while (true)
    {
        uint16_t temperature_raw;
        uint16_t humidity_raw;
        float temperature;
        float humidity;

        uint8_t result = sht30_single_read(
            &sensor, SHT30_BOOL_FALSE,
            &temperature_raw, &temperature,
            &humidity_raw, &humidity);

        if (result == 0)
        {
            ESP_LOGI(TAG, "Temperature: %.2f C | Humidity: %.2f %%",
                     temperature, humidity);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read the SHT30 (error %u)", result);
        }

        vTaskDelay(pdMS_TO_TICKS(READ_PERIOD_MS));
    }
}
