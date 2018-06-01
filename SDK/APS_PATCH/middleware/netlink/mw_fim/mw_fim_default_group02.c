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

/******************************************************************************
*  Filename:
*  ---------
*  mw_fim_default_group02.c
*
*  Project:
*  --------
*  NL1000 Project - the Flash Item Management (FIM) implement file
*
*  Description:
*  ------------
*  This implement file is include the Flash Item Management (FIM) function and api.
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file


// Sec 1: Include File
#include "ipc.h"
#include "mw_fim_default_group02.h"
#include "controller_wifi_com_patch.h"
#include "wifi_mac_sta_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable

/* Default value of Auto Connect Mode */
const uint32_t gMwFimDefaultAutoConnectMode = true;

/* Default value of Auto Connect AP number */
const uint32_t gMwFimDefaultAutoConnectAPNum = 0;

/* Default value of Auto Connect Configuration */
const MwFimAutoConnectCFG_t gMwFimDefaultAutoConnectCfg = {
    .front = -1,
    .rear = -1,
    .flag = false,
    .targetIdx = 0,
    .max_save_num = MAX_NUM_OF_AUTO_CONNECT,
};

const mw_wifi_auto_connect_ap_info_t gMwFimDefaultAutoConnectAPInfo = {
    .free_ocpy = 0,
    .bssid = {0},
    .ap_channel = 0,
    .latest_beacon_rx_time = 0,
    .ssid = {0},
    .supported_rates = {0},
    .rssi = 0,
    .beacon_interval = 0,
    .capabilities = 0,
    .dtim_prod = 0,
    .wpa_data = {0},
    .rsn_ie = {0},
    .wpa_ie = {0},
    .passphrase = {0},
    .hid_ssid = {0},
    .psk = {0},
    .fast_connect = false,
};

/* Default value of Wifi Sta configuration */
// please refer to WifiSta_StaInfoInit()
const uint8_t gMwFimDefaultWifiStaMacAddr[MAC_ADDR_LEN] = { //[0000526]
    0x22, 0x33, 0x44, 0x55, 0x66, 0x76
};

const uint8_t gMwFimDefaultwifiStaSkipDtim = 0; //[0000526]

const uint8_t gMwFimDefaultManufName[STA_INFO_MAX_MANUF_NAME_SIZE] = {
     /* C.B.S XXX, encoding GB2312 */
     0x43, 0x2E, 0x42, 0x2E, 0x53, 0x20, 
     0xB4, 0xB4,
     0xB2, 0xA9,
     0xCA, 0xC0,
};

/* Auto Connection Mode */
static uint32_t gMwFimAddrAutoConnectMode[MW_FIM_AUTO_CONN_MODE_NUM];

/* Auto Connection Configuration */
static uint32_t gMwFimAddrAutoConnectCfg[MW_FIM_IDX_WIFI_AUTO_CONN_CFG_NUM];

/* Auto Connection AP number */
static uint32_t gMwFimAddrAutoConnectAPNum[MW_FIM_AUTO_CONN_AP_NUM];

/* Auto Connection Information */
static uint32_t gMwFimAddrBufferFastConnectApInfo[MW_FIM_AUTO_CONN_INFO_NUM];

/* WIFI STA Information configuration */
static uint32_t gMwFimAddrWifiStaMacAddr[MW_FIM_STA_MAC_ADDR_NUM]; //[0000526]

static uint32_t gMwFimAddrWifiStaSkipDtim[MW_FIM_STA_SKIP_DTIM_NUM]; //[0000526]


/* For blewifi CBS store use */
static uint32_t gMwFimAddrManufName[MW_FIM_DEVICE_MANUF_NAME_NUM];

// the information table of group 02
const T_MwFimFileInfo g_taMwFimGroupTable02[] =
{
    {MW_FIM_IDX_WIFI_AUTO_CONN_MODE,      MW_FIM_AUTO_CONN_MODE_NUM,      MW_FIM_AUTO_COMM_MODE_SIZE,            (uint8_t*)&gMwFimDefaultAutoConnectMode,   gMwFimAddrAutoConnectMode},
    {MW_FIM_IDX_WIFI_AUTO_CONN_CFG,       MW_FIM_IDX_WIFI_AUTO_CONN_CFG_NUM, MW_FIM_IDX_WIFI_AUTO_CONN_CFG_SIZE, (uint8_t*)&gMwFimDefaultAutoConnectCfg,    gMwFimAddrAutoConnectCfg},
    {MW_FIM_IDX_WIFI_AUTO_CONN_AP_NUM,    MW_FIM_AUTO_CONN_AP_NUM,        MW_FIM_AUTO_COMM_AP_SIZE,              (uint8_t*)&gMwFimDefaultAutoConnectAPNum,  gMwFimAddrAutoConnectAPNum},
    {MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO,   MW_FIM_AUTO_CONN_INFO_NUM,      MW_FIM_AUTO_CONN_INFO_SIZE,            (uint8_t*)&gMwFimDefaultAutoConnectAPInfo, gMwFimAddrBufferFastConnectApInfo},
    {MW_FIM_IDX_STA_MAC_ADDR,             MW_FIM_STA_MAC_ADDR_NUM,        MW_FIM_STA_MAC_ADDR_SIZE,              (uint8_t*)&gMwFimDefaultWifiStaMacAddr,    gMwFimAddrWifiStaMacAddr}, /* [0000526] */
    {MW_FIM_IDX_STA_SKIP_DTIM,            MW_FIM_STA_SKIP_DTIM_NUM,       MW_FIM_STA_SKIP_DTIM_SIZE,             (uint8_t*)&gMwFimDefaultwifiStaSkipDtim,   gMwFimAddrWifiStaSkipDtim}, /* [0000526] */
    {MW_FIM_IDX_DEVICE_MANUF_NAME,        MW_FIM_DEVICE_MANUF_NAME_NUM,   MW_FIM_DEVICE_MANUF_NAME_SIZE,         (uint8_t*)&gMwFimDefaultManufName,          gMwFimAddrManufName},
    // the end, don't modify and remove it
    {0xFFFFFFFF,            0x00,              0x00,               NULL,                            NULL}
};

// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


/***********
C Functions
***********/
// Sec 8: C Functions
