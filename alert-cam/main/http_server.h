#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_err.h"
#include "esp_camera.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t http_server_send_image(camera_fb_t *fb);

#ifdef __cplusplus
}
#endif

#endif