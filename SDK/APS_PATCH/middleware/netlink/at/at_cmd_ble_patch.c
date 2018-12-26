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

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file



// Sec 1: Include File
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
#include "le_ctrl_patch.h"
#include "sys_common_ctrl.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype
static uint8_t _at_cmd_UtilStringToHexNum(uint8_t *data, uint16_t len, uint8_t *buf);
static uint8_t _at_cmd_UtilGetAddrFromStr(uint8_t *addr_str, uint8_t *addr);


/***********
C Functions
***********/
// Sec 8: C Functions

// Refer to static BOOL LeHtcUtilStringToHexNum_Impl(UINT8 *data, UINT16 len, UINT8 *buf)
static uint8_t _at_cmd_UtilStringToHexNum(uint8_t *data, uint16_t len, uint8_t *buf)
{
	uint16_t i = 0;
	uint8_t c;
	uint8_t lc;
	uint8_t rc;
    
	if (!data || !buf || !len || (len & 1)) return false;

	while (len)
	{
		c = data[i++];
    
		if ((c >= '0') && (c <= '9'))
			c -= '0';
        else if ((c >= 'a') && (c <= 'f'))
			c = c - 'a' + 10;
        else if ((c >= 'A') && (c <= 'F'))
			c = c - 'A' + 10;
		else
			return false;

		if (i & 1) 
		{
			lc = c;
	        continue;
        }
        else
            rc = c;

		buf[(i >> 1) - 1] = (lc << 4) | rc;

        len -= 2;
    }
	
	return true;
}

// Refer to static BOOL LeCmdAppGetAddrFromStr_Impl(UINT8 *addr_str, BD_ADDR addr)
static uint8_t _at_cmd_UtilGetAddrFromStr(uint8_t *addr_str, uint8_t *addr)
{
	uint16_t len = strlen((const char *)addr_str) + 1;
	uint8_t *s = 0;
	uint8_t *e = 0;
	uint8_t *p = addr_str;
    uint8_t addrBuf[2];
	uint8_t c;
	uint8_t count = 0;

	while (len--)
	{
		c = *p;
    
		if (((c >= '0') && (c <= '9')) || 
            ((c >= 'a') && (c <= 'f')) || 
            ((c >= 'A') && (c <= 'F')))
		{
			if (!s)
				s = p;
			else
				e = p;
        }
		else
		{
			if (s)
			{
				uint8_t *a = &addr[count];
                
				if (!e)
				{
					addrBuf[0] = '0';
					addrBuf[1] = *s;
                }
				else if ((e - s) < 2)
				{
					addrBuf[0] = *s;
					addrBuf[1] = *e;
                }
            	else
                    return false;

				if (!_at_cmd_UtilStringToHexNum(addrBuf, 2, a)) return false;
				
				count++;

				if (count >= 6) break;
            }
        
			s = e = 0;
        }

        p++;
    }

	if (count == 6)
	{
		return true;
    }

	return false;
}

/*
 * @brief Command at+letest
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_letest(char *buf, int len, int mode)
{
    /* at+letest=251, 1  =>  251 octets, 1 sec */
    
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    uint8_t tx_octets = 0;
    uint8_t duration = 0;

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }
    if(argc != 3)
    {
        at_output("invalid param number\r\n");
        goto done;
    }
    
    tx_octets = atoi(argv[1]);
    duration = atoi(argv[2]);
    
    le_ctrl_throughput_test(tx_octets, duration);
    iRet = 1;

done:
    if(!iRet)
    {
        at_output("ERROR\r\n");
    }

    return true;
}

