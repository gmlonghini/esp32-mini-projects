#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "cJSON.h"

#define API_URL "https://api.quotable.io/random"

static const char *TAG = "http_client";

char *http_client_get_random_quote(void)
{
    esp_http_client_config_t config = {
        .url = API_URL,
        .timeout_ms = 10000,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .buffer_size = 1024,
        .buffer_size_tx = 512,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client)
    {
        ESP_LOGE(TAG, "Failed to create HTTP client");
        return NULL;
    }

    esp_http_client_set_method(client, HTTP_METHOD_GET);

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Connection failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return NULL;
    }

    int content_length = esp_http_client_fetch_headers(client);
    if (content_length <= 0)
    {
        ESP_LOGE(TAG, "Invalid content length");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return NULL;
    }

    char *buffer = malloc(content_length + 1);
    if (!buffer)
    {
        ESP_LOGE(TAG, "Memory allocation failed");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return NULL;
    }

    int total_read = 0;
    while (total_read < content_length)
    {
        int read = esp_http_client_read(client,
                                        buffer + total_read,
                                        content_length - total_read);

        if (read <= 0)
        {
            ESP_LOGE(TAG, "Read error");
            free(buffer);
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            return NULL;
        }

        total_read += read;
    }

    buffer[total_read] = '\0';

    ESP_LOGI(TAG, "JSON received: %s", buffer);

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    // Parse JSON
    cJSON *root = cJSON_Parse(buffer);
    free(buffer);

    if (!root)
    {
        ESP_LOGE(TAG, "JSON parse error");
        return NULL;
    }

    cJSON *content = cJSON_GetObjectItem(root, "content");

    char *quote = NULL;
    if (cJSON_IsString(content))
    {
        quote = strdup(content->valuestring);
    }

    cJSON_Delete(root);
    return quote;
}
