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

#ifndef __CONTROLLER_WIFI_H__
#define __CONTROLLER_WIFI_H__

#include "controller_task.h"
#include "ieee80211_mac_header.h"
#include "wifi_mac_frame.h"
#include "controller_wifi_com.h"

#define REG_TRX_BASE            0X40003000
#define REG_TRX_WIFI_BSSID0     (REG_TRX_BASE + 0X10C)
#define REG_TRX_WIFI_BSSID1     (REG_TRX_BASE + 0X110)
#define REG_TRX_WIFI_OWN_MAC0   (REG_TRX_BASE + 0X118)
#define REG_TRX_WIFI_OWN_MAC1   (REG_TRX_BASE + 0X11C)

#define WIFI_SCAN_TOUT_TIME     10000   //10 seconds
#define WIFI_CONN_TOUT_TIME     8000    // Connection Timer, 8 seconds
#define WIFI_CMD_TOUT_TIME      8000    // Common Command Timer, 8 seconds

typedef enum
{
	TM_MODE,
	TM_WIFI_TX,
	TM_WIFI_RX,
	TM_RX_RESET_CNTS,
	TM_RX_COUNTERS,
	TM_CHANNEL,
	TM_GO,

}TestMode;

typedef struct
{
    u8 u8Trigger_wifi_tx;
    u8 u8media_access;
    u8 u8TestMode;
    u8 u8TXEnable;
    u8 u8RXEnable;
    u32 u32cmd_type;
    u32 u32Pramble;
    u32 u32DataLen;
    u32 u32Interval;
    u32 u32Channel;
    float fDataRate;
    
}rf_cmd_t;

typedef struct
{
    u8 u8Enable;
    u32 u32dbg_rx_uc_count;
    u32 u32dbg_rx_bc_count;
    u32 u32dbg_rx_er_count;
    u32 u32dbg_rssi;
    
}rf_rev_t;

typedef struct
{
    u32  cmd_type:8;
    u32  arg1:8;
    u32  reserved:16;
    void *prvData;
} wifi_cmd_t;

typedef struct
{
    uint32_t evt_type; /*mlme_evt_type_e*/
    void *prvData;
} wifi_evt_t;

extern scan_report_t gScanReport;

int controller_wifi_init_impl(void *pScanResult);
int controller_wifi_cmd_handler_impl(wifi_cmd_t * pWifiCmd);
int controller_wifi_evt_handler_impl(wifi_evt_t * pWifiEvt);

/* Rom patch interface */
typedef int (*controller_wifi_init_fp_t)(void *pScanResult);
typedef int (*controller_wifi_cmd_handler_fp_t)(wifi_cmd_t *pWifiCmd);
typedef int (*controller_wifi_evt_handler_fp_t)(wifi_evt_t *pWifiEvt);

/* Export interface funtion pointer */
extern controller_wifi_init_fp_t controller_wifi_init;
extern controller_wifi_cmd_handler_fp_t controller_wifi_cmd_handler;
extern controller_wifi_evt_handler_fp_t controller_wifi_evt_handler;

#endif  //__CONTROLLER_WIFI_H__

