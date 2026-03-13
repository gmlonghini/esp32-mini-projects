#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_camera.h"
#include "wifi_sta.h"
#include "http_server.h"

#define BUTTON_PIN GPIO_NUM_48
#define BUZZER_PIN GPIO_NUM_46

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PWD"

static const char *TAG = "alert-cam";

#define CAM_PIN_SIOD GPIO_NUM_4
#define CAM_PIN_SIOC GPIO_NUM_5
#define CAM_PIN_XCLK GPIO_NUM_15
#define CAM_PIN_PWDN -1
#define CAM_PIN_RESET -1
#define CAM_PIN_PCLK GPIO_NUM_13
#define CAM_PIN_VSYNC GPIO_NUM_6
#define CAM_PIN_HREF GPIO_NUM_7
#define CAM_PIN_D0 GPIO_NUM_11
#define CAM_PIN_D1 GPIO_NUM_9
#define CAM_PIN_D2 GPIO_NUM_8
#define CAM_PIN_D3 GPIO_NUM_10
#define CAM_PIN_D4 GPIO_NUM_12
#define CAM_PIN_D5 GPIO_NUM_18
#define CAM_PIN_D6 GPIO_NUM_17
#define CAM_PIN_D7 GPIO_NUM_16

static void init_gpio(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUZZER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&io_conf);

    gpio_set_level(BUZZER_PIN, 0);

    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
    io_conf.mode = GPIO_MODE_INPUT;

    gpio_config(&io_conf);
}

static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,

    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,

    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    .xclk_freq_hz = 20000000,

    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_VGA,

    .jpeg_quality = 12,
    .fb_count = 5,
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_LATEST,
};

static esp_err_t init_camera(void)
{
    ESP_LOGI(TAG, "Initializing camera");

    esp_err_t err = esp_camera_init(&camera_config);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera init failed: %s", esp_err_to_name(err));
        return err;
    }

    sensor_t *s = esp_camera_sensor_get();

    if (s)
    {
        s->set_brightness(s, 0);
        s->set_saturation(s, 0);
        s->set_contrast(s, 0);

        s->set_vflip(s, 1);
        s->set_hmirror(s, 1);
    }

    ESP_LOGI(TAG, "Camera initialized");

    return ESP_OK;
}

void app_main(void)
{
    init_gpio();

    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }

    wifi_station_init(WIFI_SSID, WIFI_PASS);

    if (init_camera() != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera init failed");
        return;
    }

    static int led_level = 0;

    while (1)
    {

        if (gpio_get_level(BUTTON_PIN) == 1)
        {
            gpio_set_level(BUZZER_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(1000));
            gpio_set_level(BUZZER_PIN, 0);

            camera_fb_t *fb = esp_camera_fb_get();

            if (!fb)
            {
                ESP_LOGE(TAG, "Capture failed");
                vTaskDelay(pdMS_TO_TICKS(2000));
                continue;
            }

            ESP_LOGI(TAG,
                     "Image: %ux%u | %u bytes",
                     fb->width,
                     fb->height,
                     fb->len);

            http_server_send_image(fb);
            esp_camera_fb_return(fb);
            vTaskDelay(pdMS_TO_TICKS(5000));
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}