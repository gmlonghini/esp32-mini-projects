#include "tournament.h"
#include "team_image.h"
#include "esp_log.h"
#include <string.h>

typedef struct
{
    const char *team1;
    const char *team2;
    int winner; // 1 or 2 - make an enum
} match_state_t;

static match_state_t round0[16] = {
    {TEAM_ZAF, TEAM_CAN, 0},
    {TEAM_NLD, TEAM_MAR, 0},
    {TEAM_DEU, TEAM_PRY, 0},
    {TEAM_FRA, TEAM_SWE, 0},

    {TEAM_BEL, TEAM_SEN, 0},
    {TEAM_USA, TEAM_BIH, 0},
    {TEAM_ESP, TEAM_AUT, 0},
    {TEAM_PRT, TEAM_HRV, 0},

    {TEAM_BRA, TEAM_JPN, 0},
    {TEAM_CIV, TEAM_NOR, 0},
    {TEAM_MEX, TEAM_ECU, 0},
    {TEAM_ENG, TEAM_COD, 0},

    {TEAM_CHE, TEAM_DZA, 0},
    {TEAM_COL, TEAM_GHA, 0},
    {TEAM_AUS, TEAM_EGY, 0},
    {TEAM_ARG, TEAM_CPV, 0},

};

static match_state_t round1[8];
static match_state_t round2[4];
static match_state_t round3[2];
static match_state_t final_match[1];
static match_state_t third_place_match[1];

static int current_round = 0;
static int current_match = 0;

static const int matches_per_round[] = {
    16, 8, 4, 2, 1, 1};

static match_state_t *get_round(int round)
{
    switch (round)
    {
    case 0:
        return round0;
    case 1:
        return round1;
    case 2:
        return round2;
    case 3:
        return round3;
    case 4:
        return third_place_match;
    case 5:
        return final_match;
    default:
        return NULL;
    }
}

static const char *phase_to_string(int round)
{
    switch (round)
    {
    case 0:
        return "16 Avos";
    case 1:
        return "Oitavas de Final";
    case 2:
        return "Quartas de Final";
    case 3:
        return "Semifinal";
    case 4:
        return "3 Lugar";
    case 5:
        return "Final";
    default:
        return "";
    }
}

static void build_next_round(match_state_t *src, match_state_t *dst, int size)
{
    for (int i = 0; i < size; i++)
    {
        int i1 = i * 2;
        int i2 = i * 2 + 1;

        match_state_t *m1 = &src[i1];
        match_state_t *m2 = &src[i2];

        const char *w1 = (m1->winner == 1) ? m1->team1 : m1->team2;
        const char *w2 = (m2->winner == 1) ? m2->team1 : m2->team2;

        dst[i].team1 = w1;
        dst[i].team2 = w2;
        dst[i].winner = 0;
    }
}

void tournament_init(void)
{
    current_round = 0;
    current_match = 0;

    memset(round1, 0, sizeof(round1));
    memset(round2, 0, sizeof(round2));
    memset(round3, 0, sizeof(round3));
    memset(final_match, 0, sizeof(final_match));
    memset(third_place_match, 0, sizeof(third_place_match));
}

const tournament_match_t *tournament_get_match(void)
{
    static tournament_match_t out;

    if (current_round > 5)
    {
        ESP_LOGE("tournament", "No more rounds available");
        return NULL;
    }

    if (current_match >= matches_per_round[current_round])
    {
        ESP_LOGE("tournament", "No more matches available in this round");
        return NULL;
    }

    match_state_t *round = get_round(current_round);
    if (!round)
    {
        ESP_LOGE("tournament", "Invalid round: %d", current_round);
        return NULL;
    }

    match_state_t *m = &round[current_match];

    if (!m->team1 || !m->team2)
    {
        ESP_LOGE("tournament", "Match %d in round %d is not set up properly", current_match, current_round);
        return NULL;
    }

    out.team1 = m->team1;
    out.team2 = m->team2;

    out.team1_img = team_image_get(m->team1);
    out.team2_img = team_image_get(m->team2);

    out.phase = phase_to_string(current_round);

    return &out;
}

void tournament_submit_result(int winner)
{
    match_state_t *round = get_round(current_round);
    if (!round)
        return;

    match_state_t *m = &round[current_match];
    m->winner = winner;

    // semifinal
    if (current_round == 3)
    {
        const char *winner_name = (winner == 1) ? m->team1 : m->team2;
        const char *loser_name = (winner == 1) ? m->team2 : m->team1;

        if (current_match == 0)
        {
            third_place_match[0].team1 = loser_name;
            final_match[0].team1 = winner_name;
        }
        else
        {
            third_place_match[0].team2 = loser_name;
            final_match[0].team2 = winner_name;
        }
    }

    current_match++;

    if (current_match >= matches_per_round[current_round])
    {
        switch (current_round)
        {
        case 0:
            build_next_round(round0, round1, 8);
            break;

        case 1:
            build_next_round(round1, round2, 4);
            break;

        case 2:
            build_next_round(round2, round3, 2);
            break;
        }

        current_round++;
        current_match = 0;
    }
}