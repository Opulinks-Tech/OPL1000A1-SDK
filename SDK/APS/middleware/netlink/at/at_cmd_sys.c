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
 * @file at_cmd_sys.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File supports the System module AT Commands.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_sys.h"
#include "at_cmd_common.h"

/*
 * @brief Command at
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_at(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+rst
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_rst(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+gmr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_gmr(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+gslp
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_gslp(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+restore
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_restore(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+uart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_uart(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+uartcur
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_uartcur(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+uartdefault
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_uartdefault(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+sleep
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_sleep(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ram
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_ram(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+reg
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_reg(int argc, char *argv[])
{
    /** at+reg=0x40009040 --> 2 */
    /** at+reg=0x40009040,0x3F4154 --> 3 */
    volatile uint32_t u32Value = 0;

    switch(argc)
    {
        case 2: /** Read */
            u32Value = reg_read(strtol(argv[1], NULL, 16));
            printf("%d\r\n", u32Value);
            msg_print_uart1("%d\r\n", u32Value);
            break;
        case 3: /** Write */
            reg_write(strtol(argv[1], NULL, 16), strtol(argv[2], NULL, 16));
            break;
        default:
            printf("UnKnow command");
            msg_print_uart1("UnKnow command");
            break;
    }

    printf("ok\r\n");
    msg_print_uart1("ok\r\n");

    return true;
}

/*
 * @brief Command at+start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+end
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_end(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+rsv
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_rsv(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+auto_recording_on
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_auto_recording_on(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+auto_recording_off
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_auto_recording_off(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+fwver
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_fwver(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mcupwrsave
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_mcupwrsave(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mcustandby
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_mcustandby(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ota_svr_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_ota_svr_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ota_svr_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_ota_svr_stop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief  Sample code to do system test
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_sys_sample(void)
{
    return true;
}

/**
  * @brief AT Command Table for System Module
  *
  */
at_command_t gAtCmdTbl_Sys[] =
{
    { "at",                     at_cmd_sys_at,            "Test AT startup" },
    { "at+rst",                 at_cmd_sys_rst,           "Restart module" },
    { "at+gmr",                 at_cmd_sys_gmr,           "View version info" },
    { "at+gslp",                at_cmd_sys_gslp,          "Enter deep-sleep mode" },
    { "at+restore",             at_cmd_sys_restore,       "Factory Reset" },
    { "at+uart",                at_cmd_sys_uart,          "UART configuration" },
    { "at+uart_cur",            at_cmd_sys_uartcur,       "UART current configuration show" },
    { "at+uart_default",        at_cmd_sys_uartdefault,   "UART default configuration, save to flash" },
    { "at+sleep",               at_cmd_sys_sleep,         "Sleep mode" },
    { "at+sysram",              at_cmd_sys_ram,           "RAM information" },
    { "at+reg",                 at_cmd_sys_reg,           "Register Access" },
    { "at+sysinit",             at_cmd_sys_start,         "Sys Start" },    //Back Door
    { "at+sysend",              at_cmd_sys_end,           "Sys End" },      //Back Door
    { "at+sysrsv",              at_cmd_sys_rsv,           "Sys Reserved" }, //Back Door
    { "at+auto_recording_on",   at_cmd_sys_auto_recording_on,  "Save Wi-Fi/BLE status" },
    { "at+auto_recording_off",  at_cmd_sys_auto_recording_off, "Turn off auto_recording" },
    { "at+fwver",               at_cmd_sys_fwver,         "Show FW Version" },
    { "at+mcupwrsave",          at_cmd_sys_mcupwrsave,    "Enter Power save mode" },
    { "at+mcustandby",          at_cmd_sys_mcustandby,    "Enter Standby mode" },
    { "at+ota_svr_start",       at_cmd_sys_ota_svr_start, "Open OTA FW Upgrade(HTTP) function" },
    { "at+ota_svr_stop",        at_cmd_sys_ota_svr_stop,  "Close OTA FW Upgrade(HTTP) function" },
    { NULL,                     NULL,                     NULL},
};

/*
 * @brief Global variable g_AtCmdTbl_Sys_Ptr retention attribute segment
 *
 */
RET_DATA at_command_t *g_AtCmdTbl_Sys_Ptr;

/*
 * @brief AT Command Interface Initialization for System modules
 *
 */
void at_cmd_sys_func_init(void)
{
    /** Command Table (System) */
    g_AtCmdTbl_Sys_Ptr = gAtCmdTbl_Sys;
}

