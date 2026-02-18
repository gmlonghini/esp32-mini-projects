#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "stdio.h"
#include "esp_err.h"

esp_err_t http_client_post_temperature(float temperature);

#endif // HTTP_CLIENT_H