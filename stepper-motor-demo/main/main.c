#include "driver/gpio.h"
#include "esp_timer.h"

#define IN1 GPIO_NUM_5
#define IN2 GPIO_NUM_18
#define IN3 GPIO_NUM_19
#define IN4 GPIO_NUM_21

#define N_COILS 4
#define HALF_STEP_SEQUENCE_LENGTH (N_COILS * 2)
#define STEP_INTERVAL_US 1000 // 1kHz

static const uint8_t half_step_sequence[HALF_STEP_SEQUENCE_LENGTH][N_COILS] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}};

static void apply_step(int s)
{
    gpio_set_level(IN1, half_step_sequence[s][0]);
    gpio_set_level(IN2, half_step_sequence[s][1]);
    gpio_set_level(IN3, half_step_sequence[s][2]);
    gpio_set_level(IN4, half_step_sequence[s][3]);
}

void stepper_callback_isr(void *arg)
{
    static int step = 0;
    step++;
    apply_step(step % HALF_STEP_SEQUENCE_LENGTH);
}

void app_main(void)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask =
            (1ULL << IN1) |
            (1ULL << IN2) |
            (1ULL << IN3) |
            (1ULL << IN4),
    };

    gpio_config(&io_conf);

    const esp_timer_create_args_t timer_args = {
        .callback = &stepper_callback_isr,
        .name = "stepper_timer"};

    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);

    esp_timer_start_periodic(timer, STEP_INTERVAL_US);
}