#include "keypad.h"
#include "esp_log.h"

void keypad_init(keypad_config_t *config)
{
    for (int i = 0; i < KEYPAD_ROWS; i++)
    {
        gpio_set_direction(config->row_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(config->row_pins[i], 1);
    }

    for (int i = 0; i < KEYPAD_COLS; i++)
    {
        gpio_set_direction(config->col_pins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(config->col_pins[i], GPIO_PULLUP_ONLY);
    }
}

char keypad_scan(keypad_config_t *config)
{
    const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'},
        {'*', '0', '#'}};

    for (int row = 0; row < KEYPAD_ROWS; row++)
    {
        gpio_set_level(config->row_pins[row], 0);

        for (int col = 0; col < KEYPAD_COLS; col++)
        {
            if (gpio_get_level(config->col_pins[col]) == 0)
            {
                gpio_set_level(config->row_pins[row], 1);
                return keys[row][col];
            }
        }

        gpio_set_level(config->row_pins[row], 1);
    }

    return '\0';
}