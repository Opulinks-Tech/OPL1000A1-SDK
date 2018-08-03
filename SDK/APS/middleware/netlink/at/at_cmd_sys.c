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

#define CMD_TOKEN_SIZE          16
#define AT_CMD_SYS_WAIT_TIME    5   // ms

extern size_t xFreeBytesRemaining;

/*
 * @brief parse the token
 *
 * @param [in] the input string
 *
 * @param [in] argc count of parameters
 *
 * @param [out] argv parameters array
 *
 * @return none
 *
 */
static void Cmd_TokenParse(char* pszData, uint8_t* pubCount, uint8_t* pubaToken[])
{
    uint8_t ucaSeps[] = "?=, \t\r\n";         // ?, =, space, tab, new line
    uint8_t i = 0;

    // get the first token
    pubaToken[0] = (uint8_t*)strtok(pszData, (const char*)ucaSeps);

    // get the other token
    if (NULL != pubaToken[0])
    {
        for (i=1; i<CMD_TOKEN_SIZE; i++)
        {
            pubaToken[i] = (uint8_t*)strtok(NULL, (const char*)ucaSeps);
            if (NULL == pubaToken[i])
                break;
        }
    }

    // update the token count
    *pubCount = i;
}

/*
 * @brief it is a busy wait
 *
 * @param [in] none
 *
 * @param [in] none
 *
 * @return none
 *
 */
