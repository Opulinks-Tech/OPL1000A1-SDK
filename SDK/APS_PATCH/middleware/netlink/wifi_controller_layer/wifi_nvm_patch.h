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

#include "msg.h"

#define wifi_nvm_printf   tracer_log

u16 wifi_nvm_sta_info_read(u16 id, u16 len, void *buf);
u16 wifi_nvm_sta_info_write(u16 id, u16 len, void *ptr);

#ifndef _WIFI_NVM_PATCH_H_
#define _WIFI_NVM_PATCH_H_
typedef enum {
    WIFI_NVM_STA_INFO_ID_MAC_ADDR,
    /* 1 ~ 29 reserved */
    WIFI_NVM_STA_INFO_ID_SKIP_DTIM = 30,
    /* for CBS use */
    WIFI_NVM_STA_INFO_MANUFACTURE_NAME = 50,
    WIFI_NVM_STA_INFO_ALL,
    WIFI_NVM_STA_INFO_ID_MAX_NUM,
} wifi_nvm_sta_info_cfg_id_e;
#endif /* _WIFI_NVM_PATCH_H_ */

