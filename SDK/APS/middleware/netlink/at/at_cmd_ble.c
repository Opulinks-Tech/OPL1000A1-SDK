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
 * @file at_cmd_ble.c
 * @author Michael Liao
 * @date 14 Dec 2017
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
int at_cmd_ble_init(int argc, char *argv[])
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
int at_cmd_ble_addr(int argc, char *argv[])
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
int at_cmd_ble_scanparam(int argc, char *argv[])
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
int at_cmd_ble_scanrspdata(int argc, char *argv[])
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
int at_cmd_ble_advparam(int argc, char *argv[])
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
int at_cmd_ble_advdata(int argc, char *argv[])
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
int at_cmd_ble_datalen(int argc, char *argv[])
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
int at_cmd_ble_cfgmtu(int argc, char *argv[])
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
int at_cmd_ble_gattssrvstart(int argc, char *argv[])
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
int at_cmd_ble_scan(int argc, char *argv[])
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
int at_cmd_ble_advstart(int argc, char *argv[])
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
int at_cmd_ble_advstop(int argc, char *argv[])
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
int at_cmd_ble_conn(int argc, char *argv[])
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
int at_cmd_ble_gattssrv(int argc, char *argv[])
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
int at_cmd_ble_gattschar(int argc, char *argv[])
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
int at_cmd_ble_gattsntfy(int argc, char *argv[])
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
int at_cmd_ble_gattsind(int argc, char *argv[])
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
int at_cmd_ble_gattssetattr(int argc, char *argv[])
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
int at_cmd_ble_gattcprimsrv(int argc, char *argv[])
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
int at_cmd_ble_gattcinclsrv(int argc, char *argv[])
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
int at_cmd_ble_gattcchar(int argc, char *argv[])
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
int at_cmd_ble_gattcrd(int argc, char *argv[])
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
int at_cmd_ble_gattcwr(int argc, char *argv[])
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
int at_cmd_ble_disconn(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Global variable ble_uart_mode declaration & initialization
 *
 * It's for BLE HCI moe's internal usage
 *
 */
uint8_t ble_uart_mode = 0;

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
int at_cmd_ble_mode(int argc, char *argv[])
{
	if (argc == 2)
	{
		int param = strtol(argv[1], NULL, 10);
		switch (param)
		{
			case 1:
				ble_uart_mode = 1;
				le_ctrl_data.is_uart_hci_mode = 1;
				printf("\r\nOK\r\n");
                msg_print_uart1("\r\nOK\r\n"); /** For UART1 */
				break;
			case 2:
				ble_uart_mode = 2;
				printf("\r\nOK\r\n");
                msg_print_uart1("\r\nOK\r\n"); /** For UART1 */
				break;
			default:
				printf("\r\nERROR\r\n");
                msg_print_uart1("\r\nERROR\r\n"); /** For UART1 */
				break;
		}
	}
	else
	{
		printf("\r\nERROR\r\n");
        msg_print_uart1("\r\nERROR\r\n"); /** For UART1 */
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
int at_cmd_ble_dbg(int argc, char *argv[])
{
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
int at_cmd_ble_start(int argc, char *argv[])
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
int at_cmd_ble_end(int argc, char *argv[])
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
int at_cmd_ble_rsv(int argc, char *argv[])
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
int at_cmd_bles_init(int argc, char *argv[])
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
int at_cmd_bles_start_bcst(int argc, char *argv[])
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
int at_cmd_bles_stop_bcst(int argc, char *argv[])
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
int at_cmd_bles_senddata(int argc, char *argv[])
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
int at_cmd_bles_setdata(int argc, char *argv[])
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
int at_cmd_bles_showdata(int argc, char *argv[])
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
int at_cmd_bles_down(int argc, char *argv[])
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
int at_cmd_bles_adv_infinite(int argc, char *argv[])
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
int at_cmd_ble_connect(int argc, char *argv[])
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
int at_cmd_ble_connect_info(int argc, char *argv[])
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
int at_cmd_ble_getdata(int argc, char *argv[])
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
int at_cmd_ble_senddata(int argc, char *argv[])
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
int at_cmd_ble_showdata(int argc, char *argv[])
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
int at_cmd_ble_removedata(int argc, char *argv[])
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
int at_cmd_ble_disconnect(int argc, char *argv[])
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
int at_cmd_ble_down(int argc, char *argv[])
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
int at_cmd_blestatus(int argc, char *argv[])
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
int at_cmd_bts_init(int argc, char *argv[])
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
int at_cmd_bts_down(int argc, char *argv[])
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
int at_cmd_bts_rfcomm_start(int argc, char *argv[])
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
int at_cmd_bts_rfcomm_stop(int argc, char *argv[])
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
int at_cmd_bts_rfcomm_send(int argc, char *argv[])
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
int at_cmd_bt_init(int argc, char *argv[])
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
int at_cmd_bt_down(int argc, char *argv[])
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
int at_cmd_bt_rfcomm_start(int argc, char *argv[])
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
int at_cmd_bt_rfcomm_stop(int argc, char *argv[])
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
int at_cmd_bt_rfcomm_send(int argc, char *argv[])
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
int at_cmd_bt_scan(int argc, char *argv[])
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
int at_cmd_btstatus(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Sample code to do BLE test
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_ble_sample(void)
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

/**
  * @brief AT Command Table for BLE Module
  *
  */
at_command_t gAtCmdTbl_Ble[] =
{
    { "at+bleinit",          at_cmd_ble_init,           "Bluetooth Low Energy (BLE) initialization" },
    { "at+bleaddr",          at_cmd_ble_addr,           "Sets BLE device's address" },
    { "at+blescanparam",     at_cmd_ble_scanparam,      "Sets parameters of BLE scanning" },
    { "at+blescanrspdata",   at_cmd_ble_scanrspdata,    "Sets BLE scan response" },
    { "at+bleadvparam",      at_cmd_ble_advparam,       "Sets parameters of BLE advertising" },
    { "at+bleadvdata",       at_cmd_ble_advdata,        "Sets BLE advertising data" },
    { "at+bledatalen",       at_cmd_ble_datalen,        "Sets BLE data length" },
    { "at+blecfgmtU",        at_cmd_ble_cfgmtu,         "Sets BLE MTU length" },
    { "at+blegattssrvstart", at_cmd_ble_gattssrvstart,  "GATTS starts services" },
    { "at+blescan",          at_cmd_ble_scan,           "Enables BLE scanning" },
    { "at+bleadvstart",      at_cmd_ble_advstart,       "Starts BLE advertising" },
    { "at+bleadvstop",       at_cmd_ble_advstop,        "Stops BLE advertising" },
    { "at+bleconn",          at_cmd_ble_conn,           "Establishes BLE connection" },
    { "at+blegattssrv",      at_cmd_ble_gattssrv,       "discovers services" },
    { "at+blegattschar",     at_cmd_ble_gattschar,      "GATTS discovers characteristics" },
    { "at+blegattsntfy",     at_cmd_ble_gattsntfy,      "GATTS notifies of characteristics" },
    { "at+blegattsind",      at_cmd_ble_gattsind,       "GATTS indicates characteristics" },
    { "at+blegattssetattr",  at_cmd_ble_gattssetattr,   "GATTS sets attributes" },
    { "at+blegattcprimsrv",  at_cmd_ble_gattcprimsrv,   "Generic Attributes Client (GATTC) discovers primary services" },
    { "at+blegattcinclsrv",  at_cmd_ble_gattcinclsrv,   "GATTC discovers included services" },
    { "at+blegattcchar",     at_cmd_ble_gattcchar,      "GATTC discovers characteristics" },
    { "at+blegattcrd",       at_cmd_ble_gattcrd,        "GATTC reads characteristic" },
    { "at+blegattcwr",       at_cmd_ble_gattcwr,        "GATTC writes characteristics" },
    { "at+bledisconn",       at_cmd_ble_disconn,        "Ends BLE connection" },
    { "at+blemode",          at_cmd_ble_mode,           "Change ble mode. 0:default, 1:controller, 2:host" },
    { "at+bledbg",           at_cmd_ble_dbg,            "Enable ble debug message. e.g. at+bledbg [HCI][LL_Hook][LL_ENC (value:0-disable;1-enable]" },
    { "at+blestart",         at_cmd_ble_start,          "BLE Start" },    //Back Door
    { "at+bleend",           at_cmd_ble_end,            "BLE End" },      //Back Door
    { "at+blersv",           at_cmd_ble_rsv,            "BLE Reserved" }, //Back Door
    { "at+bles_init",        at_cmd_bles_init,          "BLE server initialization" },
    { "at+bles_start_bcst",  at_cmd_bles_start_bcst,    "BLE server starts ADV, Broadcasting" },
    { "at+bles_stop_bcst",   at_cmd_bles_stop_bcst,     "BLE server stops ADV, Broadcasting" },
    { "at+bles_senddata",    at_cmd_bles_senddata,      "BLE server sends data to BLE client" },
    { "at+bles_setdata",     at_cmd_bles_setdata,       "Set data on BLE server" },
    { "at+bles_showdata",    at_cmd_bles_showdata,      "Show customerized data on BLE server" },
    { "at+bles_down",        at_cmd_bles_down,          "Stops BLE server" },
    { "at+bles_adv_infinite",at_cmd_bles_adv_infinite,  "Select broading mode of BLE server" },
    { "at+ble_scan",         at_cmd_ble_scan,           "Do BLE scan" },
    { "at+ble_connect",      at_cmd_ble_connect,        "Connect to a specified BLE server" },
    { "at+ble_connect_info", at_cmd_ble_connect_info,   "Show connected BLE server information" },
    { "at+ble_getdata",      at_cmd_ble_getdata,        "Get data from BLE server" },
    { "at+ble_senddata",     at_cmd_ble_senddata,       "Send the data to BLE server" },
    { "at+ble_showdata",     at_cmd_ble_showdata,       "Show the data from BLE buffer" },
    { "at+ble_removedata",   at_cmd_ble_removedata,     "Remove the data from BLE buffer" },
    { "at+ble_disconnect",   at_cmd_ble_disconnect,     "Disconnect BLE connction" },
    { "at+ble_down",         at_cmd_ble_down,           "Stop BLE client" },
    { "at+blestatus",        at_cmd_blestatus,          "Show BLE modules status" },
    { "at+bts_init",         at_cmd_bts_init,           "Init BT server" },
    { "at+bts_down",         at_cmd_bts_down,           "Close BT server" },
    { "at+bts_rfcomm_start", at_cmd_bts_rfcomm_start,   "Start BT server RFCOMM service" },
    { "at+bts_rfcomm_stop",  at_cmd_bts_rfcomm_stop,    "Close BT server RFCOMM service" },
    { "at+bts_rfcomm_send",  at_cmd_bts_rfcomm_send,    "BT server send RFCOMM data" },
    { "at+bt_init",          at_cmd_bt_init,            "Initialize BT client" },
    { "at+bt_down",          at_cmd_bt_down,            "Close BT client" },
    { "at+bt_rfcomm_start",  at_cmd_bt_rfcomm_start,    "Open BT client RFCOMM service" },
    { "at+bt_rfcomm_start",  at_cmd_bt_rfcomm_stop,     "Close BT client RFCOMM service" },
    { "at+bt_rfcomm_send",   at_cmd_bt_rfcomm_send,     "BT client sends RFCOMM data" },
    { "at+bt_scan",          at_cmd_bt_scan,            "Do BT scan" },
    { "at+btstatus",         at_cmd_btstatus,           "Show BT status" },
    { NULL,                  NULL,                      NULL},
};

/*
 * @brief Global variable ble_uart_mode retention attribute segment
 *
 */
RET_DATA at_command_t *g_AtCmdTbl_Ble_Ptr;

/*
 * @brief AT Command Interface Initialization for BLE module
 *
 */
void at_cmd_ble_func_init(void)
{
    /** Command Table (BLE) */
    g_AtCmdTbl_Ble_Ptr = gAtCmdTbl_Ble;
}

