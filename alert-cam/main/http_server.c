#include "http_server.h"
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "http_client";

/* endereço do servidor Flask */
#define SERVER_URL "http://192.168.0.6:5000/upload"

esp_err_t http_server_send_image(camera_fb_t *fb)
{
    if (!fb)
    {
        ESP_LOGE(TAG, "Frame buffer NULL");
        return ESP_FAIL;
    }

    esp_http_client_config_t config = {
        .url = SERVER_URL,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    if (client == NULL)
    {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        return ESP_FAIL;
    }

    esp_http_client_set_header(client, "Content-Type", "image/jpeg");

    esp_http_client_set_post_field(
        client,
        (const char *)fb->buf,
        fb->len);

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "Image sent, status = %d", status);
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);

    return err;
}