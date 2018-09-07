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

#ifndef _SYS_COMMON_CTRL_H_
#define _SYS_COMMON_CTRL_H_

#include <stdint.h>
#include <stdbool.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BASE_NVM_MAC_SRC_IFACE_ID_STA = 0,
    BASE_NVM_MAC_SRC_IFACE_ID_SOFTAP,
    BASE_NVM_MAC_SRC_IFACE_ID_BLE,
    WIFI_NVM_MAC_SRC_IFACE_ID_MAX_NUM,
} base_nvm_mac_src_iface_id_t;

typedef enum {
    BASE_NVM_MAC_SRC_TYPE_ID_OTP = 0,
    BASE_NVM_MAC_SRC_TYPE_ID_FLASH,
    BASE_NVM_MAC_SRC_TYPE_ID_MAX_NUM,
} base_nvm_mac_src_type_id_t;

int base_mac_addr_src_get_cfg(u8 iface, u8 *type);
int base_mac_addr_src_set_cfg(u8 iface, u8 type);
int base_nvm_mac_addr_src_read(u16 id, u16 len, void *buf);
int base_nvm_mac_addr_src_write(u16 id, u16 len, void *ptr);
int get_rf_power_level(u8 *level);
int set_rf_power_level(u8 level);
int get_dhcp_arp_check(void);
int get_dhcp_arp_check_from_fim(void);
int set_dhcp_arp_check(u8 mode);


#ifdef __cplusplus
}
#endif

#endif /* _SYS_COMMON_CTRL_H_ */
