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
#include "wifi_nvm_patch.h"
#include "sys_common_ctrl.h"
#include "sys_common_log.h"
#include "mw_fim_default_group01.h"

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
            ret = MwFim_FileRead(MW_FIM_IDX_GP01_MAC_ADDR_WIFI_STA_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_STA_SIZE, buf);
            if (ret != MW_FIM_OK) {
                memcpy(buf, &g_tMwFimDefaultMacAddrWifiSTASrc, MW_FIM_MAC_ADDR_SRC_WIFI_STA_SIZE);
                SYS_COMMON_LOGE("MwFim_FileRead mac address STA cfg failed");
                return false;
            }
            break;
        case BASE_NVM_MAC_SRC_IFACE_ID_SOFTAP:
            ret = MwFim_FileRead(MW_FIM_IDX_GP01_MAC_ADDR_WIFI_SOFTAP_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_SOFT_AP_SIZE, buf);
            if (ret != MW_FIM_OK) {
                memcpy(buf, &g_tMwFimDefaultMacAddrWifiSoftAPSrc, MW_FIM_MAC_ADDR_SRC_WIFI_SOFT_AP_SIZE);
                SYS_COMMON_LOGE("MwFim_FileRead mac address SoftAP cfg failed");
                return false;
            }
            break;
        case BASE_NVM_MAC_SRC_IFACE_ID_BLE:
            ret = MwFim_FileRead(MW_FIM_IDX_GP01_MAC_ADDR_BLE_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_BLE_SIZE, buf);
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
            ret = MwFim_FileWrite(MW_FIM_IDX_GP01_MAC_ADDR_WIFI_STA_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_STA_SIZE, ptr);
            if (ret != MW_FIM_OK) {
                SYS_COMMON_LOGE("MwFim_FileWrite mac address STA cfg failed");
                return false;
            }  
            break;
        case BASE_NVM_MAC_SRC_IFACE_ID_SOFTAP:
            ret = MwFim_FileWrite(MW_FIM_IDX_GP01_MAC_ADDR_WIFI_SOFTAP_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_SOFT_AP_SIZE, ptr);
            if (ret != MW_FIM_OK) {
                SYS_COMMON_LOGE("MwFim_FileWrite mac address SoftAP cfg failed");
                return false;
            }  
            break;
        case BASE_NVM_MAC_SRC_IFACE_ID_BLE:
            ret = MwFim_FileWrite(MW_FIM_IDX_GP01_MAC_ADDR_BLE_SRC, 0, MW_FIM_MAC_ADDR_SRC_WIFI_BLE_SIZE, ptr);
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
