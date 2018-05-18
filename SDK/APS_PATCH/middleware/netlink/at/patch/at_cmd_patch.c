/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlnik Communication Corp. (C) 2017
******************************************************************************/
/**
 * @file at_cmd_patch.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File supports the patch code's implementation & modification.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "os.h"
#include "at_cmd_patch.h"
#include "at_cmd_common.h"
#include "at_cmd_common_patch.h"
#include "at_cmd_data_process.h"
#include "at_cmd_func_patch.h"
#include "at_cmd_wifi_patch.h"
#include "at_cmd_ble_patch.h"
#include "at_cmd_others_patch.h"
#include "at_cmd_pip_patch.h"
#include "at_cmd_rf_patch.h"
#include "at_cmd_sys_patch.h"
#include "at_cmd_tcpip_patch.h"
#include "at_cmd_msg_ext_patch.h"

/*
 * @brief Global variable g_AtCmdTbl_Sys_Ptr retention attribute segment
 *
 */
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

/** Sample to do patch */
#if 0
int at_cmd_parse_impl_v1(char *pbuf)
{
    printf("\r\n at_cmd_parse_impl_v1 \r\n");
	return 1;
}

int at_cmd_wifi_cwmode_v1(int argc, char *argv[])
{
    wpa_cli_scan_handler(argc, argv);
    printf("\r\nOK\r\n");
    return 1;
}

int at_cmd_wifi_cwscan_v1(int argc, char *argv[])
{
    printf("\r\n at_cmd_wifi_cwscan_v1 \r\n");
    wpa_cli_scan_handler(argc, argv);
    printf("\r\nOK\r\n");
    return 1;
}

at_command_t gAtCmdTbl_v1[] = //sample to re-define new cmd table
{
    { "at+cwscan",           at_cmd_wifi_cwscan,       "Wi-Fi Scan" },     //sample to call to rom's original api
    { "at+cwscanv1",         at_cmd_wifi_cwscan_v1,     "Wi-Fi Scan v1" }, //sample to call to new api
    { NULL,                  NULL,                     NULL},
};
#endif

/*
 * @brief AT Commands Handler
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 */
int at_cmd_handler_patch(int argc, char *argv[])
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

/*
 * @brief Interface Initialization for patch functions
 *
 */
void at_cmd_init_patch(void)
{
    at_cmd_common_func_init_patch();
    _at_func_init();
    _at_msg_ext_init();
}

