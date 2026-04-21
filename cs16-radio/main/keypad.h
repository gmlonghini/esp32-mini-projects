#ifndef KEYPAD_H
#define KEYPAD_H

#include "driver/gpio.h"

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

typedef struct
{
    gpio_num_t row_pins[KEYPAD_ROWS];
    gpio_num_t col_pins[KEYPAD_COLS];
} keypad_config_t;

void keypad_init(keypad_config_t *config);
char keypad_scan(keypad_config_t *config);

#endif /* KEYPAD_H */