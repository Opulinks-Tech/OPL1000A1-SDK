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
 * @file at_cmd.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File creates the major architecture of AT Command parser.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "opl1000.h"
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_sys.h"
#include "at_cmd_wifi.h"
#include "at_cmd_ble.h"
#include "at_cmd_tcpip.h"
#include "at_cmd_rf.h"
#include "at_cmd_pip.h"
#include "at_cmd_others.h"
#include "le_ctrl.h"
#include "hal_uart.h"
#include "at_cmd_common.h"
#if defined(__AT_CMD_TASK__)
#include "at_cmd_task.h"
#endif
#include "at_cmd_data_process.h"

/*
 * @brief An external Function at_cmd_handler prototype declaration retention attribute segment
 *
 */
RET_DATA at_cmd_handler_fp_t at_cmd_handler;

/*
 * @brief An external Function at_cmd_parse prototype declaration retention attribute segment
 *
 */
RET_DATA at_cmd_parse_fp_t at_cmd_parse;

/*
 * @brief An external Function at_cmd_extend prototype declaration retention attribute segment
 *
 * For extending more commands
 */
RET_DATA at_cmd_extend_fp_t at_cmd_extend; /** For Future Extend Command Support */

/*
 * @brief An external global variable g_AtCmdTbl_Sys_Ptr prototype declaration
 *
 */
extern at_command_t *g_AtCmdTbl_Sys_Ptr;

/*
 * @brief An external global variable g_AtCmdTbl_Wifi_Ptr prototype declaration
 *
 */
extern at_command_t *g_AtCmdTbl_Wifi_Ptr;

/*
 * @brief An external global variable g_AtCmdTbl_Ble_Ptr prototype declaration
 *
 */
extern at_command_t *g_AtCmdTbl_Ble_Ptr;

/*
 * @brief An external global variable g_AtCmdTbl_Tcpip_Ptr prototype declaration
 *
 */
extern at_command_t *g_AtCmdTbl_Tcpip_Ptr;

/*
 * @brief An external global variable g_AtCmdTbl_Rf_Ptr prototype declaration
 *
 */
extern at_command_t *g_AtCmdTbl_Rf_Ptr;

/*
 * @brief An external global variable g_AtCmdTbl_Pip_Ptr prototype declaration
 *
 */
extern at_command_t *g_AtCmdTbl_Pip_Ptr;

/*
 * @brief An external global variable g_AtCmdTbl_Others_Ptr prototype declaration
 *
 */
extern at_command_t *g_AtCmdTbl_Others_Ptr;

#if 0
/*
 * @brief AT Commands Handler
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 */
int at_cmd_handler_impl(int argc, char *argv[])
{
    int isDefCmdFound = 0;
    const at_command_t *cmd_ptr;

    /** System */
    for(cmd_ptr=g_AtCmdTbl_Sys_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(!strcasecmp(argv[0], cmd_ptr->cmd))
        {
            isDefCmdFound = 1;
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(argc, argv);
            return true;
        }
    }

    /** Wi-Fi */
    for(cmd_ptr=g_AtCmdTbl_Wifi_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(!strcasecmp(argv[0], cmd_ptr->cmd))
        {
            isDefCmdFound = 1;
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(argc, argv);
            return true;
        }
    }

    /** BLE */
    for(cmd_ptr=g_AtCmdTbl_Ble_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(!strcasecmp(argv[0], cmd_ptr->cmd))
        {
            isDefCmdFound = 1;
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(argc, argv);
            return true;
        }
    }

    /** TCP/IP */
    for(cmd_ptr=g_AtCmdTbl_Tcpip_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(!strcasecmp(argv[0], cmd_ptr->cmd))
        {
            isDefCmdFound = 1;
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(argc, argv);
            return true;
        }
    }

    /** RF */
    for(cmd_ptr=g_AtCmdTbl_Rf_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(!strcasecmp(argv[0], cmd_ptr->cmd))
        {
            isDefCmdFound = 1;
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(argc, argv);
            return true;
        }
    }

    /** Perpherial IP */
    for(cmd_ptr=g_AtCmdTbl_Pip_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(!strcasecmp(argv[0], cmd_ptr->cmd))
        {
            isDefCmdFound = 1;
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(argc, argv);
            return true;
        }
    }

    /** Others */
    for(cmd_ptr=g_AtCmdTbl_Others_Ptr; cmd_ptr->cmd; cmd_ptr++)
    {
        if(!strcasecmp(argv[0], cmd_ptr->cmd))
        {
            isDefCmdFound = 1;
            msg_print_uart1("\r\n");
            cmd_ptr->cmd_handle(argc, argv);
            return true;
        }
    }

    if(isDefCmdFound == 0)
    {
        at_cmd_extend(argc, argv);
    }

    return true;
}

