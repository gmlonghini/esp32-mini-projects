#include "http_client.h"

#include <string.h>

#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"

#define API_URL "https://script.google.com/macros/"
#define API_TOKEN "minha-chave-secreta"
#define DEVICE_ID "ESP32-01"
#define HTTP_RESPONSE_BUFFER_SIZE 32

static const char *TAG = "http_client";

typedef struct
{
    char body[HTTP_RESPONSE_BUFFER_SIZE];
    size_t length;
    bool overflow;
} http_response_t;

static esp_err_t http_event_handler(esp_http_client_event_t *event)
{
    http_response_t *response = event->user_data;

    if (event->event_id == HTTP_EVENT_REDIRECT)
    {
        ESP_LOGD(TAG, "Following redirect as GET");

        esp_err_t err = esp_http_client_set_method(event->client, HTTP_METHOD_GET);
        if (err != ESP_OK)
        {
            return err;
        }

        err = esp_http_client_set_post_field(event->client, NULL, 0);
        if (err != ESP_OK)
        {
            return err;
        }

        return esp_http_client_set_redirection(event->client);
    }

    if (event->event_id == HTTP_EVENT_ON_DATA)
    {
        int status_code = esp_http_client_get_status_code(event->client);

        if (status_code < 200 || status_code >= 300 || event->data_len <= 0)
        {
            return ESP_OK;
        }

        size_t available = sizeof(response->body) - response->length - 1;
        size_t bytes_to_copy = (size_t)event->data_len;

        if (bytes_to_copy > available)
        {
            bytes_to_copy = available;
            response->overflow = true;
        }

        if (bytes_to_copy > 0)
        {
            memcpy(response->body + response->length, event->data, bytes_to_copy);
            response->length += bytes_to_copy;
            response->body[response->length] = '\0';
        }
    }

    return ESP_OK;
}

esp_err_t http_client_send_uid(const char *uid)
{
    if (uid == NULL || uid[0] == '\0')
    {
        ESP_LOGE(TAG, "UID must not be empty");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return ESP_ERR_NO_MEM;
    }

    if (cJSON_AddStringToObject(json, "token", API_TOKEN) == NULL ||
        cJSON_AddStringToObject(json, "uid", uid) == NULL ||
        cJSON_AddStringToObject(json, "device", DEVICE_ID) == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON payload");
        cJSON_Delete(json);
        return ESP_ERR_NO_MEM;
    }

    char *payload = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    if (payload == NULL)
    {
        ESP_LOGE(TAG, "Failed to serialize JSON payload");
        return ESP_ERR_NO_MEM;
    }

    http_response_t response = {0};

    esp_http_client_config_t config = {
        .url = API_URL,
        .timeout_ms = 10000,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .buffer_size = 1024,
        .buffer_size_tx = 512,
        .event_handler = http_event_handler,
        .user_data = &response,
        .disable_auto_redirect = true,
        .max_redirection_count = 5,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL)
    {
        ESP_LOGE(TAG, "Failed to create HTTP client");
        cJSON_free(payload);
        return ESP_ERR_NO_MEM;
    }

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, payload, strlen(payload));

    esp_err_t err = esp_http_client_perform(client); // Perform the HTTP request, after redirection handling, if any
    if (err == ESP_OK)
    {
        int status_code = esp_http_client_get_status_code(client);

        if (status_code < 200 || status_code >= 300)
        {
            ESP_LOGE(TAG, "Server returned HTTP %d for UID %s", status_code, uid);
            err = ESP_FAIL;
        }
        else if (response.overflow)
        {
            ESP_LOGE(TAG, "Server response is too large for UID %s", uid);
            err = ESP_ERR_INVALID_SIZE;
        }
        else if (strcmp(response.body, "OK") != 0)
        {
            ESP_LOGE(TAG,
                     "Server rejected UID %s: %s",
                     uid,
                     response.length > 0 ? response.body : "empty response");
            err = ESP_FAIL;
        }
        else
        {
            ESP_LOGI(TAG, "UID %s sent successfully (HTTP %d, response: OK)", uid, status_code);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to send UID %s: %s", uid, esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    cJSON_free(payload);

    return err;
}
