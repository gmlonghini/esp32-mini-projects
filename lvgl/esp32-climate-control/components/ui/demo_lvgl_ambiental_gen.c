/**
 * @file demo_lvgl_ambiental_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "demo_lvgl_ambiental_gen.h"

#if defined(LV_USE_XML) && LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t demo_lvgl_ambiental_target = DEMO_LVGL_AMBIENTAL_TARGET_ALL;

/*----------------
 * Translations
 *----------------*/

#ifndef LV_EDITOR_PREVIEW
    static const char * translation_languages[] = {"en", "de", NULL};
    static const char * translation_tags[] = {"dog", "cat", "house", NULL};
    static const char * translation_texts[] = {
        "This is a dog", "Das ist ein Hund", /* dog */
        "A curious little cat", "Eine neugierige kleine Katze", /* cat */
        "The house is cozy and warm", "Das Haus ist gemütlich und warm", /* house */
    };
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

lv_font_t * font_inter_regular_10;
lv_font_t * font_inter_bold_22;
lv_font_t * font_inter_bold_12;
lv_font_t * font_inter_bold_18;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * image_screenbackground = NULL;
extern const void * image_screenbackground_data;
const void * image_vec_4_104 = NULL;
extern const void * image_vec_4_104_data;
const void * image_vec_4_105 = NULL;
extern const void * image_vec_4_105_data;
const void * image_humidityicon = NULL;
extern const void * image_humidityicon_data;
const void * image_fanicon = NULL;
extern const void * image_fanicon_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t style_screen_base_global;

/*----------------
 * Subjects
 *----------------*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void demo_lvgl_ambiental_init_gen(const char * asset_path)
{
    char buf[256];

    /* When running from the editor the theme set from the XML should overwrite this */
#if !defined(LV_EDITOR_PREVIEW)
#if LV_USE_THEME_SIMPLE
    lv_display_t * disp = lv_display_get_default();
    lv_theme_t * th = lv_theme_simple_init(disp);
    lv_display_set_theme(disp, th);
#else
    LV_LOG_WARN("Simple theme is selected in project.xml but LV_USE_THEME_SIMPLE is disabled");
#endif
#endif /*LV_EDITOR_PREVIEW*/


    /*----------------
     * Fonts
     *----------------*/

    /* Targets: any */

    #if DEMO_LVGL_AMBIENTAL_CHECK_COMPILE_TARGET(DEMO_LVGL_AMBIENTAL_TARGET_ALL)
    if (demo_lvgl_ambiental_check_target(DEMO_LVGL_AMBIENTAL_TARGET_ALL)) {
        if (!font_inter_regular_10) {
            /* font_inter_regular_10 */
            /* create bin font 'font_inter_regular_10' from file */
            lv_snprintf(buf, 256, "%s%s", asset_path, "fonts/font_inter_regular_10.bin");
            font_inter_regular_10 = lv_binfont_create(buf);

        }
        if (!font_inter_bold_22) {
            /* font_inter_bold_22 */
            /* create bin font 'font_inter_bold_22' from file */
            lv_snprintf(buf, 256, "%s%s", asset_path, "fonts/font_inter_bold_22.bin");
            font_inter_bold_22 = lv_binfont_create(buf);

        }
        if (!font_inter_bold_12) {
            /* font_inter_bold_12 */
            /* create bin font 'font_inter_bold_12' from file */
            lv_snprintf(buf, 256, "%s%s", asset_path, "fonts/font_inter_bold_12.bin");
            font_inter_bold_12 = lv_binfont_create(buf);

        }
        if (!font_inter_bold_18) {
            /* font_inter_bold_18 */
            /* create bin font 'font_inter_bold_18' from file */
            lv_snprintf(buf, 256, "%s%s", asset_path, "fonts/font_inter_bold_18.bin");
            font_inter_bold_18 = lv_binfont_create(buf);

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if DEMO_LVGL_AMBIENTAL_CHECK_COMPILE_TARGET(DEMO_LVGL_AMBIENTAL_TARGET_ALL)
    if (demo_lvgl_ambiental_check_target(DEMO_LVGL_AMBIENTAL_TARGET_ALL)) {
        /* image_screenbackground */
        if (!image_screenbackground) {
            image_screenbackground = &image_screenbackground_data;
        }
        /* image_vec_4_104 */
        if (!image_vec_4_104) {
            image_vec_4_104 = &image_vec_4_104_data;
        }
        /* image_vec_4_105 */
        if (!image_vec_4_105) {
            image_vec_4_105 = &image_vec_4_105_data;
        }
        /* image_humidityicon */
        if (!image_humidityicon) {
            image_humidityicon = &image_humidityicon_data;
        }
        /* image_fanicon */
        if (!image_fanicon) {
            image_fanicon = &image_fanicon_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_screen_base_global);

        lv_style_set_border_width(&style_screen_base_global, 0);
        lv_style_set_radius(&style_screen_base_global, 0);
        lv_style_set_shadow_width(&style_screen_base_global, 0);
        lv_style_set_shadow_opa(&style_screen_base_global, 0);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    /*----------------
     * Translations
     *----------------*/

    #ifndef LV_EDITOR_PREVIEW
        lv_translation_add_static(translation_languages, translation_tags, translation_texts);
        lv_translation_set_language(translation_languages[0]);
    #endif

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&font_inter_regular_10, "font_inter_regular_10");
    check_font(&font_inter_bold_22, "font_inter_bold_22");
    check_font(&font_inter_bold_12, "font_inter_bold_12");
    check_font(&font_inter_bold_18, "font_inter_bold_18");

    /* Register fonts */
    lv_xml_register_font(NULL, "font_inter_regular_10", font_inter_regular_10);
    lv_xml_register_font(NULL, "font_inter_bold_22", font_inter_bold_22);
    lv_xml_register_font(NULL, "font_inter_bold_12", font_inter_bold_12);
    lv_xml_register_font(NULL, "font_inter_bold_18", font_inter_bold_18);

    /* Register subjects */

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "image_screenbackground", image_screenbackground);
    lv_xml_register_image(NULL, "image_vec_4_104", image_vec_4_104);
    lv_xml_register_image(NULL, "image_vec_4_105", image_vec_4_105);
    lv_xml_register_image(NULL, "image_humidityicon", image_humidityicon);
    lv_xml_register_image(NULL, "image_fanicon", image_fanicon);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void demo_lvgl_ambiental_set_target(uint32_t target)
{
    demo_lvgl_ambiental_target = target;
}

uint32_t demo_lvgl_ambiental_get_target(void)
{
    return demo_lvgl_ambiental_target;
}

bool demo_lvgl_ambiental_check_target(uint32_t target)
{
    return (demo_lvgl_ambiental_target & target) ? true : false;
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}