/**
 * @file demo_lvgl_ambiental.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "demo_lvgl_ambiental.h"

extern const uint8_t font_inter_regular_10_start[] asm("_binary_font_inter_regular_10_bin_start");
extern const uint8_t font_inter_regular_10_end[] asm("_binary_font_inter_regular_10_bin_end");
extern const uint8_t font_inter_bold_12_start[] asm("_binary_font_inter_bold_12_bin_start");
extern const uint8_t font_inter_bold_12_end[] asm("_binary_font_inter_bold_12_bin_end");
extern const uint8_t font_inter_bold_18_start[] asm("_binary_font_inter_bold_18_bin_start");
extern const uint8_t font_inter_bold_18_end[] asm("_binary_font_inter_bold_18_bin_end");
extern const uint8_t font_inter_bold_22_start[] asm("_binary_font_inter_bold_22_bin_start");
extern const uint8_t font_inter_bold_22_end[] asm("_binary_font_inter_bold_22_bin_end");

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void demo_lvgl_ambiental_init(const char * asset_path)
{
    LV_LOG("Initializing custom C code using LVGL v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    font_inter_regular_10 = lv_binfont_create_from_buffer(
        (void *)font_inter_regular_10_start,
        font_inter_regular_10_end - font_inter_regular_10_start);
    font_inter_bold_12 = lv_binfont_create_from_buffer(
        (void *)font_inter_bold_12_start,
        font_inter_bold_12_end - font_inter_bold_12_start);
    font_inter_bold_18 = lv_binfont_create_from_buffer(
        (void *)font_inter_bold_18_start,
        font_inter_bold_18_end - font_inter_bold_18_start);
    font_inter_bold_22 = lv_binfont_create_from_buffer(
        (void *)font_inter_bold_22_start,
        font_inter_bold_22_end - font_inter_bold_22_start);

    demo_lvgl_ambiental_init_gen(asset_path);

    if (!font_inter_regular_10) font_inter_regular_10 = (lv_font_t *)LV_FONT_DEFAULT;
    if (!font_inter_bold_12) font_inter_bold_12 = (lv_font_t *)LV_FONT_DEFAULT;
    if (!font_inter_bold_18) font_inter_bold_18 = (lv_font_t *)LV_FONT_DEFAULT;
    if (!font_inter_bold_22) font_inter_bold_22 = (lv_font_t *)LV_FONT_DEFAULT;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
