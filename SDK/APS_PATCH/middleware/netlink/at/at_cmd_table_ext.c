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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "at_cmd.h"
#include "at_cmd_sys.h"
#include "at_cmd_common.h"
#include "at_cmd_data_process.h"
#include "at_cmd_table_ext.h"
#include "common.h"
#include "sys_common_api.h"

int at_cmd_sys_mac_addr_def(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int ret;
    u8 ret_st = true;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    
    switch(mode) {
        case AT_CMD_MODE_READ:
        {
            u8 sta_type, ble_type;
            
            ret = mac_addr_get_config_source(MAC_IFACE_WIFI_STA, (mac_source_type_t *)&sta_type);
            if (ret != 0) {
                ret_st = false;
                goto done;
            }
            
            ret = mac_addr_get_config_source(MAC_IFACE_BLE, (mac_source_type_t *)&ble_type);
            if (ret != 0) {
                ret_st = false;
                goto done;
            }
            
            msg_print_uart1("\r\n+MACADDRDEF:%d,%d,%d,%d\r\n", MAC_IFACE_WIFI_STA, sta_type,
                             MAC_IFACE_BLE - 1, ble_type);
        }
            break;
        case AT_CMD_MODE_SET:
        {
            u8 iface, type;
            if (argc != 3) {
                ret_st = false;
                goto done;
            }
            
            iface = atoi(argv[1]);
            type = atoi(argv[2]);
            
            //Skip SoftAP mode since we have not support it.
            if (iface > MAC_IFACE_BLE - 1) {
                ret_st = false;
                goto done;
            }
            
            if (type > MAC_SOURCE_FROM_FLASH) {
                ret_st = false;
                goto done;
            }

            //AT command : 0 = STA, 1 = BLE
            if (iface == 0) {
                ret = mac_addr_set_config_source(MAC_IFACE_WIFI_STA, (mac_source_type_t)type);
            }
            else if (iface == 1) {
                ret = mac_addr_set_config_source(MAC_IFACE_BLE, (mac_source_type_t)type);
            }
            
            if (ret != 0) {
                ret_st = false;
                goto done;
            }
        }
            break;
        default:
            ret_st = false;
            break;
    }

done:
    if (ret_st)
        msg_print_uart1("\r\nOK\r\n");
    else 
        msg_print_uart1("\r\nError\r\n");
    
    return ret_st;
}

int at_cmd_sys_rf_hp(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int ret;
    u8 ret_st = true;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    
    switch(mode) {
        case AT_CMD_MODE_READ:
        {
            u8 level = 0;
            sys_get_config_rf_power_level((sys_rf_power_level_t *)&level);
            msg_print_uart1("\r\n+RFHP:%d\r\n", level);
        }
            break;
        case AT_CMD_MODE_SET:
        {
            u8 level = 0;
            
            if (argc != 2) {
                ret_st = false;
                goto done;
            }
            
            level = atoi(argv[1]);
            
            ret = sys_set_config_rf_power_level((sys_rf_power_level_t)level);
            if (ret != 0) {
                ret_st = false;
                goto done;
            }
        }
            break;
        default:
            ret_st = false;
            break;
    }

done:
    if (ret_st)
        msg_print_uart1("\r\nOK\r\n");
    else 
        msg_print_uart1("\r\nError\r\n");
    
    return ret_st;
}

/**
  * @brief extern AT Command Table for All Module
  *
  */
_at_command_t gAtCmdTbl_ext[] =
{
    { "at+macaddrdef",          at_cmd_sys_mac_addr_def,  "Default mac address from OTP or others storage" },
    { "at+rfhp",                at_cmd_sys_rf_hp,         "Set RF power"},
    { NULL,                     NULL,                     NULL},
};
