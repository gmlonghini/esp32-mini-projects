#ifndef WIFI_STATION_H
#define WIFI_STATION_H

#include "stdio.h"
#include "stdbool.h"

void wifi_station_init(char *wifi_ssid, char *wifi_password);
bool wifi_station_is_connected(void);

#endif // WIFI_STATION_H