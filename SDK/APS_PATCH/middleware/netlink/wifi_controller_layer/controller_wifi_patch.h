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

#ifndef __CONTROLLER_WIFI_PATCH_H__
#define __CONTROLLER_WIFI_PATCH_H__

#include "controller_wifi.h"
#include "controller_wifi_com_patch.h"

extern osTimerId wifi_cmd_tout_timer;
extern uint32_t g_cmd_retry;
extern osTimerId g_tCtrlWifiPsTimer;

int controller_wifi_init_patch(void *pScanResult);
void CtrlWifi_PsTout(void const *arg);

int CtrlWifi_PsStateForce_impl(WifiSta_PSForceMode_t mode, uint32_t timeout);

typedef int (*CtrlWifi_PsStateForce_fp_t)(WifiSta_PSForceMode_t mode, uint32_t timeout);

extern CtrlWifi_PsStateForce_fp_t CtrlWifi_PsStateForce;

#endif  //__CONTROLLER_WIFI_PATCH_H__
