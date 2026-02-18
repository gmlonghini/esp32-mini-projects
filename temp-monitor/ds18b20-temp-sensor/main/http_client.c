#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"

#define SERVER_URL "http://192.168.0.6:5000/data"

static const char *TAG = "temp_post";

esp_err_t http_client_post_temperature(float temperature)
{
    cJSON *root = cJSON_CreateObject();
    if (!root)
    {
        ESP_LOGE(TAG, "Failed to create JSON");
        return ESP_FAIL;
    }

    cJSON_AddNumberToObject(root, "temperature", temperature);

    char *json_string = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if (!json_string)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "POST data: %s", json_string);

    esp_http_client_config_t config = {
        .url = SERVER_URL,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client)
    {
        ESP_LOGE(TAG, "Failed to init client");
        free(json_string);
        return ESP_FAIL;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_set_post_field(client, json_string, strlen(json_string));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "POST Status = %d", status);
    }
    else
    {
        ESP_LOGE(TAG, "POST failed: %s", esp_err_to_name(err));
    }

    // Limpeza
    esp_http_client_cleanup(client);
    free(json_string);

    return err;
}