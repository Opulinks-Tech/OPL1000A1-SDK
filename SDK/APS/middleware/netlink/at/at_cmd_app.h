/******************************************************************************
*  Copyright 2017, Netlink Communication Corp.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlink Communication Corp. (C) 2017
******************************************************************************/

#ifndef __AT_CMD_APP_H__
#define __AT_CMD_APP_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"


int  wifi_station_get_connect_status(void);
void wifi_station_init_connect_status(void);
void at_cmd_wifi_hook(void);

void at_cmd_app_func_preinit(void);

#define STATION_IDLE                    0
#define STATION_CONNECTING              1
#define STATION_WRONG_PASSWORD          2
#define STATION_NO_AP_FOUND             3
#define STATION_CONNECT_FAIL            4
#define STATION_GOT_IP                  5

#ifdef __cplusplus
}
#endif

#endif /* __AT_CMD_APP_H__ */
