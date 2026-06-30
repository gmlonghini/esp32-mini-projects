#ifndef TEAM_IMAGE_H
#define TEAM_IMAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define TEAM_BRA "BRA"
#define TEAM_CAN "CAN"
#define TEAM_ZAF "ZAF"
#define TEAM_JPN "JPN"
#define TEAM_DEU "DEU"
#define TEAM_PRY "PRY"
#define TEAM_NLD "NLD"
#define TEAM_MAR "MAR"

#define TEAM_CIV "CIV"
#define TEAM_NOR "NOR"
#define TEAM_FRA "FRA"
#define TEAM_SWE "SWE"

#define TEAM_MEX "MEX"
#define TEAM_ECU "ECU"
#define TEAM_ENG "ENG"
#define TEAM_COD "COD"

#define TEAM_BEL "BEL"
#define TEAM_SEN "SEN"
#define TEAM_USA "USA"
#define TEAM_BIH "BIH"

#define TEAM_ESP "ESP"
#define TEAM_AUT "AUT"
#define TEAM_PRT "PRT"
#define TEAM_HRV "HRV"

#define TEAM_CHE "CHE"
#define TEAM_DZA "DZA"
#define TEAM_AUS "AUS"
#define TEAM_EGY "EGY"

#define TEAM_ARG "ARG"
#define TEAM_CPV "CPV"
#define TEAM_COL "COL"
#define TEAM_GHA "GHA"

    const lv_img_dsc_t *team_image_get(const char *team_code);

#ifdef __cplusplus
}
#endif

#endif