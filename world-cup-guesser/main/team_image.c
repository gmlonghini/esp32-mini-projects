#include "team_image.h"
#include <string.h>

LV_IMAGE_DECLARE(brazil);
LV_IMAGE_DECLARE(canada);
LV_IMAGE_DECLARE(south_africa);
LV_IMAGE_DECLARE(japan);
LV_IMAGE_DECLARE(germany);
LV_IMAGE_DECLARE(paraguay);
LV_IMAGE_DECLARE(netherlands);
LV_IMAGE_DECLARE(morocco);

LV_IMAGE_DECLARE(ivory_coast);
LV_IMAGE_DECLARE(norway);
LV_IMAGE_DECLARE(france);
LV_IMAGE_DECLARE(sweden);

LV_IMAGE_DECLARE(mexico);
LV_IMAGE_DECLARE(ecuador);
LV_IMAGE_DECLARE(england);
LV_IMAGE_DECLARE(dr_congo);

LV_IMAGE_DECLARE(belgium);
LV_IMAGE_DECLARE(senegal);
LV_IMAGE_DECLARE(united_states);
LV_IMAGE_DECLARE(bosnia_and_herzegovina);

LV_IMAGE_DECLARE(spain);
LV_IMAGE_DECLARE(austria);
LV_IMAGE_DECLARE(portugal);
LV_IMAGE_DECLARE(croatia);

LV_IMAGE_DECLARE(switzerland);
LV_IMAGE_DECLARE(algeria);
LV_IMAGE_DECLARE(australia);
LV_IMAGE_DECLARE(egypt);

LV_IMAGE_DECLARE(argentina);
LV_IMAGE_DECLARE(cape_verde);
LV_IMAGE_DECLARE(colombia);
LV_IMAGE_DECLARE(ghana);

typedef struct
{
    const char *code;
    const lv_img_dsc_t *img;
} team_image_map_t;

static const team_image_map_t map[] = {

    {TEAM_BRA, &brazil},
    {TEAM_CAN, &canada},
    {TEAM_ZAF, &south_africa},
    {TEAM_JPN, &japan},

    {TEAM_DEU, &germany},
    {TEAM_PRY, &paraguay},
    {TEAM_NLD, &netherlands},
    {TEAM_MAR, &morocco},

    {TEAM_CIV, &ivory_coast},
    {TEAM_NOR, &norway},
    {TEAM_FRA, &france},
    {TEAM_SWE, &sweden},

    {TEAM_MEX, &mexico},
    {TEAM_ECU, &ecuador},
    {TEAM_ENG, &england},
    {TEAM_COD, &dr_congo},

    {TEAM_BEL, &belgium},
    {TEAM_SEN, &senegal},
    {TEAM_USA, &united_states},
    {TEAM_BIH, &bosnia_and_herzegovina},

    {TEAM_ESP, &spain},
    {TEAM_AUT, &austria},
    {TEAM_PRT, &portugal},
    {TEAM_HRV, &croatia},

    {TEAM_CHE, &switzerland},
    {TEAM_DZA, &algeria},
    {TEAM_AUS, &australia},
    {TEAM_EGY, &egypt},

    {TEAM_ARG, &argentina},
    {TEAM_CPV, &cape_verde},
    {TEAM_COL, &colombia},
    {TEAM_GHA, &ghana},
};

#define TEAM_COUNT (sizeof(map) / sizeof(map[0]))

const lv_img_dsc_t *team_image_get(const char *team_code)
{
    if (!team_code)
        return NULL;

    for (int i = 0; i < TEAM_COUNT; i++)
    {
        if (strcmp(team_code, map[i].code) == 0)
        {
            return map[i].img;
        }
    }

    return NULL;
}