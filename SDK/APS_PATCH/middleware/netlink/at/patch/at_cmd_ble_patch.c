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
 * @file at_cmd_ble_patch.c
 * @author Michael Liao
 * @date 20 Mar 2018
 * @brief File supports the BLE module AT Commands.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_ble.h"
#include "le_ctrl.h"
#include "at_cmd_common.h"
#include "at_cmd_ble_patch.h"
#include "at_cmd_patch.h"
#include "at_cmd_common_patch.h"


/*
 * @brief Command at+bleinit
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_init(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bleaddr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_addr(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blescanparam
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_scanparam(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blescanrspdata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_scanrspdata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bleadvparam
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_advparam(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bleadvdata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_advdata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bledatalen
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_datalen(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blecfgmtU
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_cfgmtu(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattssrvstart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattssrvstart(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blescan
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_scan(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bleadvstart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_advstart(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bleadvstop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_advstop(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bleconn
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_conn(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattssrv
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattssrv(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattschar
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattschar(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattsntfy
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattsntfy(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattsind
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattsind(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattssetattr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattssetattr(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattcprimsrv
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattcprimsrv(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattcinclsrv
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattcinclsrv(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattcchar
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattcchar(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattcrd
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattcrd(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blegattcwr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_gattcwr(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bledisconn
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_disconn(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blemode
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_mode(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    int uart_mode = UART1_MODE_DEFAULT;
    extern unsigned int g_uart1_mode;

    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    
    if(argc > 1) {
        uart_mode = atoi(argv[1]);
        if ((uart_mode >= UART1_MODE_NUM) || (uart_mode < UART1_MODE_AT)) return false;
    }
    
    g_uart1_mode = uart_mode;
    
    switch (uart_mode)
    {
        case UART1_MODE_AT:
            uart1_mode_set_at();
            break;
        case UART1_MODE_BLE_HCI:
            uart1_mode_set_ble_hci();
            break;
/*
        case UART1_MODE_BLE_HOST:
            break;
*/
        case UART1_MODE_OTHERS:
            uart1_mode_set_others();
            break;
        default:
            break;
    }
	return true;
}

