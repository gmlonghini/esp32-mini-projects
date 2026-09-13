/**
 * @file environmentaldashboard_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "environmentaldashboard_gen.h"
#include "../demo_lvgl_ambiental.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * environmentaldashboard_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if DEMO_LVGL_AMBIENTAL_CHECK_COMPILE_TARGET(DEMO_LVGL_AMBIENTAL_TARGET_ALL)
    if (demo_lvgl_ambiental_check_target(DEMO_LVGL_AMBIENTAL_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "environmentaldashboard_#");
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111827), 0);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);

        lv_obj_add_style(lv_obj_0, &style_screen_base_global, 0);
        lv_obj_t * image_0 = image_create(lv_obj_0);
        lv_obj_set_x(image_0, -2);
        lv_obj_set_y(image_0, 0);
        lv_obj_set_width(image_0, 323);
        lv_obj_set_height(image_0, 240);
        lv_image_set_src(image_0, image_screenbackground);

        lv_obj_t * container_0 = container_create(lv_obj_0);
        lv_obj_set_x(container_0, 16);
        lv_obj_set_y(container_0, 60);
        lv_obj_set_width(container_0, 288);
        lv_obj_set_height(container_0, 164);
        lv_obj_t * card_0 = card_create(container_0);
        lv_obj_set_x(card_0, 0);
        lv_obj_set_y(card_0, 0);
        lv_obj_set_width(card_0, 140);
        lv_obj_set_height(card_0, 88);
        lv_obj_set_style_bg_color(card_0, lv_color_hex(0x1F2937), 0);
        lv_obj_set_style_bg_opa(card_0, 132, 0);
        lv_obj_set_style_border_color(card_0, lv_color_hex(0x4B5563), 0);
        lv_obj_set_style_border_opa(card_0, 155, 0);
        lv_obj_set_style_border_width(card_0, 1, 0);
        lv_obj_set_style_radius(card_0, 12, 0);
        lv_obj_t * container_1 = container_create(card_0);
        lv_obj_set_x(container_1, 86);
        lv_obj_set_y(container_1, 22);
        lv_obj_set_width(container_1, 51);
        lv_obj_set_height(container_1, 51);
        lv_obj_t * container_2 = container_create(container_1);
        lv_obj_set_x(container_2, 14);
        lv_obj_set_y(container_2, 0);
        lv_obj_set_width(container_2, 22);
        lv_obj_set_height(container_2, 51);
        lv_obj_t * image_1 = image_create(container_2);
        lv_obj_set_x(image_1, 6);
        lv_obj_set_y(image_1, 19);
        lv_obj_set_width(image_1, 10);
        lv_obj_set_height(image_1, 25);
        lv_image_set_src(image_1, image_vec_4_104);

        lv_obj_t * image_2 = image_create(container_2);
        lv_obj_set_x(image_2, 0);
        lv_obj_set_y(image_2, 0);
        lv_obj_set_width(image_2, 22);
        lv_obj_set_height(image_2, 51);
        lv_image_set_src(image_2, image_vec_4_105);

        lv_obj_t * label_0 = label_create(card_0);
        lv_obj_set_x(label_0, 16);
        lv_obj_set_y(label_0, 14);
        lv_obj_set_height(label_0, 12);
        lv_label_set_text(label_0, "TEMPERATURA");
        lv_obj_set_style_text_font(label_0, font_inter_regular_10, 0);
        lv_obj_set_style_text_color(label_0, lv_color_hex(0xE5E7EB), 0);

        lv_obj_t * label_1 = label_create(card_0);
        lv_obj_set_x(label_1, 16);
        lv_obj_set_y(label_1, 46);
        lv_obj_set_height(label_1, 27);
        lv_label_set_text(label_1, "25.6 ºC");
        lv_obj_set_style_text_font(label_1, font_inter_bold_22, 0);
        lv_obj_set_style_text_color(label_1, lv_color_hex(0xF9FAFB), 0);

        lv_obj_t * container_3 = container_create(container_0);
        lv_obj_set_x(container_3, 148);
        lv_obj_set_y(container_3, 0);
        lv_obj_set_width(container_3, 140);
        lv_obj_set_height(container_3, 88);
        lv_obj_t * card_1 = card_create(container_3);
        lv_obj_set_x(card_1, 0);
        lv_obj_set_y(card_1, 0);
        lv_obj_set_width(card_1, 140);
        lv_obj_set_height(card_1, 88);
        lv_obj_set_style_bg_color(card_1, lv_color_hex(0x1F2937), 0);
        lv_obj_set_style_bg_opa(card_1, 132, 0);
        lv_obj_set_style_border_color(card_1, lv_color_hex(0x4B5563), 0);
        lv_obj_set_style_border_opa(card_1, 155, 0);
        lv_obj_set_style_border_width(card_1, 1, 0);
        lv_obj_set_style_radius(card_1, 12, 0);
        lv_obj_t * image_3 = image_create(card_1);
        lv_obj_set_x(image_3, 76);
        lv_obj_set_y(image_3, 13);
        lv_obj_set_width(image_3, 63);
        lv_obj_set_height(image_3, 63);
        lv_image_set_src(image_3, image_humidityicon);
        lv_obj_set_flag(image_3, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_t * label_2 = label_create(card_1);
        lv_obj_set_x(label_2, 16);
        lv_obj_set_y(label_2, 14);
        lv_obj_set_height(label_2, 12);
        lv_label_set_text(label_2, "UMIDADE");
        lv_obj_set_style_text_font(label_2, font_inter_regular_10, 0);
        lv_obj_set_style_text_color(label_2, lv_color_hex(0xE5E7EB), 0);

        lv_obj_t * label_3 = label_create(card_1);
        lv_obj_set_x(label_3, 16);
        lv_obj_set_y(label_3, 46);
        lv_obj_set_height(label_3, 27);
        lv_label_set_text(label_3, "100%");
        lv_obj_set_style_text_font(label_3, font_inter_bold_22, 0);
        lv_obj_set_style_text_color(label_3, lv_color_hex(0xF9FAFB), 0);

        lv_obj_t * card_2 = card_create(container_0);
        lv_obj_set_x(card_2, 0);
        lv_obj_set_y(card_2, 100);
        lv_obj_set_width(card_2, 288);
        lv_obj_set_height(card_2, 64);
        lv_obj_set_style_bg_color(card_2, lv_color_hex(0x1F2937), 0);
        lv_obj_set_style_bg_opa(card_2, 132, 0);
        lv_obj_set_style_border_color(card_2, lv_color_hex(0x4B5563), 0);
        lv_obj_set_style_border_opa(card_2, 155, 0);
        lv_obj_set_style_border_width(card_2, 1, 0);
        lv_obj_set_style_radius(card_2, 12, 0);
        lv_obj_t * image_4 = image_create(card_2);
        lv_obj_set_x(image_4, 16);
        lv_obj_set_y(image_4, 8);
        lv_obj_set_width(image_4, 47);
        lv_obj_set_height(image_4, 47);
        lv_image_set_src(image_4, image_fanicon);
        lv_obj_set_flag(image_4, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_t * label_4 = label_create(card_2);
        lv_obj_set_x(label_4, 78);
        lv_obj_set_y(label_4, 24);
        lv_obj_set_height(label_4, 15);
        lv_label_set_text(label_4, "VENTOINHA");
        lv_obj_set_style_text_font(label_4, font_inter_bold_12, 0);
        lv_obj_set_style_text_color(label_4, lv_color_hex(0xE5E7EB), 0);

        lv_obj_t * fanstatusindicator_0 = fanstatusindicator_create(card_2, false);
        lv_obj_set_x(fanstatusindicator_0, 242);
        lv_obj_set_y(fanstatusindicator_0, 17);

        lv_obj_t * label_5 = label_create(lv_obj_0);
        lv_obj_set_x(label_5, 74);
        lv_obj_set_y(label_5, 13);
        lv_obj_set_height(label_5, 20);
        lv_label_set_text(label_5, "PAINEL AMBIENTAL");
        lv_obj_set_style_text_font(label_5, font_inter_bold_18, 0);
        lv_obj_set_style_text_color(label_5, lv_color_hex(0xF9FAFB), 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

