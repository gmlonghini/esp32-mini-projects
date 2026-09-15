#include <stddef.h>
#include <string.h>

#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sht30.h"

#define SHT30_COMMAND_SOFT_RESET 0x30A2
#define SHT30_COMMAND_MEASURE_HIGH 0x2400
#define SHT30_TRANSACTION_TIMEOUT_MS 100
#define SHT30_MEASUREMENT_TIME_MS 20

static uint8_t sht30_crc(const uint8_t *data, size_t length)
{
    uint8_t crc = 0xFF;

    for (size_t byte = 0; byte < length; byte++)
    {
        crc ^= data[byte];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
        }
    }

    return crc;
}

static esp_err_t sht30_send_command(sht30_t *sensor, uint16_t command)
{
    const uint8_t command_buffer[] = {
        (uint8_t)(command >> 8),
        (uint8_t)command,
    };

    return i2c_master_transmit(
        sensor->device,
        command_buffer,
        sizeof(command_buffer),
        SHT30_TRANSACTION_TIMEOUT_MS);
}

esp_err_t sht30_init(sht30_t *sensor, const sht30_config_t *config)
{
    if (sensor == NULL || config == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    memset(sensor, 0, sizeof(*sensor));

    const i2c_master_bus_config_t bus_config = {
        .i2c_port = config->i2c_port,
        .sda_io_num = config->sda_io_num,
        .scl_io_num = config->scl_io_num,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };

    esp_err_t err = i2c_new_master_bus(&bus_config, &sensor->bus);
    if (err != ESP_OK)
    {
        return err;
    }

    const i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = config->device_address,
        .scl_speed_hz = config->scl_speed_hz,
    };

    err = i2c_master_bus_add_device(sensor->bus, &device_config, &sensor->device);
    if (err != ESP_OK)
    {
        i2c_del_master_bus(sensor->bus);
        sensor->bus = NULL;
        return err;
    }

    err = sht30_send_command(sensor, SHT30_COMMAND_SOFT_RESET);
    if (err != ESP_OK)
    {
        sht30_deinit(sensor);
        return err;
    }

    esp_rom_delay_us(2000);
    return ESP_OK;
}

esp_err_t sht30_read(sht30_t *sensor, float *temperature, float *humidity)
{
    if (sensor == NULL || sensor->device == NULL || temperature == NULL || humidity == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = sht30_send_command(sensor, SHT30_COMMAND_MEASURE_HIGH);
    if (err != ESP_OK)
    {
        return err;
    }

    vTaskDelay(pdMS_TO_TICKS(SHT30_MEASUREMENT_TIME_MS));

    uint8_t data[6];
    err = i2c_master_receive(
        sensor->device,
        data,
        sizeof(data),
        SHT30_TRANSACTION_TIMEOUT_MS);
    if (err != ESP_OK)
    {
        return err;
    }

    if (sht30_crc(data, 2) != data[2] || sht30_crc(&data[3], 2) != data[5])
    {
        return ESP_ERR_INVALID_CRC;
    }

    const uint16_t temperature_raw = (uint16_t)((data[0] << 8) | data[1]);
    const uint16_t humidity_raw = (uint16_t)((data[3] << 8) | data[4]);

    *temperature = ((float)temperature_raw / 65535.0f) * 175.0f - 45.0f;
    *humidity = ((float)humidity_raw / 65535.0f) * 100.0f;

    return ESP_OK;
}

esp_err_t sht30_deinit(sht30_t *sensor)
{
    if (sensor == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = ESP_OK;

    if (sensor->device != NULL)
    {
        result = i2c_master_bus_rm_device(sensor->device);
        sensor->device = NULL;
    }

    if (sensor->bus != NULL)
    {
        const esp_err_t err = i2c_del_master_bus(sensor->bus);
        if (result == ESP_OK)
        {
            result = err;
        }
        sensor->bus = NULL;
    }

    return result;
}