static void _at_cmd_sys_internal_wait(uint32_t u32MilliSec)
{
    uint32_t u32TimeOutTick = 0;
    uint32_t u32TimeOutStart = 0;

    if(u32MilliSec >= Hal_Tick_MilliSecMax())
        u32TimeOutTick = Hal_Tick_MilliSecMax() * Hal_Tick_PerMilliSec();
    else
        u32TimeOutTick = u32MilliSec * Hal_Tick_PerMilliSec();

    u32TimeOutStart = Hal_Tick_Diff(0);
    while (Hal_Tick_Diff(u32TimeOutStart) < u32TimeOutTick)
    {
        ;
    }
}

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
static uint8_t _at_cmd_sys_internal_uart_config_parse(char *buf, int len, T_HalUartConfig *ptUartConfig)
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
        case 2:
            ptUartConfig->ubStopBit = STOP_BIT_1P5;
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
static uint8_t _at_cmd_sys_internal_uart_config_output(T_HalUartConfig *ptUartConfig)
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

    msg_print_uart1("+UART_DEF:%u,%u,%u,%u,%u\r\n", ulBuadrate, ubDataBit, ubStopBit, ubParity, ubFlowCtrl);

    msg_print_uart1("\r\nOK\r\n");

    return 1;
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
int _at_cmd_sys_rst(char *buf, int len, int mode)
{
    if (AT_CMD_MODE_EXECUTION == mode)
    {
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
 * @brief Command at+gmr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_sys_gmr(char *buf, int len, int mode)
{
    if (AT_CMD_MODE_EXECUTION == mode)
    {
        msg_print_uart1("\r\n");

        msg_print_uart1("AT version info: %s \r\n", AT_VER);
        msg_print_uart1("SDK version info: %d \r\n", SVN_REVISION);
        msg_print_uart1("Compile time: %s \r\n", SVN_TIME_NOW);

        msg_print_uart1("\r\nOK\r\n");
    }

    return true;
}

void _at_cmd_sys_gslp_wakeup_callback()
{
	msg_print_uart1("\r\nWAKEUP\r\n");
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
int _at_cmd_sys_gslp(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

	switch (mode)		
	{
		case AT_CMD_MODE_SET:
		{
			int sleep_duration_ms = atoi(argv[1]);
			msg_print_uart1("\r\nOK\r\n");
			ps_sleep_requested_by_app(sleep_duration_ms * 1000, _at_cmd_sys_gslp_wakeup_callback);
			break;
		}

		default:
			msg_print_uart1("\r\ndefault\r\n");
			break;
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
int _at_cmd_sys_restore(char *buf, int len, int mode)
{
    if (AT_CMD_MODE_EXECUTION == mode)
    {
        // reset the config of UART0 / UART1
        MwFim_FileWriteDefault(MW_FIM_IDX_GP01_UART_CFG, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_GP01_UART_CFG, 1);

        // reset Auto connect configuartion and Info
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_MODE, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_NUM, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_CFG, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 0);
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 1);
        MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 2);

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
 * @brief Command at+uart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_sys_uart(char *buf, int len, int mode)
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
int _at_cmd_sys_uartcur(char *buf, int len, int mode)
{
    T_HalUartConfig tUartConfig;

    if (AT_CMD_MODE_READ == mode)
    {
        if (0 == Hal_Uart_ConfigGet(UART_IDX_1, &tUartConfig))
        {
            _at_cmd_sys_internal_uart_config_output(&tUartConfig);
        }
    }
    else if (AT_CMD_MODE_SET == mode)
    {
        if (1 == _at_cmd_sys_internal_uart_config_parse(buf, len, &tUartConfig))
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
int _at_cmd_sys_uartdef(char *buf, int len, int mode)
{
    T_HalUartConfig tUartConfig;

    if (AT_CMD_MODE_READ == mode)
    {
        if (0 == Hal_Uart_ConfigGet(UART_IDX_1, &tUartConfig))
        {
            _at_cmd_sys_internal_uart_config_output(&tUartConfig);
        }
    }
    else if (AT_CMD_MODE_SET == mode)
    {
        if (1 == _at_cmd_sys_internal_uart_config_parse(buf, len, &tUartConfig))
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
 * @brief Command at+sleep
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_sys_sleep(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

	switch (mode)		
	{
		case AT_CMD_MODE_SET:
		{
			int is_enable = atoi(argv[1]);
			msg_print_uart1("\r\nOK\r\n");
			ps_enable(is_enable);
			break;
		}

		default:
			msg_print_uart1("\r\ndefault\r\n");
			break;
	}

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
int _at_cmd_sys_ram(char *buf, int len, int mode)
{
    if (AT_CMD_MODE_READ == mode)
    {
        msg_print_uart1("+SYSRAM:%u\r\n", xFreeBytesRemaining);

        msg_print_uart1("\r\nOK\r\n");
    }
    return true;
}

void sys_cmd_reg(int argc, char *argv[])
{
    uint32_t addr, val, old;

    if (strcmp(argv[1], "r") == 0 && argc >=3)
    {
        addr = strtoul(argv[2], NULL, 16);
        addr &= 0xFFFFFFFC;

        if (argc > 3)
        {
            uint32_t length;

            at_output("\nStart address: 0x%08X\n", addr);
            length = strtoul(argv[3], NULL, 0);
            for (uint32_t i=0; i<length; i++)
            {
                if ((i&0x3) == 0x0)
                    at_output("\n%08X:", addr+i*4);
                at_output("    %08X", reg_read(addr+i*4));
            }
            at_output("\n");
        }
        else
        {
            val = reg_read(addr);
            at_output("0x%X: 0x%08X\n", addr, val);
        }
    }
    else if (strcmp(argv[1], "w") == 0 && argc == 4)
    {
        addr = strtoul(argv[2], NULL, 16);
        addr &= 0xFFFFFFFC;
        val = strtoul(argv[3], NULL, 16);
        old = reg_read(addr);
        reg_write(addr, val);
        val = reg_read(addr);

        at_output("REG:0x%08X: 0x%08X, (old:0x%08X)\n", addr, val, old);
    }
    else
    {
        at_output("Usage:");
        at_output("\treg r <addr> [<length>]\r\n");
        at_output("\treg w <addr> <value>\r\n");
    }
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
int _at_cmd_sys_reg(char *buf, int len, int mode)
{
#if 1
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if((argc < 1) || (argc > AT_MAX_CMD_ARGS))
    {
        at_output("invalid param number\r\n");
        goto done;
    }

    sys_cmd_reg(argc, argv);

    iRet = 1;

done:
    if(!iRet)
    {
        at_output("ERROR\r\n");
    }
    else
    {
        at_output("OK\r\n");
    }

    return iRet;
#else
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
#endif
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
int _at_cmd_sys_start(char *buf, int len, int mode)
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
int _at_cmd_sys_end(char *buf, int len, int mode)
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
int _at_cmd_sys_rsv(char *buf, int len, int mode)
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
int _at_cmd_sys_auto_recording_on(char *buf, int len, int mode)
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
int _at_cmd_sys_auto_recording_off(char *buf, int len, int mode)
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
int _at_cmd_sys_fwver(char *buf, int len, int mode)
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
int _at_cmd_sys_mcupwrsave(char *buf, int len, int mode)
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
int _at_cmd_sys_mcustandby(char *buf, int len, int mode)
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
int _at_cmd_sys_ota_svr_start(char *buf, int len, int mode)
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
int _at_cmd_sys_ota_svr_stop(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+dbguart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_sys_dbg_uart_ctrl(char *buf, int len, int mode)
{
    uint8_t ubCount;
    uint8_t* pubaToken[CMD_TOKEN_SIZE];
    uint32_t ulToken;

    // pre-parser the input string
    Cmd_TokenParse(buf, &ubCount, pubaToken);
    
    if (AT_CMD_MODE_SET == mode)
    {
        // AT+DBGUART=<enable/disable>
        if (2 == ubCount)
        {
            ulToken = strtoul((const char*)pubaToken[1], NULL, 0);
            if (ulToken == 1)
            {
                Hal_DbgUart_RxIntEn(1);
                
                msg_print_uart1("\r\nOK\r\n");
            }
            else if (ulToken == 0)
            {
                Hal_DbgUart_RxIntEn(0);
                
                msg_print_uart1("\r\nOK\r\n");
            }
        }
    }

    return true;
}

/*
 * @brief Command at+download
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
extern uint32_t Boot_CheckUartBehavior(void);
int _at_cmd_sys_download(char *buf, int len, int mode)
{
    if (AT_CMD_MODE_EXECUTION == mode)
    {
        msg_print_uart1("\r\nOK\r\n");

        // wait the output of Uart
        _at_cmd_sys_internal_wait(AT_CMD_SYS_WAIT_TIME);
        
        osEnterCritical();
        Hal_SysPinMuxDownloadInit();
        Boot_CheckUartBehavior();
        osExitCritical();
        
        // wait the output of Uart
        _at_cmd_sys_internal_wait(AT_CMD_SYS_WAIT_TIME);

        // reset all
        Hal_Sys_SwResetAll();
        while(1);
    }

    return true;
}

/*
 * @brief  Sample code to do system test
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_sys_sample(void)
{
    return true;
}

/**
  * @brief AT Command Table for System Module
  *
  */
_at_command_t _gAtCmdTbl_Sys[] =
{
    { "at+rst",                 _at_cmd_sys_rst,           "Restart module" },
    { "at+gmr",                 _at_cmd_sys_gmr,           "View version info" },
    { "at+gslp",                _at_cmd_sys_gslp,          "Enter deep-sleep mode" },
    { "at+restore",             _at_cmd_sys_restore,       "Factory Reset" },
    { "at+uart_cur",            _at_cmd_sys_uartcur,       "UART current configuration show" },
    { "at+uart_def",            _at_cmd_sys_uartdef,       "UART default configuration, save to flash" },
    { "at+uart",                _at_cmd_sys_uart,          "UART configuration" },
    { "at+sleep",               _at_cmd_sys_sleep,         "Sleep mode" },
    { "at+sysram",              _at_cmd_sys_ram,           "RAM information" },
    { "at+reg",                 _at_cmd_sys_reg,           "Register Access" },
    { "at+sysinit",             _at_cmd_sys_start,         "Sys Start" },    //Back Door
    { "at+sysend",              _at_cmd_sys_end,           "Sys End" },      //Back Door
    { "at+sysrsv",              _at_cmd_sys_rsv,           "Sys Reserved" }, //Back Door
    { "at+auto_recording_on",   _at_cmd_sys_auto_recording_on,  "Save Wi-Fi/BLE status" },
    { "at+auto_recording_off",  _at_cmd_sys_auto_recording_off, "Turn off auto_recording" },
    { "at+fwver",               _at_cmd_sys_fwver,         "Show FW Version" },
    { "at+mcupwrsave",          _at_cmd_sys_mcupwrsave,    "Enter Power save mode" },
    { "at+mcustandby",          _at_cmd_sys_mcustandby,    "Enter Standby mode" },
    { "at+ota_svr_start",       _at_cmd_sys_ota_svr_start, "Open OTA FW Upgrade(HTTP) function" },
    { "at+ota_svr_stop",        _at_cmd_sys_ota_svr_stop,  "Close OTA FW Upgrade(HTTP) function" },
    { "at+dbguart",             _at_cmd_sys_dbg_uart_ctrl, "Enable/Disable the Rx part of debug UART" },
    { "at+download",            _at_cmd_sys_download,      "Download the patch image" },
    { NULL,                     NULL,                     NULL},
};

/*
 * @brief Global variable g_AtCmdTbl_Sys_Ptr retention attribute segment
 *
 */
RET_DATA _at_command_t *_g_AtCmdTbl_Sys_Ptr;

/*
 * @brief AT Command Interface Initialization for System modules
 *
 */
void _at_cmd_sys_func_init(void)
{
    /** Command Table (System) */
    _g_AtCmdTbl_Sys_Ptr = _gAtCmdTbl_Sys;
}

