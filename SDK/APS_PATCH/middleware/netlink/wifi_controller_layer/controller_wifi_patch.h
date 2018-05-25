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
#include "wifi_mac_dcf.h"


#define RF_CMD_PARAM_NUM    10


typedef enum
{
    RF_EVT_MODE = 0,
    RF_EVT_GO,
    RF_EVT_CHANNEL,
    RF_EVT_RX_RESET_CNTS,
    RF_EVT_RX_CNTS,
    RF_EVT_WIFI_RX,
    RF_EVT_WIFI_TX,

    RF_EVT_SHOW_SCA,
    RF_EVT_SET_SCA,
    RF_EVT_CAL_VCO,

    RF_EVT_BLE_DTM,
    RF_EVT_BLE_ADV,

    RF_EVT_IPC_ENABLE,

    RF_EVT_MAX
} T_RfCmdEvtType;

typedef struct
{
    uint32_t u32Type;
    int iArgc;
    char *saArgv[RF_CMD_PARAM_NUM];
} T_RfCmd;

typedef struct
{
    uint32_t u32Type;
    uint8_t u8Status;
    uint8_t u8IpcEnable;
    uint8_t u8Unicast;
    uint8_t u8Reserved;
    uint32_t u32Mode;
    uint32_t u32RfChannel;
    uint32_t u32Freq;
    uint16_t u16RfMode;

    // BLE
    uint16_t u16RxCnt;
    uint16_t u16RxCrcOkCnt;
    uint8_t u8Pkt;
    uint8_t u8Len;
    uint8_t u8Freq;

    void *pParam;
} T_RfEvt;


extern osTimerId wifi_cmd_tout_timer;
extern uint32_t g_cmd_retry;
extern osTimerId g_tCtrlWifiPsTimer;

int controller_wifi_init_patch(void *pScanResult);
void CtrlWifi_PsTout(void const *arg);

int CtrlWifi_PsStateForce_impl(WifiSta_PSForceMode_t mode, uint32_t timeout);

typedef int (*CtrlWifi_PsStateForce_fp_t)(WifiSta_PSForceMode_t mode, uint32_t timeout);

extern CtrlWifi_PsStateForce_fp_t CtrlWifi_PsStateForce;

#endif  //__CONTROLLER_WIFI_PATCH_H__