/*
 * @brief Command at+bledbg
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_dbg(char *buf, int len, int mode)
{
#if 0
	if (argc == 4)
	{
		int enable_hci = strtol(argv[1], NULL, 10);
		int enable_ll_hook = strtol(argv[2], NULL, 10);
		int enable_ll_enc = strtol(argv[3], NULL, 10);

		le_ctrl_data.open_dbg_hci_standard = enable_hci;
		le_ctrl_data.open_dbg_ll_hci_hook = enable_ll_hook;
		le_ctrl_data.open_dbg_ll_enc = enable_ll_enc;

		printf("\r\nOK\r\n");
        msg_print_uart1("\r\nOK\r\n");
	}
	else
	{
		printf("\r\nERROR\r\n");
        msg_print_uart1("\r\nERROR\r\n");
	}
#endif
    return true;
}

/*
 * @brief Command at+blestart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_start(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bleend
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_end(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blersv
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_rsv(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bles_init
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bles_init(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+at_cmd_bles_start_bcst
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bles_start_bcst(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bles_stop_bcst
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bles_stop_bcst(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+at_cmd_bles_senddata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bles_senddata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bles_setdata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bles_setdata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bles_showdata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bles_showdata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bles_down
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bles_down(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bles_adv_infinite
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bles_adv_infinite(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ble_connect
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_connect(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ble_connect_info
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_connect_info(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ble_getdata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_getdata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ble_senddata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_senddata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ble_showdata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_showdata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ble_removedata
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_removedata(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ble_disconnect
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_disconnect(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+ble_down
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_down(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+blestatus
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_blestatus(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+at_cmd_bts_init
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bts_init(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bts_down
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bts_down(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bts_rfcomm_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bts_rfcomm_start(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bts_rfcomm_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bts_rfcomm_stop(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bts_rfcomm_send
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bts_rfcomm_send(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bt_init
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bt_init(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bt_down
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bt_down(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bt_rfcomm_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bt_rfcomm_start(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+at_cmd_bt_rfcomm_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bt_rfcomm_stop(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bt_rfcomm_send
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bt_rfcomm_send(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+bt_scan
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_bt_scan(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Command at+at_cmd_btstatus
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_btstatus(char *buf, int len, int mode)
{
    return true;
}

/*
 * @brief Sample code to do BLE test
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_ble_sample(void)
{
    //call BLE at cmd to do BLE initialization

    //call BLE at cmd to do Host initialization

    //call BLE at cmd to enable adv

    //call BLE at cmd to establish BLE connection

    //call BLE at cmd to read/write data

    //call BLE at cmd to end BLE connection

    //...

    return true;
}

_at_command_t _gAtCmdTbl_Ble[] =
{
#if defined(__AT_CMD_ENABLE__)
    { "at+bleinit",          _at_cmd_ble_init,           "Bluetooth Low Energy (BLE) initialization" },
    { "at+bleaddr",          _at_cmd_ble_addr,           "Sets BLE device's address" },
    { "at+blescanparam",     _at_cmd_ble_scanparam,      "Sets parameters of BLE scanning" },
    { "at+blescanrspdata",   _at_cmd_ble_scanrspdata,    "Sets BLE scan response" },
    { "at+bleadvparam",      _at_cmd_ble_advparam,       "Sets parameters of BLE advertising" },
    { "at+bleadvdata",       _at_cmd_ble_advdata,        "Sets BLE advertising data" },
    { "at+bledatalen",       _at_cmd_ble_datalen,        "Sets BLE data length" },
    { "at+blecfgmtU",        _at_cmd_ble_cfgmtu,         "Sets BLE MTU length" },
    { "at+blegattssrvstart", _at_cmd_ble_gattssrvstart,  "GATTS starts services" },
    { "at+blescan",          _at_cmd_ble_scan,           "Enables BLE scanning" },
    { "at+bleadvstart",      _at_cmd_ble_advstart,       "Starts BLE advertising" },
    { "at+bleadvstop",       _at_cmd_ble_advstop,        "Stops BLE advertising" },
    { "at+bleconn",          _at_cmd_ble_conn,           "Establishes BLE connection" },
    { "at+blegattssrv",      _at_cmd_ble_gattssrv,       "discovers services" },
    { "at+blegattschar",     _at_cmd_ble_gattschar,      "GATTS discovers characteristics" },
    { "at+blegattsntfy",     _at_cmd_ble_gattsntfy,      "GATTS notifies of characteristics" },
    { "at+blegattsind",      _at_cmd_ble_gattsind,       "GATTS indicates characteristics" },
    { "at+blegattssetattr",  _at_cmd_ble_gattssetattr,   "GATTS sets attributes" },
    { "at+blegattcprimsrv",  _at_cmd_ble_gattcprimsrv,   "Generic Attributes Client (GATTC) discovers primary services" },
    { "at+blegattcinclsrv",  _at_cmd_ble_gattcinclsrv,   "GATTC discovers included services" },
    { "at+blegattcchar",     _at_cmd_ble_gattcchar,      "GATTC discovers characteristics" },
    { "at+blegattcrd",       _at_cmd_ble_gattcrd,        "GATTC reads characteristic" },
    { "at+blegattcwr",       _at_cmd_ble_gattcwr,        "GATTC writes characteristics" },
    { "at+bledisconn",       _at_cmd_ble_disconn,        "Ends BLE connection" },
    { "at+blemode",          _at_cmd_ble_mode,           "Change ble mode. 0:default, 1:controller, 2:host" },
    { "at+bledbg",           _at_cmd_ble_dbg,            "Enable ble debug message. e.g. at+bledbg [HCI][LL_Hook][LL_ENC (value:0-disable;1-enable]" },
    { "at+blestart",         _at_cmd_ble_start,          "BLE Start" },    //Back Door
    { "at+bleend",           _at_cmd_ble_end,            "BLE End" },      //Back Door
    { "at+blersv",           _at_cmd_ble_rsv,            "BLE Reserved" }, //Back Door
    { "at+bles_init",        _at_cmd_bles_init,          "BLE server initialization" },
    { "at+bles_start_bcst",  _at_cmd_bles_start_bcst,    "BLE server starts ADV, Broadcasting" },
    { "at+bles_stop_bcst",   _at_cmd_bles_stop_bcst,     "BLE server stops ADV, Broadcasting" },
    { "at+bles_senddata",    _at_cmd_bles_senddata,      "BLE server sends data to BLE client" },
    { "at+bles_setdata",     _at_cmd_bles_setdata,       "Set data on BLE server" },
    { "at+bles_showdata",    _at_cmd_bles_showdata,      "Show customerized data on BLE server" },
    { "at+bles_down",        _at_cmd_bles_down,          "Stops BLE server" },
    { "at+bles_adv_infinite",_at_cmd_bles_adv_infinite,  "Select broading mode of BLE server" },
    { "at+ble_scan",         _at_cmd_ble_scan,           "Do BLE scan" },
    { "at+ble_connect",      _at_cmd_ble_connect,        "Connect to a specified BLE server" },
    { "at+ble_connect_info", _at_cmd_ble_connect_info,   "Show connected BLE server information" },
    { "at+ble_getdata",      _at_cmd_ble_getdata,        "Get data from BLE server" },
    { "at+ble_senddata",     _at_cmd_ble_senddata,       "Send the data to BLE server" },
    { "at+ble_showdata",     _at_cmd_ble_showdata,       "Show the data from BLE buffer" },
    { "at+ble_removedata",   _at_cmd_ble_removedata,     "Remove the data from BLE buffer" },
    { "at+ble_disconnect",   _at_cmd_ble_disconnect,     "Disconnect BLE connction" },
    { "at+ble_down",         _at_cmd_ble_down,           "Stop BLE client" },
    { "at+blestatus",        _at_cmd_blestatus,          "Show BLE modules status" },
    { "at+bts_init",         _at_cmd_bts_init,           "Init BT server" },
    { "at+bts_down",         _at_cmd_bts_down,           "Close BT server" },
    { "at+bts_rfcomm_start", _at_cmd_bts_rfcomm_start,   "Start BT server RFCOMM service" },
    { "at+bts_rfcomm_stop",  _at_cmd_bts_rfcomm_stop,    "Close BT server RFCOMM service" },
    { "at+bts_rfcomm_send",  _at_cmd_bts_rfcomm_send,    "BT server send RFCOMM data" },
    { "at+bt_init",          _at_cmd_bt_init,            "Initialize BT client" },
    { "at+bt_down",          _at_cmd_bt_down,            "Close BT client" },
    { "at+bt_rfcomm_start",  _at_cmd_bt_rfcomm_start,    "Open BT client RFCOMM service" },
    { "at+bt_rfcomm_start",  _at_cmd_bt_rfcomm_stop,     "Close BT client RFCOMM service" },
    { "at+bt_rfcomm_send",   _at_cmd_bt_rfcomm_send,     "BT client sends RFCOMM data" },
    { "at+bt_scan",          _at_cmd_bt_scan,            "Do BT scan" },
    { "at+btstatus",         _at_cmd_btstatus,           "Show BT status" },
#endif
    { NULL,                  NULL,                      NULL},
};

RET_DATA _at_command_t *_g_AtCmdTbl_Ble_Ptr;

/*
 * @brief AT Command Interface Initialization for BLE module
 *
 */
void _at_cmd_ble_func_init(void)
{
    /** Command Table (BLE) */
    _g_AtCmdTbl_Ble_Ptr = _gAtCmdTbl_Ble;
}

