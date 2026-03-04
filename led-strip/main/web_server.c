#include "web_server.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

static const char *TAG = "web_server";

static webserver_set_color_cb_t s_set_color_cb = NULL;
static webserver_get_color_cb_t s_get_color_cb = NULL;

static esp_err_t root_get_handler(httpd_req_t *req)
{
    rgb_color_t current = {0};

    if (s_get_color_cb)
        current = s_get_color_cb();

    char html[2048];

    snprintf(html, sizeof(html),
             "<!DOCTYPE html>"
             "<html>"
             "<head>"
             "<meta name='viewport' content='width=device-width, initial-scale=1'>"
             "<title>ESP32 LED Control</title>"
             "</head>"
             "<body style='text-align:center;font-family:sans-serif;'>"
             "<h2>ESP32 LED Color Picker</h2>"
             "<input type='color' id='picker'>"
             "<script>"
             "const picker=document.getElementById('picker');"
             "picker.value='#%02X%02X%02X';"
             "picker.addEventListener('input',function(){"
             " const c=picker.value;"
             " const r=parseInt(c.substr(1,2),16);"
             " const g=parseInt(c.substr(3,2),16);"
             " const b=parseInt(c.substr(5,2),16);"
             " fetch(`/set?r=${r}&g=${g}&b=${b}`);"
             "});"
             "</script>"
             "</body>"
             "</html>",
             current.r, current.g, current.b);

    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* ===================== SET COLOR ===================== */

static esp_err_t set_color_handler(httpd_req_t *req)
{
    char query[100];

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK)
    {
        char param[10];
        rgb_color_t color = {0};

        if (httpd_query_key_value(query, "r", param, sizeof(param)) == ESP_OK)
            color.r = atoi(param);

        if (httpd_query_key_value(query, "g", param, sizeof(param)) == ESP_OK)
            color.g = atoi(param);

        if (httpd_query_key_value(query, "b", param, sizeof(param)) == ESP_OK)
            color.b = atoi(param);

        if (s_set_color_cb)
            s_set_color_cb(color);
    }

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void web_server_start(webserver_set_color_cb_t set_cb,
                      webserver_get_color_cb_t get_cb)
{
    s_set_color_cb = set_cb;
    s_get_color_cb = get_cb;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = NULL};

        httpd_uri_t set_color = {
            .uri = "/set",
            .method = HTTP_GET,
            .handler = set_color_handler,
            .user_ctx = NULL};

        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &set_color);

        ESP_LOGI(TAG, "Web server started");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start web server");
    }
}