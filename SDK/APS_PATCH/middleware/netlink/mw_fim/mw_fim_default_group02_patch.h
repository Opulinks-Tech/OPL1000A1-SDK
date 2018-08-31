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
#ifndef _MW_FIM_DEFAULT_GROUP02_PATCH_H_
#define _MW_FIM_DEFAULT_GROUP02_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include "mw_fim.h"
#include "mw_fim_default_group02.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the file ID
// xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx
// ^^^^ ^^^^ Zone (0~3)
//           ^^^^ ^^^^ Group (0~8), 0 is reserved for swap
//                     ^^^^ ^^^^ ^^^^ ^^^^ File ID, start from 0
typedef enum
{
    MW_FIM_IDX_GP02_PATCH_START = 0x00020000,             // the start IDX of group 02
    MW_FIM_IDX_GP02_PATCH_WIFI_AUTO_CONN_MODE,
    MW_FIM_IDX_GP02_PATCH_WIFI_AUTO_CONN_CFG,
    MW_FIM_IDX_GP02_PATCH_WIFI_AUTO_CONN_AP_NUM,
    MW_FIM_IDX_GP02_PATCH_WIFI_AUTO_CONN_AP_INFO,
    MW_FIM_IDX_GP02_PATCH_STA_INFO_CFG,
    MW_FIM_IDX_GP02_PATCH_DEVICE_MANUF_NAME,
    MW_FIM_IDX_GP02_PATCH_STA_MAC_ADDR,
    MW_FIM_IDX_GP02_PATCH_STA_SKIP_DTIM,
    MW_FIM_IDX_DHCP_ARP_CHK,
    MW_FIM_IDX_GP02_PATCH_MAX
} E_MwFimIdxGroup02_Patch;


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list
#define MW_FIM_STA_MAC_ADDR_NUM          1
#define MW_FIM_STA_MAC_ADDR_SIZE         MAC_ADDR_LEN

#define MW_FIM_STA_SKIP_DTIM_NUM         1
#define MW_FIM_STA_SKIP_DTIM_SIZE        1

#define MW_FIM_DHCP_ARP_CHK_NUM          1
#define MW_FIM_DHCP_ARP_CHK_SIZE         1


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern const T_MwFimFileInfo g_taMwFimGroupTable02_patch[];
extern const uint8_t gMwFimDefaultWifiStaMacAddr[MAC_ADDR_LEN];
extern const uint8_t gMwFimDefaultwifiStaSkipDtim;

// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _MW_FIM_DEFAULT_GROUP02_PATCH_H_
