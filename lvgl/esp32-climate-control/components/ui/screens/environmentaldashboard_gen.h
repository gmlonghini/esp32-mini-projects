/**
 * @file environmentaldashboard_gen.h
 */

#ifndef LVGL_PRO_ENVIRONMENTALDASHBOARD_GEN_H
#define LVGL_PRO_ENVIRONMENTALDASHBOARD_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#if defined(LV_USE_XML) && LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/



extern lv_obj_t * environmentaldashboard_temperature_value;
extern lv_obj_t * environmentaldashboard_humidity_value;
extern lv_obj_t * environmentaldashboard_fan_status;

lv_obj_t * environmentaldashboard_create(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_ENVIRONMENTALDASHBOARD_GEN_H*/
