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
 * @file at_cmd_others_patch.c
 * @author Michael Liao
 * @date 20 Mar 2018
 * @brief File supports other modules AT Commands.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_others.h"
#include "at_cmd_common.h"
#include "at_cmd_others_patch.h"
#include "at_cmd_patch.h"

/*
 * @brief Command at+ir_tx_init
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_ir_tx_init(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ir_tx_down
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_ir_tx_down(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+at_cmd_other_ir_tx
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_ir_tx(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ir_rx_init
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_ir_rx_init(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ir_rx_down
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_ir_rx_down(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+common_buf_show
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_common_buf_show(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+temperature_on
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_temperature_on(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+temperature_off
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_temperature_off(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+at_cmd_other_secondary_uart_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_secondary_uart_start(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+secondary_uart_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_secondary_uart_stop(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+secondary_uart_write
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_other_secondary_uart_write(char *buf, int len, int mode)
{
    return true;
}

/**
  * @brief AT Command Table for Others Module
  *
  */
_at_command_t _gAtCmdTbl_Others[] =
{
    { "at+ir_tx_init",              _at_cmd_other_ir_tx_init,            "Open IR tx" },
    { "at+ir_tx_down",              _at_cmd_other_ir_tx_down,            "Close IR tx" },
    { "at+ir_tx",                   _at_cmd_other_ir_tx,                 "Do IR tx" },
    { "at+ir_rx_init",              _at_cmd_other_ir_rx_init,            "Open IR rx" },
    { "at+ir_rx_down",              _at_cmd_other_ir_rx_down,            "Close IR rx" },
    { "at+common_buf_show",         _at_cmd_other_common_buf_show,       "Show common buffer content" },
    { "at+temperature_on",          _at_cmd_other_temperature_on,        "On temperature monitoring" },
    { "at+temperature_off",         _at_cmd_other_temperature_off,       "Off temperature monitoring" },
    { "at+secondary_uart_start",    _at_cmd_other_secondary_uart_start,  "Start second UART interface" },
    { "at+secondary_uart_stop",     _at_cmd_other_secondary_uart_stop,   "Stop second UART interface" },
    { "at+secondary_uart_write",    _at_cmd_other_secondary_uart_write,  "Send data to second UART interface" },
    { NULL,                         NULL,                               NULL},
};

/*
 * @brief Global variable g_AtCmdTbl_Others_Ptr retention attribute segment
 *
 */
RET_DATA _at_command_t *_g_AtCmdTbl_Others_Ptr;

/*
 * @brief AT Command Interface Initialization for others modules
 *
 */
void _at_cmd_others_func_init(void)
{
    /** Command Table (Others) */
    _g_AtCmdTbl_Others_Ptr = _gAtCmdTbl_Others;
}

