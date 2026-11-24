#include <stdio.h>

#include "driver/rc522_spi.h"
#include "esp_log.h"
#include "rc522.h"
#include "rc522_picc.h"

#define RC522_SPI_BUS_GPIO_MISO 19
#define RC522_SPI_BUS_GPIO_MOSI 23
#define RC522_SPI_BUS_GPIO_SCLK 18
#define RC522_SPI_SCANNER_GPIO_SDA 5
#define RC522_SCANNER_GPIO_RST 22

static const char *TAG = "rfid_logger";

static rc522_driver_handle_t driver;
static rc522_handle_t scanner;

static rc522_spi_config_t driver_config = {
    .host_id = SPI3_HOST,

    .bus_config = &(spi_bus_config_t){
        .miso_io_num = RC522_SPI_BUS_GPIO_MISO,
        .mosi_io_num = RC522_SPI_BUS_GPIO_MOSI,
        .sclk_io_num = RC522_SPI_BUS_GPIO_SCLK,
    },

    .dev_config = {
        .spics_io_num = RC522_SPI_SCANNER_GPIO_SDA,
    },

    .rst_io_num = RC522_SCANNER_GPIO_RST,
};

static void on_card_detected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    rc522_picc_state_changed_event_t *event = data;
    rc522_picc_t *picc = event->picc;

    if (picc->state == RC522_PICC_STATE_ACTIVE)
    {
        char uid[RC522_PICC_UID_STR_BUFFER_SIZE_MAX];

        if (rc522_picc_uid_to_str(&picc->uid, uid, sizeof(uid)) == ESP_OK)
        {
            ESP_LOGI(TAG, "Card detected!");
            ESP_LOGI(TAG, "UID: %s", uid);
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing RFID reader...");

    ESP_ERROR_CHECK(rc522_spi_create(&driver_config, &driver));

    ESP_ERROR_CHECK(rc522_driver_install(driver));

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    ESP_ERROR_CHECK(rc522_create(&scanner_config, &scanner));

    ESP_ERROR_CHECK(rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_card_detected, NULL));

    ESP_ERROR_CHECK(rc522_start(scanner));

    ESP_LOGI(TAG, "Waiting for RFID card...");
}
