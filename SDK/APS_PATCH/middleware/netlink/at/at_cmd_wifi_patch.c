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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_wifi.h"
#include "wpa_cli.h"
#include "driver_netlink.h"
#include "at_cmd_common.h"
#include "at_cmd_data_process.h"
#include "wifi_api.h"
#include "wpa_at_if.h"
#include "wlannetif.h"

#include "controller_wifi_com_patch.h"
#include "at_cmd_msg_ext.h"
#include "at_cmd_wifi_patch.h"
#include "at_cmd_task_patch.h"


#ifdef AT_CMD_WIFI_DBG
    #define AT_LOG                  printf
#else

    #define AT_LOG(...)
#endif

extern _at_command_t *_g_AtCmdTbl_Wifi_Ptr;
extern int g_wpa_mode;

uint8_t g_wifi_init_mode = 0;

/*
 * @brief Command at+cwmode
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwmode_patch(char *buf, int len, int mode)
{
    int mode_;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            msg_print_uart1("\r\n+CWMODE:%d\r\n", g_wifi_init_mode);
            msg_print_uart1("\r\nOK\r\n");
            break;

        case AT_CMD_MODE_EXECUTION:
            //Do nothing
            break;

        case AT_CMD_MODE_SET:
            if(argc > 1) {
                mode_ = atoi(argv[1]);
                //we only support station mode
                if (mode_ == 1)
                {
                    //wpa_set_wpa_mode(WPA_MODE_STA);
                    g_wifi_init_mode = WPA_MODE_STA;
                    
                    //Initialize AT task (TCPIP data task, event loop task)
                    at_wifi_net_task_init();
                    msg_print_uart1("\r\nOK\r\n");
                }
                else
                {
                    msg_print_uart1("\r\nERROR\r\n");
                }
            }
            break;

        case AT_CMD_MODE_TESTING:
            msg_print_uart1("\r\n+CWMODE:%d\r\n", g_wifi_init_mode);
            msg_print_uart1("\r\nOK\r\n");
            break;

        default:
            break;
    }

    return true;
}

/*
 * @brief Command at+cwjap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwjap_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    //char msg[128] = {0};
    int state;
    int rssi;
    u8 bssid[MAC_ADDR_LEN] = {0};
    u8 ssid[MAX_LEN_OF_SSID + 1] = {0};
    int freq;
    u8 ret = FALSE;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            state = wpas_get_state();

            if(state == WPA_COMPLETED || state == WPA_ASSOCIATED)
            {
                memset(bssid, 0, MAC_ADDR_LEN);
                memset(ssid, 0, MAX_LEN_OF_SSID + 1);
                wpa_cli_getssid(ssid);
                wpa_cli_getbssid(bssid);
                freq = wpas_get_assoc_freq_patch();
                rssi = wpa_driver_netlink_get_rssi();

                msg_print_uart1("\r\n+CWJAP:");
                msg_print_uart1("%s,", ssid);
                msg_print_uart1("%02x:%02x:%02x:%02x:%02x:%02x,", bssid[0], bssid[1], bssid[2],bssid[3], bssid[4], bssid[5]);
                msg_print_uart1("%d,", freq);
                msg_print_uart1("%d\r\n", rssi);
            }
            else
            {
                msg_print_uart1("\r\nNo AP connected\r\n");
            }
            msg_print_uart1("\r\nOK\r\n");
            break;

        case AT_CMD_MODE_EXECUTION:
            //Do nothing
            break;

        case AT_CMD_MODE_SET:
            if (argc == 3) {
                if (strlen(argv[2]) >= MAX_LEN_OF_PASSWD) {
                    msg_print_uart1("\r\n+CWJAP:%d\r\n", ERR_WIFI_CWJAP_PWD_INVALID);
                    msg_print_uart1("\r\nERROR\r\n");
                }
            }

            ret = wpa_cli_connect_handler(argc, argv);
            
            if (ret == FALSE) {
                _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_NO_AP);
            }
            
            break;

        case AT_CMD_MODE_TESTING:
            //Do nothing
            break;

        default:
            break;
    }

    return true;
}

/*
 * @brief Command at+cwautoconn
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwautoconn_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0; 
    u8 automode, ap_num; 
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            wifi_auto_connect_get_mode(&automode);
            wifi_auto_connect_get_ap_num(&ap_num);
            msg_print_uart1("\r\n+CWAUTOCONN:%d,%d\r\n", automode, ap_num);
            msg_print_uart1("\r\nOK\r\n");
            break;

        case AT_CMD_MODE_EXECUTION:
            break;

        case AT_CMD_MODE_SET:
            if (argc >= 2) { //mode
                automode = atoi(argv[1]);
                
                if (automode > AUTO_CONNECT_ENABLE) {
                    msg_print_uart1("\r\n+CWAUTOCONN:%d\r\n", ERR_WIFI_CWAUTOCONN_INVALID);
                    msg_print_uart1("\r\nERROR\r\n");
                    return FALSE;
                }
                
                wifi_auto_connect_set_mode(automode);
            }

            if (argc >= 3) { //AP number
                ap_num = atoi(argv[2]);

                if (ap_num == 0 || ap_num > MAX_NUM_OF_AUTO_CONNECT) {
                    msg_print_uart1("\r\n+CWAUTOCONN:%d\r\n", ERR_WIFI_CWAUTOCONN_INVALID);
                    msg_print_uart1("\r\nERROR\r\n");
                    return FALSE;
                }
                
                wifi_auto_connect_set_ap_num(ap_num);
            }
            
            msg_print_uart1("\r\nOK\r\n");

            break;

        case AT_CMD_MODE_TESTING:
            break;

        default:
            break;
    }

    return true;
}

/*
 * @brief Command at+wifi_mac_cfg
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_mac_cfg_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int cfg_id;
    int argc = 0;
    uint32_t skip_dtim = 0;
    u8 ret;
    bool bIsValid = FALSE;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            wifi_config_get_skip_dtim((uint8_t *)&skip_dtim);
            msg_print_uart1("\r\n+WIFIMACCFG:%d,%d\r\n", AT_WIFI_SKIP_DTIM_CFG, skip_dtim);

            msg_print_uart1("\r\nOK\r\n");
            break;
        case AT_CMD_MODE_SET:
            if(argc > 1) {
                cfg_id = atoi(argv[1]);

                if (cfg_id > AT_WIFI_MAX_NUM) {
                    msg_print_uart1("\r\n+CWWIFIMACCFG:%d\r\n", ERR_COMM_INVALID);
                    msg_print_uart1("\r\nERROR\r\n");
                    return false;
                }

                switch(cfg_id) {
                    case AT_WIFI_SKIP_DTIM_CFG:
                        skip_dtim = atoi(argv[2]);
                        if (skip_dtim <= WIFI_MAX_SKIP_DTIM_PERIODS_PATCH) {
                            //Update share memory by M0
                            ret =  wifi_config_set_skip_dtim(skip_dtim, true);
                            if (ret == 0) {
                                bIsValid = TRUE;
                            }
                        }
                        break;
                    default:
                        break;
                }
                
                if (bIsValid == TRUE) {
                    msg_print_uart1("\r\nOK\r\n");
                }
                else {
                    msg_print_uart1("\r\n+CWWIFIMACCFG:%d\r\n", ERR_COMM_INVALID);
                    msg_print_uart1("\r\nERROR\r\n");
                }
            }
            break;
        default:
            break;
    }

    return true;
}

/*
 * @brief Command at+cwhostname
 *
 * @param [in] buf: input string
 *
 * @param [in] len: length of input string
 *
 * @param [in] mode
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwhostname_patch(char *buf, int len, int mode)
{
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    char *sName = NULL;

    if(mode == AT_CMD_MODE_READ)
    {
        at_output("\r\n+CWHOPSTNAME:");

        if((g_wpa_mode == WPA_MODE_STA) || (g_wpa_mode == WPA_MODE_STA_AP))
        {
            at_output("\"%s\"", g_sLwipHostName);
        }
    }
    else if(mode == AT_CMD_MODE_SET)
    {
        if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
        {
            AT_LOG("[%s %d] _at_cmd_buf_to_argc_argv fail\n", __func__, __LINE__);
            goto done;
        }

        if(argc < 2)
        {
            AT_LOG("[%s %d] param not enough\n", __func__, __LINE__);
            goto done;
        }

        AT_LOG("[%s %d] argc[%d] argv1[%s]\n", __func__, __LINE__, argc, argv[1]);

        //sName = at_cmd_param_trim(argv[1]);
        sName = argv[1];

        if(!sName)
        {
            AT_LOG("[%s %d] invalid param\n", __func__, __LINE__);
            goto done;
        }

        if(strlen(sName) > MAX_NUM_OF_HOST_NAME_SIZE || 
           strlen(sName) == 0)
        {
            AT_LOG("[%s %d] invalid param\n", __func__, __LINE__);
            goto done;
        }

        if((g_wpa_mode == WPA_MODE_STA) || (g_wpa_mode == WPA_MODE_STA_AP))
        {
            snprintf(g_sLwipHostName, sizeof(g_sLwipHostName), "%s", sName);
        }
    }
    else
    {
        AT_LOG("[%s %d] invalid mode[%d]\n", __func__, __LINE__, mode);
        goto done;
    }

    iRet = 1;

done:
    if(iRet)
    {
        at_output("\r\nOK\r\n");
    }
    else
    {
        at_output("\r\nERROR\r\n");
    }

    return iRet;
}

int _at_cmd_wifi_cwlapopt_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    u8 argv1,argv2;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    argv1=atoi(argv[1]);
    argv2=atoi(argv[2]);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            break;

        case AT_CMD_MODE_EXECUTION:
            break;

        case AT_CMD_MODE_SET:
            if (argc != 3) break;
            if (argv1==0 || argv1==1)
            {   
                if (argv2>31)
                {
                    msg_print_uart1("\r\n invalid para: <mask> should be 0~31 \r\n");
                    break;
                }
                set_sorting(atoi(argv[1]), atoi(argv[2]));
                msg_print_uart1("\r\nOK\r\n");
                break;
            }
            else
            {
                msg_print_uart1("\r\n invalid para: <sort_enable> should be 0 or 1 \r\n");
                break;
            }

        case AT_CMD_MODE_TESTING:
            break;

        default:
            break;
    }

    return true;
}

/*
 * @brief AT Command Interface Initialization for Wi-Fi modules
 *
 */
#if defined(__AT_CMD_SUPPORT__)
void _at_cmd_wifi_func_init_patch(void)
{
    /** Command Table (Wi-Fi) */
    _g_AtCmdTbl_Wifi_Ptr[0].cmd_handle = _at_cmd_wifi_cwmode_patch;
    _g_AtCmdTbl_Wifi_Ptr[1].cmd_handle = _at_cmd_wifi_cwjap_patch;
    _g_AtCmdTbl_Wifi_Ptr[2].cmd_handle = _at_cmd_wifi_cwlapopt_patch;
    _g_AtCmdTbl_Wifi_Ptr[9].cmd_handle = _at_cmd_wifi_cwautoconn_patch;
    _g_AtCmdTbl_Wifi_Ptr[14].cmd_handle = _at_cmd_wifi_cwhostname_patch;
    _g_AtCmdTbl_Wifi_Ptr[51].cmd_handle = at_cmd_wifi_mac_cfg_patch;
    
}
#endif
