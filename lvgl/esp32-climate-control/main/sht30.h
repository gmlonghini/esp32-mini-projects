#ifndef SHT30_H
#define SHT30_H

#include "driver/i2c_master.h"
#include "esp_err.h"

typedef struct
{
    i2c_port_num_t i2c_port;
    gpio_num_t sda_io_num;
    gpio_num_t scl_io_num;
    uint8_t device_address;
    uint32_t scl_speed_hz;
} sht30_config_t;

typedef struct
{
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t device;
} sht30_t;

esp_err_t sht30_init(sht30_t *sensor, const sht30_config_t *config);
esp_err_t sht30_read(sht30_t *sensor, float *temperature, float *humidity);
esp_err_t sht30_deinit(sht30_t *sensor);

#endif
