#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stdint.h>

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_color_t;

/* Callback chamado quando nova cor é recebida via HTTP */
typedef void (*webserver_set_color_cb_t)(rgb_color_t color);

/* Callback para obter cor atual */
typedef rgb_color_t (*webserver_get_color_cb_t)(void);

/* Inicializa servidor web */
void web_server_start(webserver_set_color_cb_t set_cb,
                      webserver_get_color_cb_t get_cb);

#ifdef __cplusplus
}
#endif

#endif