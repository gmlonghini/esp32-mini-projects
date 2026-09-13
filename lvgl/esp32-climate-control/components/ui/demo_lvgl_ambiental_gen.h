/**
 * @file demo_lvgl_ambiental_gen.h
 */

#ifndef LVGL_PRO_DEMO_LVGL_AMBIENTAL_GEN_H
#define LVGL_PRO_DEMO_LVGL_AMBIENTAL_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

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



/* Prototypes for target functions, needed by responsive const definitions */

void demo_lvgl_ambiental_set_target(uint32_t target);
uint32_t demo_lvgl_ambiental_get_target(void);
bool demo_lvgl_ambiental_check_target(uint32_t target);

/*********************
 *      DEFINES
 *********************/

#define DEMO_LVGL_AMBIENTAL_TARGET_UNDEFINED  (0 << 1)
#define DEMO_LVGL_AMBIENTAL_TARGET_TARGET1    (1 << 1)
#define DEMO_LVGL_AMBIENTAL_TARGET_ALL        0x0FFFFFFF

/* By default compile for all targets, allowing to switch to any targets at runtime */
#ifndef DEMO_LVGL_AMBIENTAL_COMPILE_TARGET
#define DEMO_LVGL_AMBIENTAL_COMPILE_TARGET DEMO_LVGL_AMBIENTAL_TARGET_ALL
#endif

#define DEMO_LVGL_AMBIENTAL_CHECK_COMPILE_TARGET(target) (DEMO_LVGL_AMBIENTAL_COMPILE_TARGET & (target) ? 1 : 0)

#define CONST_BASELIB_ACCENT_GLOBAL lv_color_hex(0x70609C)
#define CONST_BASELIB_UNIT_SM_GLOBAL 4


#ifndef LV_XML_EVAL_STRING_BUF_SIZE
    #define LV_XML_EVAL_STRING_BUF_SIZE 256
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t style_screen_base_global;

/*----------------
 * Fonts
 *----------------*/

/* Targets: any */
extern lv_font_t * font_inter_regular_10;
extern lv_font_t * font_inter_bold_22;
extern lv_font_t * font_inter_bold_12;
extern lv_font_t * font_inter_bold_18;


/*----------------
 * Images
 *----------------*/

/* Targets: any */
extern const void * image_screenbackground;
extern const void * image_vec_4_104;
extern const void * image_vec_4_105;
extern const void * image_humidityicon;
extern const void * image_fanicon;

/*----------------
 * Subjects
 *----------------*/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void demo_lvgl_ambiental_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/base/card/card_gen.h"
#include "components/base/container/container_gen.h"
#include "components/base/image/image_gen.h"
#include "components/base/label/label_gen.h"
#include "components/base/panel/panel_gen.h"
#include "components/fanicon/fanicon_gen.h"
#include "components/fanstatusindicator/fanstatusindicator_gen.h"
#include "components/humidityicon/humidityicon_gen.h"
#include "components/temperatureicon/temperatureicon_gen.h"
#include "screens/environmentaldashboard_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_DEMO_LVGL_AMBIENTAL_GEN_H*/