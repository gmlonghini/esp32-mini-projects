#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "esp_log.h"
#include "keypad.h"
#include "audio.h"
#include "radio_sound_map.h"

static const char *TAG = "cs16-radio";
static keypad_config_t keypad_config;

static void init_keypad()
{
    keypad_config.row_pins[0] = GPIO_NUM_4;
    keypad_config.row_pins[1] = GPIO_NUM_5;
    keypad_config.row_pins[2] = GPIO_NUM_18;
    keypad_config.row_pins[3] = GPIO_NUM_19;

    keypad_config.col_pins[0] = GPIO_NUM_21;
    keypad_config.col_pins[1] = GPIO_NUM_22;
    keypad_config.col_pins[2] = GPIO_NUM_23;

    keypad_init(&keypad_config);
}

void app_main(void)
{
    char radio_code[2] = {0};
    radio_code[2] = '\0';
    int key_count = 0;
    audio_init();
    init_keypad();

    while (true)
    {
        char key = keypad_scan(&keypad_config);
        if (key != '\0')
        {
            radio_code[key_count] = key;
            key_count++;
            if (key_count == 2)
            {
                ESP_LOGI(TAG, "Radio Code: %s", radio_code);
                radio_sound_t sound = radio_sound_map_get(radio_code);
                if (sound.data != NULL)
                {
                    audio_play(sound.data, sound.len);
                }
                key_count = 0;
            }
            else
            {
                vTaskDelay(pdMS_TO_TICKS(300));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}