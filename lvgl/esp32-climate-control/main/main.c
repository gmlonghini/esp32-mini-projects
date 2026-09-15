#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "lvgl.h"
#include "sht30.h"
#include "st7789.h"

#include "demo_lvgl_ambiental.h"
#include "environmentaldashboard_gen.h"

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

#define PIN_MOSI 11
#define PIN_MISO -1
#define PIN_SCLK 12
#define PIN_CS 10
#define PIN_DC 9
#define PIN_RST 14

#define PIN_I2C_SDA 47
#define PIN_I2C_SCL 48
#define PIN_TOUCH 37
#define PIN_RELAY 21

#define SHT30_I2C_PORT I2C_NUM_0
#define SHT30_I2C_FREQUENCY 100000
#define SHT30_ADDRESS 0x45
#define SHT30_READ_PERIOD_MS 3000

#define TOUCH_ACTIVE_LEVEL 1
#define RELAY_ACTIVE_LEVEL 1
#define BUTTON_POLL_PERIOD_MS 10
#define BUTTON_DEBOUNCE_MS 100
#define BUTTON_STARTUP_TIME_MS 500

static SemaphoreHandle_t lvgl_mutex;
static const char *TAG = "climate_control";

static void lvgl_tick_cb(void *arg)
{
    (void)arg;
    lv_tick_inc(1);
}

static void lvgl_flush_cb(lv_display_t *display,
                          const lv_area_t *area,
                          uint8_t *px_map)
{
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;

    uint16_t width = x2 - x1 + 1;
    uint16_t height = y2 - y1 + 1;

    st7789_set_window(x1, y1, x2, y2);
    st7789_push_color(px_map, width * height * 2);

    lv_display_flush_ready(display);
}

