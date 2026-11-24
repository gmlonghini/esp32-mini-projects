#include <stdio.h>

#include "driver/rc522_spi.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "http_client.h"
#include "nvs_flash.h"
#include "rc522.h"
#include "rc522_picc.h"
#include "wifi_sta.h"

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

#define RC522_SPI_BUS_GPIO_MISO 19
#define RC522_SPI_BUS_GPIO_MOSI 23
#define RC522_SPI_BUS_GPIO_SCLK 18
#define RC522_SPI_SCANNER_GPIO_SDA 5
#define RC522_SCANNER_GPIO_RST 22

#define UID_QUEUE_LENGTH 5
#define HTTP_TASK_STACK_SIZE 8192
#define HTTP_TASK_PRIORITY 5

static const char *TAG = "rfid_logger";

static rc522_driver_handle_t driver;
static rc522_handle_t scanner;
static QueueHandle_t uid_queue;

typedef struct
{
    char uid[RC522_PICC_UID_STR_BUFFER_SIZE_MAX];
} rfid_uid_message_t;

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

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
}

static void http_sender_task(void *arg)
{
    rfid_uid_message_t message;

    while (true)
    {
        if (xQueueReceive(uid_queue, &message, portMAX_DELAY) != pdPASS)
        {
            continue;
        }

        while (!wifi_station_is_connected())
        {
            ESP_LOGW(TAG, "Waiting for Wi-Fi to send UID %s", message.uid);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        if (http_client_send_uid(message.uid) != ESP_OK)
        {
            ESP_LOGE(TAG, "Could not send UID %s", message.uid);
        }
    }
}

static void on_card_detected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    rc522_picc_state_changed_event_t *event = data;
    rc522_picc_t *picc = event->picc;

    if (picc->state == RC522_PICC_STATE_ACTIVE)
    {
        rfid_uid_message_t message;

        if (rc522_picc_uid_to_str(&picc->uid, message.uid, sizeof(message.uid)) == ESP_OK)
        {
            ESP_LOGI(TAG, "Card detected!");
            ESP_LOGI(TAG, "UID: %s", message.uid);

            if (xQueueSend(uid_queue, &message, 0) != pdPASS)
            {
                ESP_LOGE(TAG, "UID queue is full; card was not queued for sending");
            }
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing Wi-Fi...");

    initialize_nvs();
    wifi_station_init(WIFI_SSID, WIFI_PASSWORD);

    uid_queue = xQueueCreate(UID_QUEUE_LENGTH, sizeof(rfid_uid_message_t));
    if (uid_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create UID queue");
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }

    if (xTaskCreate(http_sender_task,
                    "http_sender",
                    HTTP_TASK_STACK_SIZE,
                    NULL,
                    HTTP_TASK_PRIORITY,
                    NULL) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create HTTP sender task");
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }

    ESP_LOGI(TAG, "Initializing RFID reader...");

    ESP_ERROR_CHECK(rc522_spi_create(&driver_config, &driver));

    ESP_ERROR_CHECK(rc522_driver_install(driver));

    rc522_config_t scanner_config = {
        .driver = driver,
    };

    ESP_ERROR_CHECK(rc522_create(&scanner_config, &scanner));

    ESP_ERROR_CHECK(
        rc522_register_events(scanner, RC522_EVENT_PICC_STATE_CHANGED, on_card_detected, NULL));

    ESP_ERROR_CHECK(rc522_start(scanner));

    ESP_LOGI(TAG, "Waiting for RFID card...");
}
