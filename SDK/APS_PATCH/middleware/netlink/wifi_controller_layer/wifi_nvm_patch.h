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

#ifndef __WIFI_NVM_PATCH_H__
#define __WIFI_NVM_PATCH_H__

#include "wifi_nvm.h"

typedef enum {
    //WIFI_NVM_STA_INFO_ID_MAC_ADDR,
    /* 1 ~ 29 reserved */
    //WIFI_NVM_STA_INFO_ID_SKIP_DTIM = 30,
    WIFI_NVM_MAC_TX_DATA_RATE = WIFI_NVM_STA_INFO_ID_SKIP_DTIM + 1,
    /* Device information for CBS use*/
    //WIFI_NVM_STA_INFO_MANUFACTURE_NAME = 50,
    //WIFI_NVM_STA_INFO_ALL,
    //WIFI_NVM_STA_INFO_ID_MAX_NUM,
} wifi_nvm_sta_info_cfg_id_ext_e;

void wifi_nvm_func_patch(void);
int wifi_nvm_sta_def_mac_get(uint8_t *mac);

#endif /* __WIFI_NVM_PATCH_H__ */
