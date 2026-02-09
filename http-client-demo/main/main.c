#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_sta.h"
#include "http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define WIFI_SSID "Longdilo"
#define WIFI_PWD "longdilo-pwd"

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_station_init(WIFI_SSID, WIFI_PWD);

    while (wifi_station_is_connected() == true)
    {
        char *quote = http_client_get_random_quote();
        if (quote)
        {
            ESP_LOGI("MAIN", "Quote: %s", quote);
            free(quote);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
