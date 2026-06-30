#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "st7789.h"
#include "match_simulator.h"
#include "driver/gpio.h"
#include "tournament.h"

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

#define PIN_MOSI 11
#define PIN_SCLK 12
#define PIN_CS 10
#define PIN_DC 9
#define PIN_RST 14

#define BUTTON_GPIO GPIO_NUM_4
#define BUTTON_PRESSED 1
#define BUTTON_NOT_PRESSED 0

typedef enum
{
    UI_STATE_SHOW_MATCH = 0,
    UI_STATE_SHOW_RESULT
} ui_state_t;

SemaphoreHandle_t lvgl_mutex = NULL;

static lv_obj_t *score_team1_label = NULL;
static lv_obj_t *score_team2_label = NULL;

static lv_obj_t *label_team1 = NULL;
static lv_obj_t *label_team2 = NULL;

static lv_obj_t *flag_team1 = NULL;
static lv_obj_t *flag_team2 = NULL;

static lv_obj_t *label_phase = NULL;

static lv_obj_t *penalty_container = NULL;
static lv_obj_t *score_team1_pen_label = NULL;
static lv_obj_t *score_team2_pen_label = NULL;

static void button_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
}

static void ui_show_match(const tournament_match_t *match)
{
    lv_label_set_text(label_phase, match->phase);
    lv_image_set_src(flag_team1, match->team1_img);
    lv_image_set_src(flag_team2, match->team2_img);
    lv_label_set_text(label_team1, match->team1);
    lv_label_set_text(label_team2, match->team2);

    lv_label_set_text(score_team1_label, "");
    lv_label_set_text(score_team2_label, "");
    lv_label_set_text(score_team1_pen_label, "");
    lv_label_set_text(score_team2_pen_label, "");

    lv_obj_add_flag(penalty_container, LV_OBJ_FLAG_HIDDEN);

    lv_obj_set_style_bg_opa(label_team1, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(label_team2, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_color(label_team1, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_color(label_team2, lv_color_hex(0x000000), LV_PART_MAIN);
}

static void ui_show_result(const match_result_t *r)
{
    char buf1[8];
    char buf2[8];
    int winner;

    snprintf(buf1, sizeof(buf1), "%d", r->team_1_score);
    snprintf(buf2, sizeof(buf2), "%d", r->team_2_score);

    lv_label_set_text(score_team1_label, buf1);
    lv_label_set_text(score_team2_label, buf2);

    if (r->has_penalties)
    {
        char p1[8];
        char p2[8];

        snprintf(p1, sizeof(p1), "%d", r->team_1_penalty_score);
        snprintf(p2, sizeof(p2), "%d", r->team_2_penalty_score);

        lv_label_set_text(score_team1_pen_label, p1);
        lv_label_set_text(score_team2_pen_label, p2);

        lv_obj_clear_flag(penalty_container, LV_OBJ_FLAG_HIDDEN);

        winner = r->team_1_penalty_score > r->team_2_penalty_score ? 1 : 2;
    }
    else
    {
        lv_obj_add_flag(penalty_container, LV_OBJ_FLAG_HIDDEN);

        winner = r->team_1_score > r->team_2_score ? 1 : 2;
    }

    tournament_submit_result(winner);
    lv_color_t green = lv_color_hex(0x2ECC71);

    if (winner == 1)
    {
        lv_obj_set_style_bg_color(label_team1, green, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(label_team1, LV_OPA_COVER, LV_PART_MAIN);
    }
    else
    {
        lv_obj_set_style_bg_color(label_team2, green, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(label_team2, LV_OPA_COVER, LV_PART_MAIN);
    }
}

static void button_task(void *param)
{
    int last_state = BUTTON_NOT_PRESSED;
    ui_state_t ui_state = UI_STATE_SHOW_RESULT;

    const tournament_match_t *match = NULL;
    match_result_t result;

    while (1)
    {
        int current_state = gpio_get_level(BUTTON_GPIO);

        if ((last_state == BUTTON_NOT_PRESSED) &&
            (current_state == BUTTON_PRESSED))
        {
            xSemaphoreTake(lvgl_mutex, portMAX_DELAY);

            switch (ui_state)
            {
            case UI_STATE_SHOW_MATCH:
                match = tournament_get_match();
                ui_show_match(match);
                ui_state = UI_STATE_SHOW_RESULT;
                break;

            case UI_STATE_SHOW_RESULT:
                result = match_simulator_generate();
                ui_show_result(&result);
                ui_state = UI_STATE_SHOW_MATCH;
                break;
            }

            xSemaphoreGive(lvgl_mutex);
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        last_state = current_state;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void lvgl_tick_cb(void *arg)
{
    (void)arg;
    lv_tick_inc(1);
}

static void lvgl_flush_cb(lv_display_t *disp,
                          const lv_area_t *area,
                          uint8_t *px_map)
{
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;

    uint16_t w = x2 - x1 + 1;
    uint16_t h = y2 - y1 + 1;

    st7789_set_window(x1, y1, x2, y2);
    st7789_push_color(px_map, w * h * 2);

    lv_display_flush_ready(disp);
}

static void setup_lvgl(void)
{
    lv_init();

    const esp_timer_create_args_t lvgl_tick = {
        .callback = &lvgl_tick_cb,
        .name = "lvgl_tick"};

    esp_timer_handle_t lvgl_timer;
    esp_timer_create(&lvgl_tick, &lvgl_timer);
    esp_timer_start_periodic(lvgl_timer, 1000);

    lv_display_t *display =
        lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    static lv_color_t buf[DISPLAY_WIDTH * 32];

    lv_display_set_buffers(display,
                           buf,
                           NULL,
                           sizeof(buf),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(display, lvgl_flush_cb);
}

static void lvgl_task(void *arg)
{
    (void)arg;

    while (1)
    {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_timer_handler();
        xSemaphoreGive(lvgl_mutex);

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void)
{
    lvgl_mutex = xSemaphoreCreateMutex();

    button_init();
    tournament_init();

    st7789_config_t lcd_cfg = {
        .spi_host = SPI2_HOST,
        .pin_mosi = PIN_MOSI,
        .pin_sclk = PIN_SCLK,
        .pin_cs = PIN_CS,
        .pin_dc = PIN_DC,
        .pin_rst = PIN_RST,
        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,
        .colstart = 0,
        .rowstart = 0,
        .spi_clock_hz = 80 * 1000 * 1000};

    st7789_init(&lcd_cfg);
    setup_lvgl();

    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);

    lv_obj_t *scr = lv_screen_active();

    lv_obj_t *cont_team1 = lv_obj_create(scr);
    lv_obj_set_size(cont_team1, 100, 160);
    lv_obj_align(cont_team1, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_set_style_border_width(cont_team1, 0, 0);
    lv_obj_set_style_pad_all(cont_team1, 0, 0);
    lv_obj_set_style_bg_opa(cont_team1, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(cont_team1, LV_SCROLLBAR_MODE_OFF);

    label_phase = lv_label_create(scr);
    lv_label_set_text(label_phase, "FASE");
    lv_obj_set_style_text_font(label_phase, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label_phase, lv_color_hex(0x000000), 0);
    lv_obj_align(label_phase, LV_ALIGN_TOP_MID, 0, 10);

    label_team1 = lv_label_create(cont_team1);
    lv_obj_set_style_text_font(label_team1, &lv_font_montserrat_16, 0);
    lv_obj_align(label_team1, LV_ALIGN_TOP_MID, 0, 10);

    flag_team1 = lv_image_create(cont_team1);
    lv_obj_align(flag_team1, LV_ALIGN_CENTER, 0, 0);

    score_team1_label = lv_label_create(cont_team1);
    lv_obj_align(score_team1_label, LV_ALIGN_BOTTOM_MID, 0, -5);

    lv_obj_set_style_radius(score_team1_label, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(score_team1_label, lv_color_hex(0x222222), 0);
    lv_obj_set_style_bg_opa(score_team1_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(score_team1_label, lv_color_white(), 0);
    lv_obj_set_style_pad_hor(score_team1_label, 10, 0);
    lv_obj_set_style_pad_ver(score_team1_label, 5, 0);
    lv_obj_set_style_text_font(score_team1_label, &lv_font_montserrat_14, 0);

    lv_obj_t *cont_team2 = lv_obj_create(scr);
    lv_obj_set_size(cont_team2, 100, 160);
    lv_obj_align(cont_team2, LV_ALIGN_RIGHT_MID, -10, 0);

    lv_obj_set_style_border_width(cont_team2, 0, 0);
    lv_obj_set_style_pad_all(cont_team2, 0, 0);
    lv_obj_set_style_bg_opa(cont_team2, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(cont_team2, LV_SCROLLBAR_MODE_OFF);

    label_team2 = lv_label_create(cont_team2);
    lv_obj_set_style_text_font(label_team2, &lv_font_montserrat_16, 0);
    lv_obj_align(label_team2, LV_ALIGN_TOP_MID, 0, 10);

    flag_team2 = lv_image_create(cont_team2);
    lv_obj_align(flag_team2, LV_ALIGN_CENTER, 0, 0);

    score_team2_label = lv_label_create(cont_team2);
    lv_obj_align(score_team2_label, LV_ALIGN_BOTTOM_MID, 0, -5);

    lv_obj_set_style_radius(score_team2_label, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(score_team2_label, lv_color_hex(0x222222), 0);
    lv_obj_set_style_bg_opa(score_team2_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(score_team2_label, lv_color_white(), 0);
    lv_obj_set_style_pad_hor(score_team2_label, 10, 0);
    lv_obj_set_style_pad_ver(score_team2_label, 5, 0);
    lv_obj_set_style_text_font(score_team2_label, &lv_font_montserrat_14, 0);

    penalty_container = lv_obj_create(scr);
    lv_obj_set_size(penalty_container, 140, 50);
    lv_obj_align(penalty_container, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_set_style_bg_opa(penalty_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(penalty_container, 0, 0);
    lv_obj_set_style_pad_all(penalty_container, 0, 0);
    lv_obj_set_scrollbar_mode(penalty_container, LV_SCROLLBAR_MODE_OFF);

    score_team1_pen_label = lv_label_create(penalty_container);
    lv_label_set_text(score_team1_pen_label, "0");
    lv_obj_align(score_team1_pen_label, LV_ALIGN_LEFT_MID, 15, 0);

    lv_obj_set_style_text_font(score_team1_pen_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(score_team1_pen_label, lv_color_hex(0xD4AF37), 0);

    lv_obj_t *pen_sep = lv_label_create(penalty_container);
    lv_label_set_text(pen_sep, "-");
    lv_obj_align(pen_sep, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_text_font(pen_sep, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(pen_sep, lv_color_hex(0x888888), 0);

    score_team2_pen_label = lv_label_create(penalty_container);
    lv_label_set_text(score_team2_pen_label, "0");
    lv_obj_align(score_team2_pen_label, LV_ALIGN_RIGHT_MID, -15, 0);

    lv_obj_set_style_text_font(score_team2_pen_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(score_team2_pen_label, lv_color_hex(0xD4AF37), 0);

    const tournament_match_t *match = tournament_get_match();
    ui_show_match(match);

    xSemaphoreGive(lvgl_mutex);

    xTaskCreate(lvgl_task, "lvgl_task", 16384, NULL, 5, NULL);
    xTaskCreate(button_task, "button_task", 4096, NULL, 5, NULL);
}