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
#include "at_cmd_data_process.h"
#include "at_cmd.h"
#include "at_cmd_common.h"
//#include "le_cmd_app_cmd.h"
//#include "le_host_test_cmd.h"
#include "at_cmd_ble.h"
#include "at_cmd_task.h"
//#include "le_cmd_app_cmd.h"
#include "at_cmd_ble_patch.h"

typedef bool (*T_LeHostProcessHostTestCmdFp)(char* pszData, int dataLen);

extern bool LeCmdAppProcessAtCmd(char *pbuf, int len);
extern T_LeHostProcessHostTestCmdFp LeHostProcessHostTestCmd;

/*
 * @brief Global variable gp_at_cmd_ext_table retention attribute segment
 *
 */
RET_DATA _at_command_t *gp_at_cmd_ext_table;

extern int g_at_lock;
extern int g_at_ble_data_len;
extern int g_at_tcpip_data_len;

extern uint8_t at_state;

extern _at_command_t *_g_AtCmdTbl_Wifi_Ptr;
extern _at_command_t *_g_AtCmdTbl_Tcpip_Ptr;
extern _at_command_t *_g_AtCmdTbl_Sys_Ptr;
extern _at_command_t *_g_AtCmdTbl_Rf_Ptr;

uint8_t cmd_info_buf[AT_RBUF_SIZE];

typedef struct {
    uint8_t *cmd;
    uint8_t  cmd_len;
    uint8_t  op;
    uint8_t  para_num;
    uint8_t *para[AT_MAX_CMD_ARGS];
} at_cmd_information_t;

static at_cmd_information_t at_cmd_info;

int data_process_wifi_patch(char *pbuf, int len, int mode)
{
    const _at_command_t *cmd_ptr = NULL;

    if(pbuf == 0) return false;

    for(cmd_ptr=_g_AtCmdTbl_Wifi_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(strcasecmp((char*)at_cmd_info.cmd, cmd_ptr->cmd) == 0)
        {
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(pbuf, len, mode);
            return true;
        }
    }

	return false;
}

int data_process_ble_patch(char *pbuf, int len, int mode)
{
    int ret;
    //1. Check ble table, if it's ble command, return true; else return false;
    if (strncasecmp(pbuf, "at+blemode", strlen("at+blemode"))==0)
    {
        msg_print_uart1("\r\nOK\r\n");
        _at_cmd_ble_mode(pbuf, len, mode);
        return true;
    }
    else if (strncasecmp(pbuf, "at+letest", strlen("at+letest"))==0)
    {
        msg_print_uart1("\r\nOK\r\n");
        _at_cmd_letest(pbuf, len, mode);
        return true;
    }
    //2. Find the specified command handler, do it

    //Be noticed, as to BLE case, the pbuf is transfered to BLE protocol stack, and the command will be handled at that time in BLE statck

	ret = LeCmdAppProcessAtCmd(pbuf, len);

	if (!ret)
	{
		ret = LeHostProcessHostTestCmd(pbuf, len);
    }

	return ret;

    //return false;
}

int data_process_tcpip_patch(char *pbuf, int len, int mode)
{
    const _at_command_t *cmd_ptr = NULL;

    if(pbuf == 0) return false;

    for(cmd_ptr=_g_AtCmdTbl_Tcpip_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(strcasecmp((char*)at_cmd_info.cmd, cmd_ptr->cmd) == 0)
        {
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(pbuf, len, mode);
            return true;
        }
    }

	return false;
}

int data_process_sys_patch(char *pbuf, int len, int mode)
{
    const _at_command_t *cmd_ptr = NULL;

    if(pbuf == 0) return false;

    for(cmd_ptr=_g_AtCmdTbl_Sys_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(strcasecmp((char*)at_cmd_info.cmd, cmd_ptr->cmd) == 0)
        {
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(pbuf, len, mode);
            return true;
        }
    }

    return false;
}

int data_process_rf_patch(char *pbuf, int len, int mode)
{
    const _at_command_t *cmd_ptr = NULL;

    if(pbuf == 0) return false;

    for(cmd_ptr=_g_AtCmdTbl_Rf_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(strcasecmp((char*)at_cmd_info.cmd, cmd_ptr->cmd) == 0)
        {
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(pbuf, len, mode);
            return true;
        }
    }

    return false;
}

int data_process_pip_patch(char *pbuf, int len, int mode)
{
    //1. Check PIP table, if it's ble command, return true; else return false;

    //2. Find the specified command handler, do it

    return false;
}

int data_process_others_patch(char *pbuf, int len, int mode)
{
    //1. Check others table, if it's ble command, return true; else return false;

    //2. Find the specified command handler, do it

    return false;
}

