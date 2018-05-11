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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "blewifi_server_app.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
void blewifi_ble_init(void)
{
    /* BLE Init Step1: Do BLE initialization first */
   	BleWifiServerAppInit();
}

void blewifi_ble_start_initialization(void)
{
    /* Call BLE Stack API to do ble initialization */
    BleWifiSendAppMsgToBle(BLEWIFI_APP_MSG_INITIALIZING, 0, NULL);
}

void blewifi_ble_start_advertising(void)
{
    /* Call BLE Stack API to do ble advertising */
    BleWifiSendAppMsgToBle(BLEWIFI_APP_MSG_ENTER_ADVERTISING, 0, NULL);
}

void blewifi_ble_stop_advertising(void)
{
     /* Call BLE Stack API to stop ble advertising */
    BleWifiSendAppMsgToBle(BLEWIFI_APP_MSG_EXIT_ADVERTISING, 0, NULL);
}