/*
 * @brief Command at+mpbleaddr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_mp_ble_addr(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    uint8_t i = 0;
    uint8_t type = 0;
    uint8_t bd_addr[6] = {0};

    if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }
    
    if (!strncmp("?", &(argv[0][12]), 13))
    {
        get_ble_bd_addr(bd_addr);
        at_output("+MPBLEADDR:\"");
        for (i = 0; i < 6; i++)
        {
            if (i == 5)
                at_output("%2X\"\r\n", bd_addr[i]);
            else
                at_output("%2X:", bd_addr[i]);
        }
    }
    else 
    {
        type = atoi(argv[1]);
        
        if (type == 0)
        {
            if (!_at_cmd_UtilGetAddrFromStr((uint8_t *)argv[2], bd_addr)) goto done;
        
            set_ble_bd_addr(bd_addr);
        }
        else
        {
            goto done;
        }
    }
    
    iRet = 1;

done:
    if (!iRet)
    {
        at_output("+MPBLEADDR: ERROR\r\n");
    }

    return true;
}

extern int _at_cmd_ble_addr(char *buf, int len, int mode);
extern int _at_cmd_letest(char *buf, int len, int mode);
extern int _at_cmd_mp_ble_addr(char *buf, int len, int mode);
#if defined(__AT_CMD_SUPPORT__)
extern int _at_cmd_ble_init(char *buf, int len, int mode);
extern int _at_cmd_ble_scanparam(char *buf, int len, int mode);
extern int _at_cmd_ble_scanrspdata(char *buf, int len, int mode);
extern int _at_cmd_ble_advparam(char *buf, int len, int mode);
extern int _at_cmd_ble_advdata(char *buf, int len, int mode);
extern int _at_cmd_ble_datalen(char *buf, int len, int mode);
extern int _at_cmd_ble_cfgmtu(char *buf, int len, int mode);
extern int _at_cmd_ble_gattssrvstart(char *buf, int len, int mode);
extern int _at_cmd_ble_scan(char *buf, int len, int mode);
extern int _at_cmd_ble_advstart(char *buf, int len, int mode);
extern int _at_cmd_ble_advstop(char *buf, int len, int mode);
extern int _at_cmd_ble_conn(char *buf, int len, int mode);
extern int _at_cmd_ble_gattssrv(char *buf, int len, int mode);
extern int _at_cmd_ble_gattschar(char *buf, int len, int mode);
extern int _at_cmd_ble_gattsntfy(char *buf, int len, int mode);
extern int _at_cmd_ble_gattsind(char *buf, int len, int mode);
extern int _at_cmd_ble_gattssetattr(char *buf, int len, int mode);
extern int _at_cmd_ble_gattcprimsrv(char *buf, int len, int mode);
extern int _at_cmd_ble_gattcinclsrv(char *buf, int len, int mode);
extern int _at_cmd_ble_gattcchar(char *buf, int len, int mode);
extern int _at_cmd_ble_gattcrd(char *buf, int len, int mode);
extern int _at_cmd_ble_gattcwr(char *buf, int len, int mode);
extern int _at_cmd_ble_disconn(char *buf, int len, int mode);
extern int _at_cmd_ble_mode(char *buf, int len, int mode);
extern int _at_cmd_ble_dbg(char *buf, int len, int mode);
extern int _at_cmd_ble_start(char *buf, int len, int mode);
extern int _at_cmd_ble_end(char *buf, int len, int mode);
extern int _at_cmd_ble_rsv(char *buf, int len, int mode);
extern int _at_cmd_bles_init(char *buf, int len, int mode);
extern int _at_cmd_bles_start_bcst(char *buf, int len, int mode);
extern int _at_cmd_bles_stop_bcst(char *buf, int len, int mode);
extern int _at_cmd_bles_senddata(char *buf, int len, int mode);
extern int _at_cmd_bles_setdata(char *buf, int len, int mode);
extern int _at_cmd_bles_showdata(char *buf, int len, int mode);
extern int _at_cmd_bles_down(char *buf, int len, int mode);
extern int _at_cmd_bles_adv_infinite(char *buf, int len, int mode);
extern int _at_cmd_ble_scan(char *buf, int len, int mode);
extern int _at_cmd_ble_connect(char *buf, int len, int mode);
extern int _at_cmd_ble_connect_info(char *buf, int len, int mode);
extern int _at_cmd_ble_getdata(char *buf, int len, int mode);
extern int _at_cmd_ble_senddata(char *buf, int len, int mode);
extern int _at_cmd_ble_showdata(char *buf, int len, int mode);
extern int _at_cmd_ble_removedata(char *buf, int len, int mode);
extern int _at_cmd_ble_disconnect(char *buf, int len, int mode);
extern int _at_cmd_ble_down(char *buf, int len, int mode);
extern int _at_cmd_blestatus(char *buf, int len, int mode);
extern int _at_cmd_bts_init(char *buf, int len, int mode);
extern int _at_cmd_bts_down(char *buf, int len, int mode);
extern int _at_cmd_bts_rfcomm_start(char *buf, int len, int mode);
extern int _at_cmd_bts_rfcomm_stop(char *buf, int len, int mode);
extern int _at_cmd_bts_rfcomm_send(char *buf, int len, int mode);
extern int _at_cmd_bt_init(char *buf, int len, int mode);
extern int _at_cmd_bt_down(char *buf, int len, int mode);
extern int _at_cmd_bt_rfcomm_start(char *buf, int len, int mode);
extern int _at_cmd_bt_rfcomm_stop(char *buf, int len, int mode);
extern int _at_cmd_bt_rfcomm_send(char *buf, int len, int mode);
extern int _at_cmd_bt_scan(char *buf, int len, int mode);
extern int _at_cmd_btstatus(char *buf, int len, int mode);
#endif //#if defined(__AT_CMD_SUPPORT__)

_at_command_t _gAtCmdTbl_Ble_patch[] =
{
    #if defined(__AT_CMD_SUPPORT__)
    { "at+bleinit",          _at_cmd_ble_init,           "Bluetooth Low Energy (BLE) initialization" },
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
    #endif //#if defined(__AT_CMD_SUPPORT__)
    { "at+bleaddr",          _at_cmd_ble_addr,           "Sets BLE device's address" },
    { "at+letest",           _at_cmd_letest,             "LE Throughput test" },
    { "at+mpbleaddr",        _at_cmd_mp_ble_addr,        "Sets BLE device's address (MP usage)" },
    { NULL,                  NULL,                      NULL},
};


/*
 * @brief AT Command Interface Initialization for BLE module
 *
 */
void _at_cmd_ble_func_init_patch(void)
{
    extern _at_command_t *_g_AtCmdTbl_Ble_Ptr;
    /** Command Table (BLE) */
    _g_AtCmdTbl_Ble_Ptr = _gAtCmdTbl_Ble_patch;
}




