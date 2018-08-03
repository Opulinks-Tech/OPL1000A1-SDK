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
#include "cmsis_os.h"
#include "at_cmd.h"
#include "at_cmd_sys.h"
#include "at_cmd_common.h"
#include "at_cmd_data_process.h"
#include "hal_tick.h"
#include "hal_system.h"
#include "hal_uart.h"
#include "hal_dbg_uart.h"
#include "svn_rev.h"
#include "mw_fim\mw_fim.h"
#include "mw_fim\mw_fim_default_group01.h"
#include "mw_fim\mw_fim_default_group02.h"
#include "ps.h"

#include "at_cmd_common_patch.h"
#include "at_cmd_sys_patch.h"
#include "mw_fim_default_group01_patch.h"
#include "mw_fim_default_group02_patch.h"
#include "at_cmd_sys.h"
#include "sys_common_api.h"
#include "sys_common_types.h"

#define CMD_TOKEN_SIZE          16
#define AT_CMD_SYS_WAIT_TIME    1000   // ms

extern void Cmd_TokenParse(char* pszData, uint8_t* pubCount, uint8_t* pubaToken[]);
extern void _at_cmd_sys_internal_wait(uint32_t u32MilliSec);

/*
 * @brief parse the config of UART
 *
 * @param [in] the input string
 *
 * @param [in] the length of input string
 *
 * @param [out] the uart configuration
 *
 * @return 0 fail 1 success
 *
 */
static uint8_t _at_cmd_sys_internal_uart_config_parse_patch(char *buf, int len, T_HalUartConfig *ptUartConfig)
{
    uint8_t ubCount;
    uint8_t* pubaToken[CMD_TOKEN_SIZE];
    uint32_t ulToken;

    // pre-parser the input string
    Cmd_TokenParse(buf, &ubCount, pubaToken);

    // AT+UART_CUR=<baudrate>,<databits>,<stopbits>,<parity>,<flow control>
    if (6 != ubCount)
        return 0;

    // baudrate
    ulToken = strtoul((const char*)pubaToken[1], NULL, 0);
    if ((ulToken < 80) || (ulToken > 1000000))
        return 0;
    ptUartConfig->ulBuadrate = ulToken;

    // databits
    ulToken = strtoul((const char*)pubaToken[2], NULL, 0);
    switch (ulToken)
    {
        case 5:
            ptUartConfig->ubDataBit = DATA_BIT_5;
            break;
        case 6:
            ptUartConfig->ubDataBit = DATA_BIT_6;
            break;
        case 7:
            ptUartConfig->ubDataBit = DATA_BIT_7;
            break;
        case 8:
            ptUartConfig->ubDataBit = DATA_BIT_8;
            break;

        default:
            return 0;
            //break;
    }

    // stopbits
    ulToken = strtoul((const char*)pubaToken[3], NULL, 0);
    switch (ulToken)
    {
        case 1:
            ptUartConfig->ubStopBit = STOP_BIT_1;
            break;
        // 1.5 stopbits is only for 5 databits
        case 2:
            ptUartConfig->ubStopBit = STOP_BIT_1P5;
            if (ptUartConfig->ubDataBit != DATA_BIT_5)
                return 0;
            break;
        case 3:
            ptUartConfig->ubStopBit = STOP_BIT_2;
            break;

        default:
            return 0;
            //break;
    }

    // parity
    ulToken = strtoul((const char*)pubaToken[4], NULL, 0);
    switch (ulToken)
    {
        case 0:
            ptUartConfig->ubParity = PARITY_NONE;
            break;
        case 1:
            ptUartConfig->ubParity = PARITY_ODD;
            break;
        case 2:
            ptUartConfig->ubParity = PARITY_EVEN;
            break;

        default:
            return 0;
            //break;
    }

    // flow control
    ulToken = strtoul((const char*)pubaToken[5], NULL, 0);
    switch (ulToken)
    {
        case 0:
            ptUartConfig->ubFlowCtrl = 0;
            break;
        case 3:
            ptUartConfig->ubFlowCtrl = 1;
            break;

        default:
            return 0;
            //break;
    }

    return 1;
}

/*
 * @brief output the config of UART
 *
 * @param [out] the uart configuration
 *
 * @return 0 fail 1 success
 *
 */
