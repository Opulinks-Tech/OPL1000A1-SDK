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

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file


// Sec 1: Include File
#include "mw_fim_default_group02.h"
#include "mw_fim_default_group02_patch.h"


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
extern const uint32_t gMwFimDefaultAutoConnectMode;

/* Default value of Auto Connect AP number */
extern const uint32_t gMwFimDefaultAutoConnectAPNum;

/* Default value of Auto Connect Configuration */
extern const MwFimAutoConnectCFG_t gMwFimDefaultAutoConnectCfg;

extern const mw_wifi_auto_connect_ap_info_t gMwFimDefaultAutoConnectAPInfo;

/* Default value of Wifi Sta configuration */
// please refer to WifiSta_StaInfoInit()
extern const mw_wifi_sta_info_t gMwFimDefaultWifiStaCfg;

const uint8_t gMwFimDefaultWifiStaMacAddr[MAC_ADDR_LEN] = {
    0x22, 0x33, 0x44, 0x55, 0x66, 0x76
};

const uint8_t gMwFimDefaultwifiStaSkipDtim = 0;

extern const uint8_t gMwFimDefaultManufName[STA_INFO_MAX_MANUF_NAME_SIZE];

/* Auto Connection Mode */
extern uint32_t gMwFimAddrAutoConnectMode[MW_FIM_AUTO_CONN_MODE_NUM];

/* Auto Connection Configuration */
extern uint32_t gMwFimAddrAutoConnectCfg[MW_FIM_IDX_WIFI_AUTO_CONN_CFG_NUM];

/* Auto Connection AP number */
extern uint32_t gMwFimAddrAutoConnectAPNum[MW_FIM_AUTO_CONN_AP_NUM];

/* Auto Connection Information */
extern uint32_t gMwFimAddrBufferFastConnectApInfo[MW_FIM_AUTO_CONN_INFO_NUM];

/* WIFI STA Information configuration */
extern uint32_t gMwFimAddrBufferWifiStaInfo[MW_FIM_STA_INFO_NUM];

static uint32_t gMwFimAddrWifiStaMacAddr[MW_FIM_STA_MAC_ADDR_NUM];

static uint32_t gMwFimAddrWifiStaSkipDtim[MW_FIM_STA_SKIP_DTIM_NUM];

/* For blewifi CBS store use */
extern uint32_t gMwFimAddrManufName[MW_FIM_DEVICE_MANUF_NAME_NUM];

/* Default value of DHCP ARP check */
const uint8_t gMwFimDefaultDhcpArpChk = true;

/* DHCP ARP check */
uint32_t gMwFimAddrDhcpArpChk[MW_FIM_DHCP_ARP_CHK_NUM];

/* MAC TX data rate */
const uint8_t gMwFimDefaultMacDataRate = 0xFF;
uint32_t gMwFimAddrMacDataRate[MW_FIM_MAC_TX_DATA_RATE_NUM];

// the information table of group 02
const T_MwFimFileInfo g_taMwFimGroupTable02_patch[] =
{
    {MW_FIM_IDX_WIFI_AUTO_CONN_MODE,      MW_FIM_AUTO_CONN_MODE_NUM,      MW_FIM_AUTO_COMM_MODE_SIZE,            (uint8_t*)&gMwFimDefaultAutoConnectMode,   gMwFimAddrAutoConnectMode},
    {MW_FIM_IDX_WIFI_AUTO_CONN_CFG,       MW_FIM_IDX_WIFI_AUTO_CONN_CFG_NUM, MW_FIM_IDX_WIFI_AUTO_CONN_CFG_SIZE, (uint8_t*)&gMwFimDefaultAutoConnectCfg,    gMwFimAddrAutoConnectCfg},
    {MW_FIM_IDX_WIFI_AUTO_CONN_AP_NUM,    MW_FIM_AUTO_CONN_AP_NUM,        MW_FIM_AUTO_COMM_AP_SIZE,              (uint8_t*)&gMwFimDefaultAutoConnectAPNum,  gMwFimAddrAutoConnectAPNum},
    {MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO,   MW_FIM_AUTO_CONN_INFO_NUM,      MW_FIM_AUTO_CONN_INFO_SIZE,            (uint8_t*)&gMwFimDefaultAutoConnectAPInfo, gMwFimAddrBufferFastConnectApInfo},
    {MW_FIM_IDX_STA_INFO_CFG,             MW_FIM_STA_INFO_NUM,            MW_FIM_STA_INFO_SIZE,                  (uint8_t*)&gMwFimDefaultWifiStaCfg,        gMwFimAddrBufferWifiStaInfo},
    {MW_FIM_IDX_DEVICE_MANUF_NAME,        MW_FIM_DEVICE_MANUF_NAME_NUM,   MW_FIM_DEVICE_MANUF_NAME_SIZE,         (uint8_t*)&gMwFimDefaultManufName,         gMwFimAddrManufName},
    {MW_FIM_IDX_GP02_PATCH_STA_MAC_ADDR,  MW_FIM_STA_MAC_ADDR_NUM,        MW_FIM_STA_MAC_ADDR_SIZE,              (uint8_t*)&gMwFimDefaultWifiStaMacAddr,    gMwFimAddrWifiStaMacAddr}, 
    {MW_FIM_IDX_GP02_PATCH_STA_SKIP_DTIM, MW_FIM_STA_SKIP_DTIM_NUM,       MW_FIM_STA_SKIP_DTIM_SIZE,             (uint8_t*)&gMwFimDefaultwifiStaSkipDtim,   gMwFimAddrWifiStaSkipDtim}, 
    {MW_FIM_IDX_DHCP_ARP_CHK,             MW_FIM_DHCP_ARP_CHK_NUM,        MW_FIM_DHCP_ARP_CHK_SIZE,              (uint8_t*)&gMwFimDefaultDhcpArpChk,        gMwFimAddrDhcpArpChk},
    {MW_FIM_IDX_MAC_TX_DATA_RATE,         MW_FIM_MAC_TX_DATA_RATE_NUM,    MW_FIM_MAC_TX_DATA_RATE_SIZE,          (uint8_t*)&gMwFimDefaultMacDataRate,       gMwFimAddrMacDataRate},
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
