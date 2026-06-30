#ifndef MATCH_SIMULATOR_H
#define MATCH_SIMULATOR_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int team_1_score;
        int team_2_score;

        bool has_penalties;

        int team_1_penalty_score;
        int team_2_penalty_score;
    } match_result_t;

    match_result_t match_simulator_generate(void);

#ifdef __cplusplus
}
#endif

#endif // MATCH_SIMULATOR_H