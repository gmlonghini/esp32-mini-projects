/**
 * @file fanstatusindicator_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "fanstatusindicator_gen.h"
#include "../../demo_lvgl_ambiental.h"

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

lv_obj_t * fanstatusindicator_create(lv_obj_t * parent, bool checked)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_desc_0_checked;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_desc_0_checked);

        lv_style_set_bg_color(&style_desc_0_checked, lv_color_hex(0x34C759));
        lv_style_set_bg_opa(&style_desc_0_checked, 255);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if DEMO_LVGL_AMBIENTAL_CHECK_COMPILE_TARGET(DEMO_LVGL_AMBIENTAL_TARGET_ALL)
    if (demo_lvgl_ambiental_check_target(DEMO_LVGL_AMBIENTAL_TARGET_ALL)) {
        lv_obj_t * container_0 = container_create(parent);
        lv_obj_set_name_static(container_0, "fanstatusindicator_#");
        lv_obj_set_width(container_0, 30);
        lv_obj_set_height(container_0, 30);

        lv_obj_t * card_0 = card_create(container_0);
        lv_obj_set_x(card_0, 0);
        lv_obj_set_y(card_0, 0);
        lv_obj_set_width(card_0, 30);
        lv_obj_set_height(card_0, 30);
        lv_obj_set_style_bg_color(card_0, lv_color_hex(0x808080), 0);
        lv_obj_set_style_radius(card_0, 30, 0);
        lv_obj_set_state(card_0, LV_STATE_CHECKED, checked);
        lv_obj_add_style(card_0, &style_desc_0_checked, LV_STATE_CHECKED);

        the_root = container_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