uint8_t _at_cmd_sys_internal_uart_config_output_patch(T_HalUartConfig *ptUartConfig, uint8_t ubCurrent)
{
    uint32_t ulBuadrate;
    uint8_t ubDataBit;
    uint8_t ubStopBit;
    uint8_t ubParity;
    uint8_t ubFlowCtrl;

    // +UART_DEF:<baudrate>,<databits>,<stopbits>,<parity>,<flow control>

    // baudrate
    ulBuadrate = ptUartConfig->ulBuadrate;

    // databits
    switch (ptUartConfig->ubDataBit)
    {
        case DATA_BIT_5:
            ubDataBit = 5;
            break;
        case DATA_BIT_6:
            ubDataBit = 6;
            break;
        case DATA_BIT_7:
            ubDataBit = 7;
            break;
        case DATA_BIT_8:
            ubDataBit = 8;
            break;

        default:
            return 0;
            //break;
    }

    // stopbits
    switch (ptUartConfig->ubStopBit)
    {
        case STOP_BIT_1:
            ubStopBit = 1;
            break;
        case STOP_BIT_1P5:
            ubStopBit = 2;
            break;
        case STOP_BIT_2:
            ubStopBit = 3;
            break;

        default:
            return 0;
            //break;
    }

    // parity
    switch (ptUartConfig->ubParity)
    {
        case PARITY_NONE:
            ubParity = 0;
            break;
        case PARITY_ODD:
            ubParity = 1;
            break;
        case PARITY_EVEN:
            ubParity = 2;
            break;

        default:
            return 0;
            //break;
    }

    // flow control
    switch (ptUartConfig->ubFlowCtrl)
    {
        case 0:
            ubFlowCtrl = 0;
            break;
        case 1:
            ubFlowCtrl = 3;
            break;

        default:
            return 0;
            //break;
    }

    msg_print_uart1("\r\n");

    if (ubCurrent == 1)
        msg_print_uart1("+UART_CUR:%u,%u,%u,%u,%u\r\n", ulBuadrate, ubDataBit, ubStopBit, ubParity, ubFlowCtrl);
    else
        msg_print_uart1("+UART_DEF:%u,%u,%u,%u,%u\r\n", ulBuadrate, ubDataBit, ubStopBit, ubParity, ubFlowCtrl);

    msg_print_uart1("\r\nOK\r\n");

    return 1;
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
int _at_cmd_sys_gmr_patch(char *buf, int len, int mode)
{
    if (AT_CMD_MODE_EXECUTION == mode)
    {
        msg_print_uart1("\r\n");

        msg_print_uart1("AT version info: %s \r\n", AT_VER_PATCH);
        msg_print_uart1("SDK version info: %d \r\n", SVN_REVISION);
        msg_print_uart1("Compile time: %s \r\n", SVN_TIME_NOW);

        msg_print_uart1("\r\nOK\r\n");
    }

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
int _at_cmd_sys_restore_patch(char *buf, int len, int mode)
{
    int i;
    
    if (AT_CMD_MODE_EXECUTION == mode)
    {
        // reset the config of UART0 / UART1
        MwFim_FileWriteDefault(MW_FIM_IDX_GP01_UART_CFG, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_GP01_UART_CFG, 1);

        // reset Auto connect configuartion and Info
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_MODE, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_NUM, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_CFG, 0);
        
        for(i=0; i<MAX_NUM_OF_AUTO_CONNECT; i++) {
            MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, i);
        }

        // rest STA information
        MwFim_FileWriteDefault(MW_FIM_IDX_GP02_PATCH_STA_MAC_ADDR, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_GP02_PATCH_STA_SKIP_DTIM, 0);
        
        // Mac address source
        MwFim_FileWriteDefault(MW_FIM_IDX_GP01_MAC_ADDR_WIFI_STA_SRC, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_GP01_MAC_ADDR_WIFI_SOFTAP_SRC, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_GP01_MAC_ADDR_BLE_SRC, 0);
        
        // RF power
        MwFim_FileWriteDefault(MW_FIM_IDX_GP01_RF_CFG, 0);
        
        msg_print_uart1("\r\nOK\r\n");

        // wait the output of Uart
        _at_cmd_sys_internal_wait(AT_CMD_SYS_WAIT_TIME);

        // reset all
        Hal_Sys_SwResetAll();
        while(1);
    }

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
int _at_cmd_sys_uartcur_patch(char *buf, int len, int mode)
{
    T_HalUartConfig tUartConfig;

    if (AT_CMD_MODE_READ == mode)
    {
        if (0 == Hal_Uart_ConfigGet(UART_IDX_1, &tUartConfig))
        {
            _at_cmd_sys_internal_uart_config_output_patch(&tUartConfig, 1);
        }
    }
    else if (AT_CMD_MODE_SET == mode)
    {
        if (1 == _at_cmd_sys_internal_uart_config_parse_patch(buf, len, &tUartConfig))
        {
            msg_print_uart1("\r\nOK\r\n");

            // wait the output of Uart
            _at_cmd_sys_internal_wait(AT_CMD_SYS_WAIT_TIME);

            // set the new config
            Hal_Uart_ConfigSet(UART_IDX_1, &tUartConfig);
        }
    }

    return true;
}

/*
 * @brief Command at+uartdef
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_sys_uartdef_patch(char *buf, int len, int mode)
{
    T_HalUartConfig tUartConfig;

    if (AT_CMD_MODE_READ == mode)
    {
        if (0 == Hal_Uart_ConfigGet(UART_IDX_1, &tUartConfig))
        {
            _at_cmd_sys_internal_uart_config_output_patch(&tUartConfig, 0);
        }
    }
    else if (AT_CMD_MODE_SET == mode)
    {
        if (1 == _at_cmd_sys_internal_uart_config_parse_patch(buf, len, &tUartConfig))
        {
            if (MW_FIM_OK == MwFim_FileWrite(MW_FIM_IDX_GP01_UART_CFG, 1, MW_FIM_UART_CFG_SIZE, (uint8_t*)&tUartConfig))
            {
                msg_print_uart1("\r\nOK\r\n");

                // wait the output of Uart
                _at_cmd_sys_internal_wait(AT_CMD_SYS_WAIT_TIME);

                // set the new config
                Hal_Uart_ConfigSet(UART_IDX_1, &tUartConfig);
            }
        }
    }

    return true;
}

/*
 * @brief AT Command Interface Initialization for System modules
 *
 */
extern _at_command_t *_g_AtCmdTbl_Sys_Ptr;
void _at_cmd_sys_func_patch_init(void)
{
    // index = 1, it means "at+gmr"
    _g_AtCmdTbl_Sys_Ptr[1].cmd_handle = _at_cmd_sys_gmr_patch;
    _g_AtCmdTbl_Sys_Ptr[3].cmd_handle = _at_cmd_sys_restore_patch;
    _g_AtCmdTbl_Sys_Ptr[4].cmd_handle = _at_cmd_sys_uartcur_patch;
    _g_AtCmdTbl_Sys_Ptr[5].cmd_handle = _at_cmd_sys_uartdef_patch;
}
