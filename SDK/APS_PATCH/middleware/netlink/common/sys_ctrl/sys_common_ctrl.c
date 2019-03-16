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

#include "common.h"
#include "sys_cfg.h"
#include "sys_common_ctrl.h"
#include "sys_common_log.h"
#include "mw_fim_default_group01_patch.h"
#include "mw_fim_default_group02_patch.h"
#include "mw_fim_default_group03_patch.h"
#include "wifi_nvm_patch.h"
#include "le_ctrl_patch.h"

extern int dhcp_does_arp_check_flag;
extern uint8_t dhcp_retry_mode;
extern int lwip_dhcp_autoip_coop_tries;
extern int dhcp_retry_interval;

int base_mac_addr_src_get_cfg(u8 iface, u8* type)
{
    u8 ret;
    
    if (iface > BASE_NVM_MAC_SRC_IFACE_ID_BLE) {
        SYS_COMMON_LOGE("Invalid parameters.");
        return -1;
    }
    
    ret = base_nvm_mac_addr_src_read(iface, 1, type);
    if (ret != true) {
        SYS_COMMON_LOGE("Get config of mac address source failed.");
        return -1;
    }
    
    return true;
}

int base_mac_addr_src_set_cfg(u8 iface, u8 type)
{
    u8 ret;
    
    if (iface > BASE_NVM_MAC_SRC_IFACE_ID_BLE || type > BASE_NVM_MAC_SRC_TYPE_ID_FLASH) {
        SYS_COMMON_LOGE("Invalid parameters.");
        return -1;
    }
    
    ret = base_nvm_mac_addr_src_write(iface, 1, &type);
    if (ret != true) {
        SYS_COMMON_LOGE("Set config of mac address source failed.");
        return -1;
    }
    
    return true;
}

int base_nvm_mac_addr_src_read(u16 id, u16 len, void *buf)
{
    u8 ret;
    
    if (buf == NULL) {
        SYS_COMMON_LOGE("Invalid parameters.");
        return false;
    }
    
    switch(id) {
        case BASE_NVM_MAC_SRC_IFACE_ID_STA:
            ret = MwFim_FileRead(MW_FIM_IDX_GP03_PATCH_MAC_ADDR_WIFI_STA_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_STA_SIZE, buf);
            if (ret != MW_FIM_OK) {
                memcpy(buf, &g_tMwFimDefaultMacAddrWifiSTASrc, MW_FIM_MAC_ADDR_SRC_WIFI_STA_SIZE);
                SYS_COMMON_LOGE("MwFim_FileRead mac address STA cfg failed");
                return false;
            }
            break;
        case BASE_NVM_MAC_SRC_IFACE_ID_SOFTAP:
            ret = MwFim_FileRead(MW_FIM_IDX_GP03_PATCH_MAC_ADDR_WIFI_SOFTAP_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_SOFT_AP_SIZE, buf);
            if (ret != MW_FIM_OK) {
                memcpy(buf, &g_tMwFimDefaultMacAddrWifiSoftAPSrc, MW_FIM_MAC_ADDR_SRC_WIFI_SOFT_AP_SIZE);
                SYS_COMMON_LOGE("MwFim_FileRead mac address SoftAP cfg failed");
                return false;
            }
            break;
        case BASE_NVM_MAC_SRC_IFACE_ID_BLE:
            ret = MwFim_FileRead(MW_FIM_IDX_GP03_PATCH_MAC_ADDR_BLE_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_BLE_SIZE, buf);
            if (ret != MW_FIM_OK) {
                memcpy(buf, &g_tMwFimDefaultMacAddrBleSrc, MW_FIM_MAC_ADDR_SRC_WIFI_BLE_SIZE);
                SYS_COMMON_LOGE("MwFim_FileRead mac address Ble cfg failed");
                return false;
            }
            break;
        default:
            break;
    }
    
    return true;
}

