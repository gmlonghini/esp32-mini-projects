#include "radio_sound_map.h"
#include <string.h>
// cs 1.6 sounds
#include "affirmative.h"
#include "all_right_lets_move_out.h"
#include "bomb_has_been_defused.h"
#include "circle_around_back.h"
#include "counter-terrorists_win.h"
#include "cover_me.h"
#include "enemy_down.h"
#include "enemy_spotted.h"
#include "follow_me.h"
#include "get_in_position_and_wait_for_my_go.h"
#include "get_out_of_there_its_gonna_blow.h"
#include "go_go_go.h"
#include "headshot.h"
#include "hold_this_position.h"
#include "hostages_has_been_rescued.h"
#include "ill_take_the_point.h"
#include "im_hit.h"
#include "im_in_position.h"
#include "lock_n_load.h"
#include "need_backup.h"
#include "negative.h"
#include "ok_lets_go.h"
#include "protect_the_vip_team.h"
#include "regroup_team.h"
#include "reporting_in.h"
#include "report_in_team.h"
#include "roger_that.h"
#include "round_draw.h"
#include "sector_clear.h"
#include "storm_the_front.h"
#include "taking_fire_need_assistants.h"
#include "team_fall_back.h"
#include "team_fire_in_the_hole.h"
#include "teammate_down.h"
#include "terrorists_win.h"
#include "you_take_the_point.h"

typedef struct
{
    const char *code;
    const unsigned char *data;
    unsigned int len;
} radio_sound_entry_t;

static const radio_sound_entry_t table[] = {
    {"#1", affirmative_wav, affirmative_wav_len},
    {"#2", all_right_lets_move_out_wav, all_right_lets_move_out_wav_len},
    {"#3", team_fire_in_the_hole_wav, team_fire_in_the_hole_wav_len},
    {"#4", circle_around_back_wav, circle_around_back_wav_len},
    {"#5", you_take_the_point_wav, you_take_the_point_wav_len},
    {"#6", cover_me_wav, cover_me_wav_len},
    {"#7", enemy_down_wav, enemy_down_wav_len},
    {"#8", enemy_spotted_wav, enemy_spotted_wav_len},
    {"#9", follow_me_wav, follow_me_wav_len},

    {"*1", get_in_position_and_wait_for_my_go_wav, get_in_position_and_wait_for_my_go_wav_len},
    {"*2", get_out_of_there_its_gonna_blow_wav, get_out_of_there_its_gonna_blow_wav_len},
    {"*3", go_go_go_wav, go_go_go_wav_len},
    {"*4", teammate_down_wav, teammate_down_wav_len},
    {"*5", hold_this_position_wav, hold_this_position_wav_len},
    {"*6", team_fall_back_wav, team_fall_back_wav_len},
    {"*7", ill_take_the_point_wav, ill_take_the_point_wav_len},
    {"*8", im_hit_wav, im_hit_wav_len},
    {"*9", im_in_position_wav, im_in_position_wav_len},

    {"01", lock_n_load_wav, lock_n_load_wav_len},
    {"02", need_backup_wav, need_backup_wav_len},
    {"03", negative_wav, negative_wav_len},
    {"04", ok_lets_go_wav, ok_lets_go_wav_len},
    {"05", protect_the_vip_team_wav, protect_the_vip_team_wav_len},
    {"06", regroup_team_wav, regroup_team_wav_len},
    {"07", reporting_in_wav, reporting_in_wav_len},
    {"08", report_in_team_wav, report_in_team_wav_len},
    {"09", roger_that_wav, roger_that_wav_len},

    {"11", round_draw_wav, round_draw_wav_len},
    {"12", sector_clear_wav, sector_clear_wav_len},
    {"13", storm_the_front_wav, storm_the_front_wav_len},
    {"14", taking_fire_need_assistants_wav, taking_fire_need_assistants_wav_len},
    {"15", hostages_has_been_rescued_wav, hostages_has_been_rescued_wav_len},
    {"16", bomb_has_been_defused_wav, bomb_has_been_defused_wav_len},
    {"17", headshot_wav, headshot_wav_len},
    {"18", terrorists_win_wav, terrorists_win_wav_len},
    {"19", counter_terrorists_win_wav, counter_terrorists_win_wav_len},

};

radio_sound_t radio_sound_map_get(const char *code)
{
    for (int i = 0; i < sizeof(table) / sizeof(table[0]); i++)
    {
        if (strcmp(code, table[i].code) == 0)
        {
            return (radio_sound_t){
                .data = table[i].data,
                .len = table[i].len};
        }
    }

    return (radio_sound_t){
        .data = NULL,
        .len = 0};
}