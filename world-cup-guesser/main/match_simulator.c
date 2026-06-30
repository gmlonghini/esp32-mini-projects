#include "match_simulator.h"
#include "esp_random.h"

typedef struct
{
    uint8_t s1;
    uint8_t s2;
    float weight;
} score_dist_t;

static const score_dist_t normal_dist[] = {
    {2, 1, 15.04}, {1, 0, 11.79}, {1, 1, 8.13}, {3, 1, 6.50}, {0, 1, 6.50}, {2, 0, 5.69}, {3, 2, 5.28}, {4, 2, 4.88}, {0, 0, 4.88}, {1, 2, 4.07}, {3, 0, 3.25}, {0, 2, 3.25}, {4, 1, 2.44}, {2, 3, 2.03}, {6, 1, 1.63}, {2, 2, 1.63}, {4, 0, 1.63}, {0, 3, 1.63}, {5, 2, 1.22}, {3, 3, 1.22}, {4, 3, 0.81}, {1, 3, 0.81}, {6, 5, 0.41}, {1, 5, 0.41}, {6, 0, 0.41}, {5, 1, 0.41}, {7, 5, 0.41}, {3, 4, 0.41}, {8, 0, 0.41}, {1, 4, 0.41}, {7, 1, 0.41}, {6, 3, 0.41}, {2, 4, 0.41}, {0, 4, 0.41}, {5, 3, 0.41}, {1, 7, 0.41}};

static const score_dist_t pen_dist[] = {
    {3, 4, 17.14}, {4, 2, 14.29}, {3, 2, 14.29}, {4, 3, 11.43}, {5, 3, 8.57}, {1, 3, 8.57}, {5, 4, 5.71}, {4, 5, 5.71}, {4, 1, 2.86}, {3, 5, 2.86}, {0, 3, 2.86}, {2, 4, 2.86}, {3, 0, 2.86}};

static inline float randf_0_1(void)
{
    return (esp_random() >> 8) * (1.0f / 16777216.0f);
}

static void pick_score(const score_dist_t *dist,
                       int size,
                       int *out1,
                       int *out2)
{
    float sum = 0;

    for (int i = 0; i < size; i++)
        sum += dist[i].weight;

    float r = randf_0_1() * sum;

    float acc = 0;

    for (int i = 0; i < size; i++)
    {
        acc += dist[i].weight;

        if (r <= acc)
        {
            *out1 = dist[i].s1;
            *out2 = dist[i].s2;
            return;
        }
    }

    *out1 = dist[0].s1;
    *out2 = dist[0].s2;
}

match_result_t match_simulator_generate(void)
{
    match_result_t r = {0};

    pick_score(
        normal_dist,
        sizeof(normal_dist) / sizeof(normal_dist[0]),
        &r.team_1_score,
        &r.team_2_score);

    if (r.team_1_score == r.team_2_score)
    {
        r.has_penalties = true;

        pick_score(
            pen_dist,
            sizeof(pen_dist) / sizeof(pen_dist[0]),
            &r.team_1_penalty_score,
            &r.team_2_penalty_score);

        if (r.team_1_penalty_score == r.team_2_penalty_score)
            r.team_1_penalty_score++;
    }

    return r;
}