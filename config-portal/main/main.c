#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#define WIFI_AP_SSID "ESP32_AP"
#define WIFI_AP_PASSWORD "12345678"
#define WIFI_AP_CHANNEL 1
#define WIFI_AP_MAX_CONN 4
#define LED_GPIO GPIO_NUM_13
#define TOUCH_BUTTON_GPIO GPIO_NUM_27
#define BOOT_INPUT_STABILIZATION_MS 100
#define BLINK_PERIOD_DEFAULT_MS 1000
#define BLINK_PERIOD_MIN_MS 100
#define BLINK_PERIOD_MAX_MS 10000
#define NVS_NAMESPACE "config"
#define NVS_BLINK_PERIOD_KEY "blink_ms"

static const char *TAG = "config_portal";

extern const char web_page_start[] asm("_binary_web_page_html_start");

static esp_err_t save_blink_period(uint32_t period_ms)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        return err;
    }

    err = nvs_set_u32(handle, NVS_BLINK_PERIOD_KEY, period_ms);
    if (err == ESP_OK)
    {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err;
}

static uint32_t load_blink_period(void)
{
    uint32_t period_ms = BLINK_PERIOD_DEFAULT_MS;
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);

    if (err == ESP_OK)
    {
        err = nvs_get_u32(handle, NVS_BLINK_PERIOD_KEY, &period_ms);
        nvs_close(handle);
    }

    if (err != ESP_OK || period_ms < BLINK_PERIOD_MIN_MS ||
        period_ms > BLINK_PERIOD_MAX_MS)
    {
        period_ms = BLINK_PERIOD_DEFAULT_MS;
        ESP_LOGI(TAG, "Using default LED blink period: %lu ms",
                 (unsigned long)period_ms);
    }
    else
    {
        ESP_LOGI(TAG, "LED blink period loaded from NVS: %lu ms",
                 (unsigned long)period_ms);
    }

    return period_ms;
}

static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    return httpd_resp_send(req, web_page_start, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t blink_period_handler(httpd_req_t *req)
{
    char body[32];
    size_t received = 0;

    if (req->content_len == 0 || req->content_len >= sizeof(body))
    {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                   "Requisição inválida");
    }

    while (received < req->content_len)
    {
        int ret = httpd_req_recv(req, body + received,
                                 req->content_len - received);
        if (ret <= 0)
        {
            return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                       "Não foi possível ler o valor");
        }
        received += ret;
    }
    body[received] = '\0';

    const char prefix[] = "period_ms=";
    if (strncmp(body, prefix, sizeof(prefix) - 1) != 0)
    {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                   "Campo ausente");
    }

    char *end;
    long period_ms = strtol(body + sizeof(prefix) - 1, &end, 10);
    if (*end != '\0' || period_ms < BLINK_PERIOD_MIN_MS ||
        period_ms > BLINK_PERIOD_MAX_MS)
    {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                   "O período deve estar entre 100 e 10000 ms");
    }

    esp_err_t err = save_blink_period((uint32_t)period_ms);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to save LED blink period: %s",
                 esp_err_to_name(err));
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                   "Não foi possível salvar o período");
    }

    ESP_LOGI(TAG, "LED blink period saved to NVS: %ld ms", period_ms);

    char response[48];
    snprintf(response, sizeof(response), "{\"period_ms\":%ld}", period_ms);
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
}

static void web_server_start(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    const httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_handler,
    };

    const httpd_uri_t blink_period_uri = {
        .uri = "/blink-period",
        .method = HTTP_POST,
        .handler = blink_period_handler,
    };

    ESP_ERROR_CHECK(httpd_start(&server, &config));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &blink_period_uri));

    ESP_LOGI(TAG, "Web page available at http://192.168.4.1");
}

static void led_blink_task(void *arg)
{
    const uint32_t period_ms = load_blink_period();
    bool led_on = false;

    gpio_config_t led_config = {
        .pin_bit_mask = 1ULL << LED_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&led_config));
    gpio_set_level(LED_GPIO, 0);

    while (true)
    {
        led_on = !led_on;
        gpio_set_level(LED_GPIO, led_on);
        vTaskDelay(pdMS_TO_TICKS(period_ms));
    }
}

static bool configuration_mode_requested(void)
{
    gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << TOUCH_BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&button_config));

    vTaskDelay(pdMS_TO_TICKS(BOOT_INPUT_STABILIZATION_MS));
    bool pressed = gpio_get_level(TOUCH_BUTTON_GPIO) == 1;
    ESP_LOGI(TAG, "Configuration button is %s",
             pressed ? "pressed" : "not pressed");
    return pressed;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Wi-Fi event ID: %ld", event_id);
}

static void wifi_ap_init(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_create_default_wifi_ap() != NULL ? ESP_OK : ESP_FAIL);

    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        NULL);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .ssid_len = strlen(WIFI_AP_SSID),
            .channel = WIFI_AP_CHANNEL,
            .password = WIFI_AP_PASSWORD,
            .max_connection = WIFI_AP_MAX_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };

    if (strlen(WIFI_AP_PASSWORD) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi AP started");
    ESP_LOGI(TAG, "SSID: %s", WIFI_AP_SSID);
    ESP_LOGI(TAG, "Password: %s", WIFI_AP_PASSWORD);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    if (configuration_mode_requested())
    {
        ESP_LOGI(TAG, "Starting configuration mode");
        wifi_ap_init();
        web_server_start();
    }
    else
    {
        ESP_LOGI(TAG, "Starting LED mode");
        BaseType_t task_created = xTaskCreate(led_blink_task, "led_blink", 2048,
                                              NULL, 5, NULL);
        ESP_ERROR_CHECK(task_created == pdPASS ? ESP_OK : ESP_ERR_NO_MEM);
    }
}
