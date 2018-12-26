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
    
    ret = base_mac_addr_src_get_cfg(iface, (u8 *)type);
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

int sys_get_config_rf_power_level(uint8_t *level)
{
    int ret;
    
    if (level == NULL) {
        API_SYS_COMMON_LOGE("Invalid parameter.");
        return -1;
    }
    
    ret = get_rf_power_level((u8 *)level);
    if (ret != true) {
        API_SYS_COMMON_LOGE("Get rf power config failed.");
        return -1;
    }
    
    return 0;
}

int sys_set_config_rf_power_level(uint8_t level)
{
    int ret;

    ret = set_rf_power_level(level);
    if (ret != true) {
        API_SYS_COMMON_LOGE("Set rf power config failed.");
        return -1;
    }
    
    return 0;
}

int tcp_get_config_dhcp_arp_check(uint8_t *mode)
{
    if (mode == NULL) {
        API_SYS_COMMON_LOGE("Invalid parameter.");
        return -1;
    }
    
    *mode = get_dhcp_arp_check();
    return 0;
}

int tcp_set_config_dhcp_arp_check(uint8_t mode)
{
    if (mode > 1) {
        API_SYS_COMMON_LOGE("Invalid parameter.");
        return -1;
    }
    
    if (set_dhcp_arp_check(mode) != true) {
        return -1;
    }
    
    return 0;
}

int tcp_set_dhcp_interval_retry_times(uint8_t dhcp_mode,uint32_t dhcp_interval,uint8_t dhcp_retry_times)
{
    
    if (dhcp_interval ==0 || dhcp_retry_times==0 || dhcp_mode>1) {
        return -1;
    }
    
    set_dhcp_interval_retry_times(dhcp_mode,dhcp_interval,dhcp_retry_times);
        
    return 0;
}

int ble_set_config_bd_addr(uint8_t *bd_addr)
{
    set_ble_bd_addr(bd_addr);
    return 0;
}

int ble_get_config_bd_addr(uint8_t *bd_addr)
{
    get_ble_bd_addr(bd_addr);
    return 0;
}

