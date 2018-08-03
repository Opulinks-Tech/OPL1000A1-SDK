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
#ifndef _MW_FIM_DEFAULT_GROUP01_PATCH_H_
#define _MW_FIM_DEFAULT_GROUP01_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include "mw_fim.h"
#include "mw_fim_default_group01.h"
#include "msg_patch.h"
#include "sys_common_ctrl.h"
#include "rf_cfg.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the file ID
// xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx
// ^^^^ ^^^^ Zone (0~3)
//           ^^^^ ^^^^ Group (0~8), 0 is reserved for swap
//                     ^^^^ ^^^^ ^^^^ ^^^^ File ID, start from 0
typedef enum
{
    MW_FIM_IDX_GP01_PATCH_START = 0x00010000,             // the start IDX of group 01
    MW_FIM_IDX_GP01_PATCH_UART_CFG,
    
    MW_FIM_IDX_GP01_TRACER_CFG,
    MW_FIM_IDX_GP01_TRACER_INT_TASK_INFO,
    MW_FIM_IDX_GP01_TRACER_EXT_TASK_INFO,
    
    MW_FIM_IDX_GP01_MAC_ADDR_WIFI_STA_SRC,
    MW_FIM_IDX_GP01_MAC_ADDR_WIFI_SOFTAP_SRC,
    MW_FIM_IDX_GP01_MAC_ADDR_BLE_SRC,

    MW_FIM_IDX_GP01_RF_CFG,
    
    MW_FIM_IDX_GP01_PATCH_MAX
} E_MwFimIdxGroup01_Patch;


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list
#define MW_FIM_TRACER_CFG_SIZE              sizeof(T_TracerCfg)
#define MW_FIM_TRACER_CFG_NUM               1

#define MW_FIM_TRACER_INT_TASK_INFO_SIZE    sizeof(T_TracerTaskInfoExt)
#define MW_FIM_TRACER_INT_TASK_INFO_NUM     TRACER_INT_TASK_NUM_MAX

#define MW_FIM_TRACER_EXT_TASK_INFO_SIZE    sizeof(T_TracerTaskInfoExt)
#define MW_FIM_TRACER_EXT_TASK_INFO_NUM     TRACER_EXT_TASK_NUM_MAX

#define MW_FIM_MAC_ADDR_SRC_WIFI_STA_SIZE     1
#define MW_FIM_MAC_ADDR_SRC_WIFI_STA_NUM      1

#define MW_FIM_MAC_ADDR_SRC_WIFI_SOFT_AP_SIZE 1
#define MW_FIM_MAC_ADDR_SRC_WIFI_SOFTAP_NUM   1

#define MW_FIM_MAC_ADDR_SRC_WIFI_BLE_SIZE     1
#define MW_FIM_MAC_ADDR_SRC_WIFI_BLE_NUM      1

#define MW_FIM_RF_CFG_SIZE                  sizeof(T_RfCfg)
#define MW_FIM_RF_CFG_NUM                   1

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern const T_MwFimFileInfo g_taMwFimGroupTable01_patch[];

extern const uint8_t g_tMwFimDefaultMacAddrWifiSTASrc;
extern const uint8_t g_tMwFimDefaultMacAddrWifiSoftAPSrc;
extern const uint8_t g_tMwFimDefaultMacAddrBleSrc;

// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _MW_FIM_DEFAULT_GROUP01_PATCH_H_