int base_nvm_mac_addr_src_write(u16 id, u16 len, void *ptr)
{
    u8 ret;
    
    if (ptr == NULL) {
        SYS_COMMON_LOGE("Invalid parameters.");
        return false;
    }
    
    switch(id) {
        case BASE_NVM_MAC_SRC_IFACE_ID_STA:
            ret = MwFim_FileWrite(MW_FIM_IDX_GP03_PATCH_MAC_ADDR_WIFI_STA_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_STA_SIZE, ptr);
            if (ret != MW_FIM_OK) {
                SYS_COMMON_LOGE("MwFim_FileWrite mac address STA cfg failed");
                return false;
            }  
            break;
        case BASE_NVM_MAC_SRC_IFACE_ID_SOFTAP:
            ret = MwFim_FileWrite(MW_FIM_IDX_GP03_PATCH_MAC_ADDR_WIFI_SOFTAP_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_SOFT_AP_SIZE, ptr);
            if (ret != MW_FIM_OK) {
                SYS_COMMON_LOGE("MwFim_FileWrite mac address SoftAP cfg failed");
                return false;
            }  
            break;
        case BASE_NVM_MAC_SRC_IFACE_ID_BLE:
            ret = MwFim_FileWrite(MW_FIM_IDX_GP03_PATCH_MAC_ADDR_BLE_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_BLE_SIZE, ptr);
            if (ret != MW_FIM_OK) {
                SYS_COMMON_LOGE("MwFim_FileWrite mac address Ble cfg failed");
                return false;
            }  
            break;   
        default:
            break;
    }
    
    return true;
}

int get_rf_power_level(u8 *level)
{
    int ret;
    T_RfCfg rf_cfg = {0};
    
    if (level == NULL) {
        SYS_COMMON_LOGE("Invalid parameters.");
        return -1;
    }
    
    ret = sys_cfg_rf_get(&rf_cfg);
    if (ret) {
        SYS_COMMON_LOGE("Get config of rf power failed.");
        return -1;
    }
    
    *level = rf_cfg.u8HighPwrStatus;
    
    return true;
}

int set_rf_power_level(u8 level)
{
    int ret;
    T_RfCfg rf_cfg = {0};
    
    ret = sys_cfg_rf_get(&rf_cfg);
    if (ret) {
        SYS_COMMON_LOGE("Get config of rf power failed.");
        return -1;
    }

    rf_cfg.u8HighPwrStatus = level;
    
    ret = sys_cfg_rf_set(&rf_cfg, 1);
    if (ret) {
        SYS_COMMON_LOGE("Set config of rf power failed.");
        return -1;
    }
    
    return true;
}

int get_dhcp_arp_check(void)
{
    return dhcp_does_arp_check_flag;
}

int get_dhcp_arp_check_from_fim(void)
{
    u8 ret;
    u8 dhcp_arp;
    ret = MwFim_FileRead(MW_FIM_IDX_DHCP_ARP_CHK, 0, MW_FIM_DHCP_ARP_CHK_SIZE, &dhcp_arp);
    if (ret != MW_FIM_OK) {
        SYS_COMMON_LOGE("Read DHCP ARP check mode failed.");
        return -1;
    }
    return dhcp_arp;
}

int set_dhcp_arp_check(u8 mode)
{
    u8 ret;
    
    if (mode > 1) {
        return -1;
    }

    ret = MwFim_FileWrite(MW_FIM_IDX_DHCP_ARP_CHK, 0, MW_FIM_DHCP_ARP_CHK_SIZE, &mode);
    if (ret != MW_FIM_OK) {
        SYS_COMMON_LOGE("Write DHCP ARP check mode failed.");
        return -1;
    }
    
    dhcp_does_arp_check_flag = mode;
    
    return true;
}

int set_dhcp_interval_retry_times(u8 dhcp_mode,u32 dhcp_interval,u8 dhcp_retry_times)
{
    
    if (dhcp_interval ==0 || dhcp_retry_times==0 || dhcp_mode>1) {
        return -1;
    }
    dhcp_retry_mode = dhcp_mode;
    dhcp_retry_interval = dhcp_interval;
    lwip_dhcp_autoip_coop_tries = dhcp_retry_times;
        
    return true;
}

int set_ble_bd_addr(uint8_t *addr)
{
    le_write_bd_addr_to_flash(addr);
    return true;
}

int get_ble_bd_addr(uint8_t *addr)
{
    extern le_cfg_patch_t g_LeCfg;
    memcpy(addr, &g_LeCfg.bd_addr, 6);
    
    return true;
}
