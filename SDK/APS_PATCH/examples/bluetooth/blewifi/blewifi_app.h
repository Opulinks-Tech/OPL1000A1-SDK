/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Opulinks Technology Ltd. (C) 2018
******************************************************************************/

/**
 * @file blewifi_app.h
 * @author Vincent Chen
 * @date 12 Feb 2018
 * @brief File include the definition & function declaration of blewifi app task.
 *
 */

#ifndef __BLEWIFI_APP_H__
#define __BLEWIFI_APP_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_SSID               "D-Link_DIR-612"
#define WIFI_PASSWORD           "12345678"

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

void BleWifiAppInit(void);


#ifdef __cplusplus
}
#endif

#endif /* __BLEWIFI_APP_H__ */