/*
 * @brief AT Commands Parser
 *
 * @param [in] pbuf the whole command string
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_parse_impl(char *pbuf)
{
    if(pbuf == 0) return 0;

    char *argv[AT_MAX_CMD_ARGS] = {0};
    int count = 0;
    char *p = NULL;

    /** Get the first word */
    p = strtok(pbuf, "=");
    argv[count] = p;
    //msg_print_uart1("\r\n at_cmd_parse, argv[%d]:%s ", count, argv[count]);
    count++;

	while ((p = strtok(NULL, ",")) != NULL)
    {
	    argv[count] = p;
        //msg_print_uart1("\r\n at_cmd_parse, argv[%d]:%s ", count, argv[count]);
        count++;
    }

    at_cmd_handler(count, argv);

	return true;
}
#endif //#if 0
/*
 * @brief AT Command Interface Initialization for AT Command module
 *
 */
void at_cmd_func_init(void)
{
#if 0
    /** Command Tables */
    at_cmd_sys_func_init();
    at_cmd_wifi_func_init();
    at_cmd_ble_func_init();
    at_cmd_tcpip_func_init();
    at_cmd_rf_func_init();
    at_cmd_pip_func_init();
    at_cmd_others_func_init();

    /** Functions */
    at_cmd_parse = at_cmd_parse_impl;
    at_cmd_handler = at_cmd_handler_impl;
    at_cmd_extend = at_cmd_extend_impl;
#endif
}


RET_DATA _at_cmd_parse_fp_t _at_cmd_parse;
RET_DATA _at_cmd_handler_fp_t _at_cmd_handler;
RET_DATA _at_cmd_extend_fp_t _at_cmd_extend;

extern _at_command_t *_g_AtCmdTbl_Sys_Ptr;
extern _at_command_t *_g_AtCmdTbl_Wifi_Ptr;
extern _at_command_t *_g_AtCmdTbl_Ble_Ptr;
extern _at_command_t *_g_AtCmdTbl_Tcpip_Ptr;
extern _at_command_t *_g_AtCmdTbl_Rf_Ptr;
extern _at_command_t *_g_AtCmdTbl_Pip_Ptr;
extern _at_command_t *_g_AtCmdTbl_Others_Ptr;

int _at_cmd_parse_impl(char *pbuf)
{
    _at_cmd_handler(pbuf, (int)strlen(pbuf), NULL);
	return true;
}

int _at_cmd_extend_impl(char *buf, int len, int mode)
{
    return true;
}

int _at_cmd_handler_impl(char *buf, int len, int mode)
{
    data_process_handler(buf, len);
    return true;
}

/*
 * @brief AT Command Interface Initialization for AT Command module
 *
 */
void _at_cmd_func_init(void)
{
    /** Command Tables */
    _at_cmd_sys_func_init();
    _at_cmd_wifi_func_init();
    _at_cmd_ble_func_init();
    _at_cmd_tcpip_func_init();
    _at_cmd_rf_func_init();
    _at_cmd_pip_func_init();
    _at_cmd_others_func_init();

    /** Functions */
    _at_cmd_parse = _at_cmd_parse_impl;
    _at_cmd_handler = _at_cmd_handler_impl;
    _at_cmd_extend = _at_cmd_extend_impl;
}

