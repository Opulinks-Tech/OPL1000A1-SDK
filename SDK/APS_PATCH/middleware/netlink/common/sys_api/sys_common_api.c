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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "sys_common_types.h"
#include "sys_common_api.h"
#include "sys_common_ctrl.h"

#define API_SYS_COMMON_LOGE(fmt,arg...)             printf(("E [API]: "fmt"\r\n"), ##arg)
#define API_SYS_COMMON_LOGI(fmt,arg...)             printf(("I [API]: "fmt"\r\n"), ##arg)

int mac_addr_get_config_source(mac_iface_t iface, mac_source_type_t *type)
{
    int ret;
    
    if (type == NULL) {
        API_SYS_COMMON_LOGE("The parameter is NULL.");
        return -1;
    }
    
    if (iface > MAC_IFACE_BLE) {
        API_SYS_COMMON_LOGE("Invalid parameter.");
        return -1;
    }
    
    ret = base_mac_addr_src_get_cfg(iface, type);
    if (ret != true) {
        API_SYS_COMMON_LOGE("Get mac address config failed.");
        return -1;
    }
    
    return 0;
}

int mac_addr_set_config_source(mac_iface_t iface, mac_source_type_t type)
{
    int ret;
    
    if (iface > MAC_IFACE_BLE) {
        API_SYS_COMMON_LOGE("Invalid parameter.");
        return -1;
    }
    
    if (type > MAC_SOURCE_FROM_FLASH) {
        API_SYS_COMMON_LOGE("Invalid parameter.");
        return -1;
    }
    
    ret = base_mac_addr_src_set_cfg(iface, type);
    if (ret != true) {
        API_SYS_COMMON_LOGE("Get mac address config failed.");
        return -1;
    }
    
    return 0;
}
