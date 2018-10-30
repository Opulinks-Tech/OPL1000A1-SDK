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

#include "blewifi_ble_api.h"
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
void BleWifi_Ble_Init(void)
{
    /* BLE Init Step 1: Do BLE initialization first */
    BleWifi_Ble_ServerAppInit();
}

void BleWifi_Ble_StartAdvertising(void)
{
    /* Call BLE Stack API to do ble advertising */
    BleWifi_Ble_SendAppMsgToBle(BLEWIFI_APP_MSG_ENTER_ADVERTISING, 0, NULL);
}

void BleWifi_Ble_StopAdvertising(void)
{
    /* Call BLE Stack API to stop ble advertising */
    BleWifi_Ble_SendAppMsgToBle(BLEWIFI_APP_MSG_EXIT_ADVERTISING, 0, NULL);
}