static void setup_lvgl(void)
{
    lv_init();

    const esp_timer_create_args_t lvgl_tick = {
        .callback = &lvgl_tick_cb,
        .name = "lvgl_tick",
    };

    esp_timer_handle_t lvgl_timer;

    ESP_ERROR_CHECK(
        esp_timer_create(&lvgl_tick, &lvgl_timer));

    ESP_ERROR_CHECK(
        esp_timer_start_periodic(lvgl_timer, 1000));

    lv_display_t *display =
        lv_display_create(DISPLAY_HEIGHT, DISPLAY_WIDTH);

    static lv_color_t display_buffer[DISPLAY_WIDTH * 32];

    lv_display_set_buffers(
        display,
        display_buffer,
        NULL,
        sizeof(display_buffer),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(display, lvgl_flush_cb);
}

static void setup_ui(void)
{
    demo_lvgl_ambiental_init("");
    lv_obj_t *screen = environmentaldashboard_create();
    lv_label_set_text(environmentaldashboard_temperature_value, "--.- ºC");
    lv_label_set_text(environmentaldashboard_humidity_value, "--%");
    lv_screen_load(screen);
}

static void setup_control_gpio(void)
{
    const gpio_config_t touch_config = {
        .pin_bit_mask = 1ULL << PIN_TOUCH,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    const gpio_config_t relay_config = {
        .pin_bit_mask = 1ULL << PIN_RELAY,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_set_level(PIN_RELAY, !RELAY_ACTIVE_LEVEL));
    ESP_ERROR_CHECK(gpio_config(&relay_config));
    ESP_ERROR_CHECK(gpio_config(&touch_config));
}

static void update_fan_ui(bool enabled)
{
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);

    if (enabled)
    {
        lv_obj_add_state(environmentaldashboard_fan_status, LV_STATE_CHECKED);
    }
    else
    {
        lv_obj_remove_state(environmentaldashboard_fan_status, LV_STATE_CHECKED);
    }

    xSemaphoreGive(lvgl_mutex);
}

static void button_task(void *arg)
{
    (void)arg;

    bool relay_enabled = false;
    vTaskDelay(pdMS_TO_TICKS(BUTTON_STARTUP_TIME_MS));

    int sampled_level = gpio_get_level(PIN_TOUCH);
    int stable_level = sampled_level;
    bool touch_armed = stable_level != TOUCH_ACTIVE_LEVEL;
    TickType_t last_change = xTaskGetTickCount();

    ESP_LOGI(TAG, "Botao touch pronto: GPIO=%d, nivel=%d", PIN_TOUCH, stable_level);

    while (true)
    {
        const TickType_t now = xTaskGetTickCount();
        const int current_level = gpio_get_level(PIN_TOUCH);

        if (current_level != sampled_level)
        {
            sampled_level = current_level;
            last_change = now;
        }
        else if (current_level != stable_level &&
                 now - last_change >= pdMS_TO_TICKS(BUTTON_DEBOUNCE_MS))
        {
            stable_level = current_level;
            ESP_LOGI(TAG, "Botao touch: nivel=%d", stable_level);

            if (stable_level == TOUCH_ACTIVE_LEVEL && touch_armed)
            {
                touch_armed = false;
                relay_enabled = !relay_enabled;
                ESP_ERROR_CHECK(gpio_set_level(
                    PIN_RELAY,
                    relay_enabled ? RELAY_ACTIVE_LEVEL : !RELAY_ACTIVE_LEVEL));
                update_fan_ui(relay_enabled);
                ESP_LOGI(TAG, "Ventoinha %s", relay_enabled ? "ligada" : "desligada");
            }
            else if (stable_level != TOUCH_ACTIVE_LEVEL)
            {
                touch_armed = true;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_PERIOD_MS));
    }
}

static void update_sensor_ui(float temperature, float humidity)
{
    const int temperature_tenths =
        (int)(temperature * 10.0f + (temperature >= 0.0f ? 0.5f : -0.5f));
    const int temperature_magnitude =
        temperature_tenths < 0 ? -temperature_tenths : temperature_tenths;
    const int humidity_percent = (int)(humidity + 0.5f);

    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    lv_label_set_text_fmt(
        environmentaldashboard_temperature_value,
        "%s%d.%d ºC",
        temperature_tenths < 0 ? "-" : "",
        temperature_magnitude / 10,
        temperature_magnitude % 10);
    lv_label_set_text_fmt(
        environmentaldashboard_humidity_value,
        "%d%%",
        humidity_percent);
    xSemaphoreGive(lvgl_mutex);
}

static void sht30_task(void *arg)
{
    (void)arg;

    sht30_t sensor;
    const sht30_config_t config = {
        .i2c_port = SHT30_I2C_PORT,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .device_address = SHT30_ADDRESS,
        .scl_speed_hz = SHT30_I2C_FREQUENCY,
    };

    esp_err_t err;

    while ((err = sht30_init(&sensor, &config)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Falha ao inicializar SHT30: %s", esp_err_to_name(err));
        vTaskDelay(pdMS_TO_TICKS(SHT30_READ_PERIOD_MS));
    }

    ESP_LOGI(
        TAG,
        "SHT30 inicializado: SDA=%d, SCL=%d, endereco=0x%02X",
        PIN_I2C_SDA,
        PIN_I2C_SCL,
        SHT30_ADDRESS);

    while (true)
    {
        float temperature;
        float humidity;

        err = sht30_read(&sensor, &temperature, &humidity);
        if (err == ESP_OK)
        {
            ESP_LOGI(
                TAG,
                "Temperatura: %.2f C | Umidade: %.2f%%",
                (double)temperature,
                (double)humidity);
            update_sensor_ui(temperature, humidity);
        }
        else
        {
            ESP_LOGE(TAG, "Falha ao ler SHT30: %s", esp_err_to_name(err));
        }

        vTaskDelay(pdMS_TO_TICKS(SHT30_READ_PERIOD_MS));
    }
}

static void lvgl_task(void *arg)
{
    (void)arg;

    while (true)
    {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);

        lv_timer_handler();

        xSemaphoreGive(lvgl_mutex);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    lvgl_mutex = xSemaphoreCreateMutex();
    setup_control_gpio();

    st7789_config_t lcd_config = {
        .spi_host = SPI2_HOST,

        .pin_mosi = PIN_MOSI,
        .pin_miso = PIN_MISO,
        .pin_sclk = PIN_SCLK,
        .pin_cs = PIN_CS,
        .pin_dc = PIN_DC,
        .pin_rst = PIN_RST,

        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,

        .colstart = 0,
        .rowstart = 0,

        .spi_clock_hz = 80 * 1000 * 1000,
    };

    st7789_init(&lcd_config);
    st7789_set_rotation(1);
    setup_lvgl();
    setup_ui();
    xTaskCreate(lvgl_task, "lvgl_task", 16384, NULL, 5, NULL);
    xTaskCreate(sht30_task, "sht30_task", 4096, NULL, 4, NULL);
    xTaskCreate(button_task, "button_task", 3072, NULL, 4, NULL);
}
