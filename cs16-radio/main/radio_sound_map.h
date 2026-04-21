#ifndef RADIO_SOUND_MAP_H
#define RADIO_SOUND_MAP_H

#include <stddef.h>

typedef struct {
    const unsigned char *data;
    size_t len;
} radio_sound_t;

radio_sound_t radio_sound_map_get(const char *code);

#endif /* RADIO_SOUND_MAP_H */