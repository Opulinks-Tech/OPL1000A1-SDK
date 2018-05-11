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

/**
 * @file at_cmd_data_process.c
 * @author Michael Liao
 * @date 21 Mar 2018
 * @brief File contains the AT Command's data processing behavior.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include "nl1000.h"
#include "os.h"
#include "at_cmd_data_process.h"
#include "at_cmd.h"
#include "at_cmd_common.h"
#include "at_cmd_patch.h"
#include "at_cmd_common_patch.h"
#include "ble_cmd_app_cmd.h"

RET_DATA int g_at_lock;
RET_DATA int g_at_ble_data_len;
RET_DATA int g_at_tcpip_data_len;

extern uint8_t at_state;

extern _at_command_t *_g_AtCmdTbl_Wifi_Ptr;
extern _at_command_t *_g_AtCmdTbl_Tcpip_Ptr;
extern _at_command_t *_g_AtCmdTbl_Sys_Ptr;

void data_process_init(void)
{
    g_at_lock = LOCK_NONE;
    g_at_ble_data_len = 0;
    g_at_tcpip_data_len = 0;
    at_state = AT_STATE_IDLE;
}

void data_process_lock(int module, int data_len)
{
    switch(module)
    {
        case LOCK_BLE:
            g_at_lock = LOCK_BLE;
            g_at_ble_data_len = data_len;
            at_state = AT_STATE_SENDING_RECV;
            break;

        case LOCK_TCPIP:
            g_at_lock = LOCK_TCPIP;
            g_at_tcpip_data_len = data_len;
            at_state = AT_STATE_SENDING_RECV;
            break;

        case LOCK_NONE:
            data_process_init();
            break;

        default:
            g_at_lock = LOCK_NONE;
            break;
    }
}

void data_process_unlock(void)
{
    data_process_init();
}

int data_process_lock_get(void)
{
    return g_at_lock;
}

int data_process_data_len_get(void)
{
    int len = 0;

    switch(g_at_lock)
    {
        case LOCK_BLE:
            len = g_at_ble_data_len;
            break;

        case LOCK_TCPIP:
            len = g_at_tcpip_data_len;
            break;

        default:
            break;
    }

    return len;
}

int data_process_wifi_parse(char *pbuf, int len, int mode)
{
    const _at_command_t *cmd_ptr = NULL;

    if(pbuf == 0) return false;

    for(cmd_ptr=_g_AtCmdTbl_Wifi_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(strncasecmp(pbuf, cmd_ptr->cmd, strlen(cmd_ptr->cmd))) continue;
        msg_print_uart1("\r\n");
        cmd_ptr->cmd_handle(pbuf, len, mode);
        return true;
    }

	return false;
}

int data_process_wifi(char *pbuf, int len, int mode)
{
    int ret = false;
    ret = data_process_wifi_parse(pbuf, len, mode);
    return ret;
}

int data_process_ble(char *pbuf, int len, int mode)
{
    //1. Check ble table, if it's ble command, return true; else return false;

    //2. Find the specified command handler, do it

    //Be noticed, as to BLE case, the pbuf is transfered to BLE protocol stack, and the command will be handled at that time in BLE statck

	return LeCmdAppProcessAtCmd(pbuf, len);

    //return false;
}

int data_process_tcpip(char *pbuf, int len, int mode)
{
    const _at_command_t *cmd_ptr = NULL;

    if(pbuf == 0) return false;

    for(cmd_ptr=_g_AtCmdTbl_Tcpip_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(strncasecmp(pbuf, cmd_ptr->cmd, strlen(cmd_ptr->cmd))) continue;
        msg_print_uart1("\r\n");
        cmd_ptr->cmd_handle(pbuf, len, mode);
        return true;
    }

	return false;
}

int data_process_sys(char *pbuf, int len, int mode)
{
    const _at_command_t *cmd_ptr = NULL;

    if(pbuf == 0) return false;

    for(cmd_ptr=_g_AtCmdTbl_Sys_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(strncasecmp(pbuf, cmd_ptr->cmd, strlen(cmd_ptr->cmd))) continue;
        msg_print_uart1("\r\n");
        cmd_ptr->cmd_handle(pbuf, len, mode);
        return true;
    }

    return false;
}

int data_process_rf(char *pbuf, int len, int mode)
{
    //1. Check RF table, if it's ble command, return true; else return false;

    //2. Find the specified command handler, do it

    return false;
}

int data_process_pip(char *pbuf, int len, int mode)
{
    //1. Check PIP table, if it's ble command, return true; else return false;

    //2. Find the specified command handler, do it

    return false;
}

int data_process_others(char *pbuf, int len, int mode)
{
    //1. Check others table, if it's ble command, return true; else return false;

    //2. Find the specified command handler, do it

    return false;
}

int data_process_cmd_mode(char *pbuf)
{
    char *s = NULL;

    s = strstr(pbuf, PREFIX_AT_CMD_TEST);
    if(s != NULL)
    {
        return AT_CMD_MODE_TESTING;
    }

    s = strstr(pbuf, PREFIX_ATCMD_SET);
    if(s != NULL)
    {
        return AT_CMD_MODE_SET;
    }

    s = strstr(pbuf, PREFIX_AT_CMD_READ);
    if(s != NULL)
    {
        return AT_CMD_MODE_READ;
    }

    return AT_CMD_MODE_EXECUTION;
}

int data_process_handler(char *pbuf, int len)
{
    int mode = AT_CMD_MODE_INVALID;

    if (pbuf == NULL) return false;
    mode = data_process_cmd_mode(pbuf);

    if (len == 2) //Command: AT
    {
        msg_print_uart1("\r\nOK\r\n");
        return true;
    }

    if(mode == AT_CMD_MODE_EXECUTION)
    {
        if(strstr(pbuf, "ATE0") != NULL)
        {
            set_echo_on(false);
            msg_print_uart1("\r\nOK\r\n");
        }
        else if(strstr(pbuf, "ATE1") != NULL)
        {
            set_echo_on(true);
            msg_print_uart1("\r\nOK\r\n");
        }
    }

    if(g_at_lock == LOCK_NONE) //AT command input
    {
        if (data_process_wifi(pbuf, len, mode)) return true;
        if (data_process_ble(pbuf, len, mode)) return true;
        if (data_process_tcpip(pbuf, len, mode)) return true;
        if (data_process_sys(pbuf, len, mode)) return true;
        if (data_process_rf(pbuf, len, mode)) return true;
        if (data_process_pip(pbuf, len, mode)) return true;
        if (data_process_others(pbuf, len, mode)) return true;
    }

    return true;
}

void data_process_func_init(void)
{
    g_at_lock = LOCK_NONE;
    g_at_ble_data_len = 0;
    g_at_tcpip_data_len = 0;
}

