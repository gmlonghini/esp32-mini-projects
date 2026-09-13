/**
 * @file temperatureicon_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "temperatureicon_gen.h"
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

lv_obj_t * temperatureicon_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if DEMO_LVGL_AMBIENTAL_CHECK_COMPILE_TARGET(DEMO_LVGL_AMBIENTAL_TARGET_ALL)
    if (demo_lvgl_ambiental_check_target(DEMO_LVGL_AMBIENTAL_TARGET_ALL)) {
        lv_obj_t * container_0 = container_create(parent);
        lv_obj_set_name_static(container_0, "temperatureicon_#");
        lv_obj_set_width(container_0, 51);
        lv_obj_set_height(container_0, 51);

        lv_obj_t * container_1 = container_create(container_0);
        lv_obj_set_x(container_1, 14);
        lv_obj_set_y(container_1, 0);
        lv_obj_set_width(container_1, 22);
        lv_obj_set_height(container_1, 51);
        lv_obj_t * image_0 = image_create(container_1);
        lv_obj_set_x(image_0, 6);
        lv_obj_set_y(image_0, 19);
        lv_obj_set_width(image_0, 10);
        lv_obj_set_height(image_0, 25);
        lv_image_set_src(image_0, image_vec_4_104);

        lv_obj_t * image_1 = image_create(container_1);
        lv_obj_set_x(image_1, 0);
        lv_obj_set_y(image_1, 0);
        lv_obj_set_width(image_1, 22);
        lv_obj_set_height(image_1, 51);
        lv_image_set_src(image_1, image_vec_4_105);

        the_root = container_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

