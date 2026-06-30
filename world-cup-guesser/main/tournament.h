#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        TOURNAMENT_ROUND_OF_16 = 0,
        TOURNAMENT_ROUND_OF_8,
        TOURNAMENT_QUARTER_FINAL,
        TOURNAMENT_SEMI_FINAL,
        TOURNAMENT_THIRD_PLACE,
        TOURNAMENT_FINAL
    } tournament_phase_t;

    typedef struct
    {
        const char *team1;
        const char *team2;

        const lv_img_dsc_t *team1_img;
        const lv_img_dsc_t *team2_img;

        const char *phase;
    } tournament_match_t;

    void tournament_init(void);
    const tournament_match_t *tournament_get_match(void);
    void tournament_submit_result(int winner);

#ifdef __cplusplus
}
#endif

#endif