int data_process_extend_func(char *pbuf, int len, int mode)
{
    const _at_command_t *cmd_ptr = NULL;

    if(pbuf == 0) return false;

    for(cmd_ptr=gp_at_cmd_ext_table; cmd_ptr->cmd; cmd_ptr++)
    {
        if(strcasecmp((char*)at_cmd_info.cmd, cmd_ptr->cmd) == 0)
        {
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(pbuf, len, mode);
            return true;
        }
    }

    return false;
}

bool at_cmd_info_parsing(uint8_t *pStr, at_cmd_information_t *at_info_ptr)
{
    // first two letter must be "AT" or "at" or "At" or "aT"
    if ((at_info_ptr == NULL) || (pStr == NULL) || (at_strlen((char *)pStr) < 2)) {
        return false;
    }
    at_memset(at_info_ptr, 0x00, sizeof(at_cmd_information_t));
    at_info_ptr->op = AT_CMD_MODE_INVALID;

    if (((*pStr != 'A') && (*pStr != 'a')) || ((*(pStr + 1) != 'T') && (*(pStr + 1) != 't'))) {
        at_uart1_printf("AT CMD NOT FOUND AT\r\n");
        return false;
    }
    at_info_ptr->cmd = pStr;
    at_info_ptr->cmd_len = 2;
    pStr += 2;

    if (*pStr == '\0') {
        // just AT
        at_info_ptr->para_num = 0;
        at_info_ptr->op = AT_CMD_MODE_EXECUTION;
        return true;
    }

    // get cmd
    if (*pStr == '+') {
        pStr++;
        at_info_ptr->cmd_len++;
    }

    while (*pStr != '\0') {
        if ((*pStr >= 'A') && (*pStr <= 'Z')) { // OK
            pStr++;
        } else if ((*pStr >= 'a') && (*pStr <= 'z')) { // upper letter
            *pStr = (*pStr) - 'a' + 'A';
            pStr++;
        } else if ((*pStr >= '0') && (*pStr <= '9')) {
            pStr++;
        } else if ((*pStr == '!') || (*pStr == '%') || (*pStr == '-') || (*pStr == '.')
                   || (*pStr == '/') || (*pStr == ':') || (*pStr == '_')) {
            pStr++;
        } else {
            break;
        }
        at_info_ptr->cmd_len++;
    }
    // get operator
    if (*pStr == '?') {
        *pStr++ = '\0';
        at_info_ptr->op = AT_CMD_MODE_READ;
        goto PARSE_END;
    } else if (*pStr == '=') {
        *pStr++ = '\0';
        if (*pStr == '?') {
            *pStr++ = '\0';
            at_info_ptr->op = AT_CMD_MODE_TESTING;
            goto PARSE_END;
        }
        at_info_ptr->op = AT_CMD_MODE_SET;
    } else {
        goto PARSE_END;
    }
    // get parameter,like AT+CMD='abc'
    at_info_ptr->para_num = 0;
    at_info_ptr->para[at_info_ptr->para_num++] = pStr;
    while (*pStr != '\0') {
        if (*pStr == '\\') {
            pStr += 2;
            continue;
        }

        if (*pStr == ',') {
            *pStr++ = '\0';
            at_info_ptr->para[at_info_ptr->para_num++] = pStr;
            continue;
        }
        pStr++;
    }

PARSE_END:
    if (*pStr == '\0') {
        if (at_info_ptr->op == AT_CMD_MODE_INVALID) {
            at_info_ptr->op = AT_CMD_MODE_EXECUTION;
            *pStr = '\0';
        } else if (at_info_ptr->op == AT_CMD_MODE_SET) {
            *pStr = '\0';
        }
        return true;
    } else {
        at_uart1_printf("AT CMD ERROR\r\n");
    }

    return false;
}

int data_process_handler_impl(char *pbuf, int len)
{
    int mode = AT_CMD_MODE_INVALID;

    if (pbuf == NULL) return false;
    mode = data_process_cmd_mode(pbuf);

    memset(cmd_info_buf, 0, AT_RBUF_SIZE);
    memcpy(cmd_info_buf, pbuf, AT_RBUF_SIZE);

    at_cmd_info_parsing(cmd_info_buf, &at_cmd_info);

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
        if (data_process_wifi_patch(pbuf, len, mode))
            return true;
        if (data_process_ble_patch(pbuf, len, mode))
            return true;
        if (data_process_tcpip_patch(pbuf, len, mode))
            return true;
        if (data_process_sys_patch(pbuf, len, mode))
            return true;
        if (data_process_rf_patch(pbuf, len, mode))
            return true;
        if (data_process_pip_patch(pbuf, len, mode))
            return true;
        if (data_process_others_patch(pbuf, len, mode))
            return true;
        if (data_process_extend_func(pbuf, len, mode))
            return true;
        
        at_response_result(AT_RESULT_CODE_ERROR);
    }

    return true;
}

void data_process_func_init_patch(void)
{
    g_at_lock = LOCK_NONE;
    g_at_ble_data_len = 0;
    g_at_tcpip_data_len = 0;
}

