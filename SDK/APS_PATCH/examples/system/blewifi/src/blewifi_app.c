/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Opulinks Technology Ltd. (C) 2018
******************************************************************************/

/**
 * @file blewifi_app.c
 * @author Vincent Chen
 * @date 12 Feb 2018
 * @brief File creates the wifible app task architecture.
 *
 */
#include "blewifi_configuration.h"
#include "blewifi_app.h"
#include "blewifi_wifi_api.h"
#include "blewifi_ble_api.h"
#include "blewifi_ctrl.h"
#include "sys_common_api.h"
#include "ps_public.h"

blewifi_ota_t *gTheOta = 0;

void BleWifiAppInit(void)
{
	gTheOta = 0;

    /* Wi-Fi Initialization */
    BleWifi_Wifi_Init();

    /* BLE Stack Initialization */
    BleWifi_Ble_Init();

    /* blewifi "control" task Initialization */
    BleWifi_Ctrl_Init();

    /* Power saving settings */
    ps_smart_sleep(BLEWIFI_COM_POWER_SAVE_EN);
}
