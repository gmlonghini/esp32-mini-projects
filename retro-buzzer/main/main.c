#include <stdio.h>
#include <driver/ledc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

#define BUZZER_PIN 18
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RESOLUTION LEDC_TIMER_10_BIT
#define LEDC_DUTY 256
#define LEDC_FREQUENCY 1000 // 1kHz

#define BPM 180
#define QUARTER_NOTE_MS (60000 / BPM)

// Define notes
#define NOTE_REST 0
#define NOTE_E3 165
#define NOTE_G3 196
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440

typedef struct
{
    int note;
    int duration;
} note_t;

const note_t intro[] =
    {
        {NOTE_E4, 8},
        {NOTE_E4, 8},
        {NOTE_REST, 8},
        {NOTE_E4, 8},
        {NOTE_REST, 8},
        {NOTE_C4, 8},
        {NOTE_E4, 8},
        {NOTE_REST, 8},

        {NOTE_G4, 4},
        {NOTE_REST, 8},
        {NOTE_REST, 8},
        {NOTE_REST, 8},

        {NOTE_G3, 4},
        {NOTE_REST, 8},
        {NOTE_REST, 8},
        {NOTE_REST, 8},
};

const note_t theme_a[] =
    {
        {NOTE_C4, 6},
        {NOTE_REST, 8},
        {NOTE_G3, 6},
        {NOTE_REST, 8},
        {NOTE_E3, 6},
        {NOTE_REST, 8},

        {NOTE_A3, 6},
        {NOTE_REST, 8},
        {NOTE_B3, 6},
        {NOTE_REST, 8},
        {NOTE_AS3, 6},
        {NOTE_A3, 6},
        {NOTE_REST, 8},

        {NOTE_G3, 6},
        {NOTE_E4, 6},
        {NOTE_G4, 6},
        {NOTE_A4, 4},
        {NOTE_REST, 8},

        {NOTE_F4, 6},
        {NOTE_G4, 6},
        {NOTE_REST, 8},
        {NOTE_E4, 6},
        {NOTE_C4, 6},
        {NOTE_D4, 6},
        {NOTE_B3, 4},
};

const note_t theme_b[] =
    {
        {NOTE_F4, 6},
        {NOTE_REST, 8},
        {NOTE_F4, 6},
        {NOTE_REST, 8},
        {NOTE_F4, 6},
        {NOTE_REST, 8},

        {NOTE_E4, 6},
        {NOTE_REST, 8},
        {NOTE_D4, 6},
        {NOTE_REST, 8},
        {NOTE_C4, 6},
        {NOTE_REST, 8},

        {NOTE_G3, 6},
        {NOTE_REST, 8},
        {NOTE_E3, 6},
        {NOTE_REST, 8},

        {NOTE_A3, 6},
        {NOTE_REST, 8},
        {NOTE_B3, 6},
        {NOTE_REST, 8},
        {NOTE_AS3, 6},
        {NOTE_A3, 6},
        {NOTE_REST, 8},
};

const note_t end[] =
    {
        {NOTE_G4, 6},
        {NOTE_E4, 6},
        {NOTE_C4, 6},
        {NOTE_D4, 6},
        {NOTE_B3, 4},
        {NOTE_REST, 8},
        {NOTE_G3, 4},
};

void init_ledc()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RESOLUTION,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = BUZZER_PIN,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .duty = 0};

    ledc_channel_config(&ledc_channel);
}

void play_note(int freq, int duration_ms)
{
    int play_time = duration_ms * 0.9;
    int rest_time = duration_ms - play_time;

    if (freq != 0)
    {
        ledc_set_freq(LEDC_MODE, LEDC_TIMER, freq);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    }

    ets_delay_us(play_time * 1000);

    ledc_stop(LEDC_MODE, LEDC_CHANNEL, 0);

    if (rest_time > 0)
    {
        ets_delay_us(rest_time * 1000);
    }
}

void play_music_section(const note_t *section, int len)
{
    for (int i = 0; i < len; i++)
    {
        int duration_ms = (QUARTER_NOTE_MS * 4) / section[i].duration;
        play_note(section[i].note, duration_ms);
    }
}

void app_main(void)
{
    init_ledc();
    play_music_section(intro, sizeof(intro) / sizeof(note_t));
    play_music_section(theme_a, sizeof(theme_a) / sizeof(note_t));
    play_music_section(theme_a, sizeof(theme_a) / sizeof(note_t));
    play_music_section(theme_b, sizeof(theme_b) / sizeof(note_t));
    play_music_section(end, sizeof(end) / sizeof(note_t));
    play_music_section(theme_b, sizeof(theme_b) / sizeof(note_t));
    play_music_section(end, sizeof(end) / sizeof(note_t));

    ledc_stop(LEDC_MODE, LEDC_CHANNEL, 0);
}
