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
 * @file at_cmd_wifi.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File supports the Wi-Fi module AT Commands.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_wifi.h"
#include "wpa_cli.h"
#include "driver.h"
#include "driver_netlink.h"
#include "at_cmd_common.h"
#include "at_cmd_wifi.h"
#include "at_cmd.h"
#include "lwip/netif.h"
#include "wlannetif.h"
#include "at_cmd_tcpip.h"
#include "at_cmd_data_process.h"
#include "wpa_at_if.h"
#include "controller_wifi_com.h"
#include "at_cmd_msg_ext.h"
#include "wifi_nvm.h"

/** Here for now until needed in other places in lwIP */
#ifndef isprint
#define in_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#endif

//#define AT_CMD_WIFI_DBG

#ifdef AT_CMD_WIFI_DBG
    #define AT_LOG                  printf
#else

    #define AT_LOG(...)
#endif


char *g_wifi_argv[AT_MAX_CMD_ARGS];
int g_wifi_argc;
extern int g_wpa_mode;
extern struct wpa_config conf;

extern int wpas_get_state(void);
u8 g_skip_dtim = 0;

/*
 * @brief Check MAC string format
 *
 * @param [in] s MAC string
 *
 * @return 0 fail 1 success
 *
 */
int isMACValid(char *s) {
    int i;
    for(i = 0; i < 17; i++) {
        if(i % 3 != 2 && !isxdigit(s[i]))
            return 0;
        if(i % 3 == 2 && s[i] != ':')
            return 0;
    }
    if(s[17] != '\0')
        return 0;
    return true;
}

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
int _at_cmd_wifi_cwmode(char *buf, int len, int mode)
{
    int mode_;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            msg_print_uart1("\r\n+CWMODE:%d\r\n", g_wpa_mode);
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
                    g_wpa_mode = WPA_MODE_STA;
                    msg_print_uart1("\r\nOK\r\n");
                }
                else
                {
                    msg_print_uart1("\r\nERROR\r\n");
                }
            }
            break;

        case AT_CMD_MODE_TESTING:
            msg_print_uart1("\r\n+CWMODE:%d\r\n", g_wpa_mode);
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
int _at_cmd_wifi_cwjap(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    //char msg[128] = {0};
    int state;
    int rssi;
    u8 bssid[MAC_ADDR_LEN] = {0};
    u8 ssid[MAX_LEN_OF_SSID + 1] = {0};
    int freq;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            state = wpas_get_state();

            if(state == WPA_COMPLETED)
            {
                memset(bssid, 0, MAC_ADDR_LEN);
                memset(ssid, 0, MAX_LEN_OF_SSID + 1);
                wpa_cli_getssid(ssid);
                wpa_cli_getbssid(bssid);
                freq = wpas_get_assoc_freq();
                rssi = wpa_driver_netlink_get_rssi();

                msg_print_uart1("\r\n+CWJAP:");
                msg_print_uart1("%s,", ssid);
                msg_print_uart1("%x:%x:%x:%x:%x:%x,", bssid[0], bssid[1], bssid[2],bssid[3], bssid[4], bssid[5]);
                msg_print_uart1("%d,", freq);
                msg_print_uart1("%d\r\n", rssi);
            }
            else
            {
                msg_print_uart1("\r\n+CWJAP:\r\n");
            }
            msg_print_uart1("\r\nOK\r\n");
            break;

        case AT_CMD_MODE_EXECUTION:
            break;

        case AT_CMD_MODE_SET:
            wpa_cli_connect_handler(argc, argv);
            break;

        case AT_CMD_MODE_TESTING:
            break;

        default:
            break;
    }


    return true;
}

/*
 * @brief Command at+cwlapopt
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwlapopt(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            break;

        case AT_CMD_MODE_EXECUTION:
            break;

        case AT_CMD_MODE_SET:
            if(argc != 3) break;
            set_sorting(atoi(argv[1]), atoi(argv[2]));
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
 * @brief Command at+cwlap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwlap(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0, i, iRet;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    g_wifi_argc = argc;

    for (i=1; i<g_wifi_argc; i++)
    {
        if(argv[i])
        {
            if(g_wifi_argv[i])
            {
                free(g_wifi_argv[i]);
            }
            
            g_wifi_argv[i] = malloc(strlen(argv[i]) + 1);
            
            if(g_wifi_argv[i] == NULL)
            {
                goto done;
            }
            
            strcpy(g_wifi_argv[i], argv[i]);
        }
    }
    
    wpa_cli_scan_handler(0, NULL);

#if 0
    switch(mode)
    {
        case AT_CMD_MODE_READ:
            break;

        case AT_CMD_MODE_EXECUTION:
            //Show all APs
            wpa_cli_showscanresults_handler(argc, argv);
            break;

        case AT_CMD_MODE_SET:
            //Show the specified AP
            _at_msg_ext_wifi_show_one_ap_patch(argc, argv);
            break;

        case AT_CMD_MODE_TESTING:
            break;

        default:
            break;
    }


    msg_print_uart1("\r\nOK\r\n");
#endif    

done:
    if(!iRet)
    {
        for(i = 0; i < g_wifi_argc; i++)
        {
            if(g_wifi_argv[i])
            {
                free(g_wifi_argv[i]);
                g_wifi_argv[i] = NULL;
            }
        }
        
        g_wifi_argc = 0;
    }
    
    return iRet;
}

/*
 * @brief Command at+cwqap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwqap(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    wpa_cli_disconnect_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwsap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwsap(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwlif
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwlif(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwdhcp
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwdhcp(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwdhcps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwdhcps(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
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
int _at_cmd_wifi_cwautoconn(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0, i;
    u8 automode, ap_num, act_num = 0;
    MwFimAutoConnectCFG_t ac_cfg;
    auto_conn_info_t ac_info;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            msg_print_uart1("\r\n+CWAUTOCONN:%d,%d\r\n", get_auto_connect_mode(), get_auto_connect_save_ap_num());
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
                
                set_auto_connect_mode(automode);
                wifi_nvm_auto_connect_write(WIFI_NVM_ID_AUTO_CONNECT_MODE, 
                                            1,
                                            NULL,
                                            &automode);
            }

            if (argc >= 3) { //AP number
                ap_num = atoi(argv[2]);

                if (ap_num == 0 || ap_num > MAX_NUM_OF_AUTO_CONNECT) {
                    msg_print_uart1("\r\n+CWAUTOCONN:%d\r\n", ERR_WIFI_CWAUTOCONN_INVALID);
                    msg_print_uart1("\r\nERROR\r\n");
                    return FALSE;
                }

                /* ignore the same setting */
                if (get_auto_connect_ap_num() == ap_num) {
                    msg_print_uart1("\r\nOK\r\n");
                    return TRUE;
                }
               
                /* clear all AP info in FIM */
                MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 0);
                MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 1);
                MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 2);

                /* write AP info by new ap number setting */
                for (i=0; i<ap_num; i++) {
                    get_auto_connect_info(i, &ac_info);
                    if (ac_info.ap_channel != 0) {
                        wifi_nvm_auto_connect_write(WIFI_NVM_ID_AUTO_CONNECT_AP_INFO, 
                                            sizeof(mw_wifi_auto_connect_ap_info_t),
                                            i,
                                            (mw_wifi_auto_connect_ap_info_t *)&ac_info);
                        act_num++;
                    }
                }
                
                set_auto_connect_ap_num(act_num);
                wifi_nvm_auto_connect_write(WIFI_NVM_ID_AUTO_CONNECT_AP_NUM, 
                                            1,
                                            NULL,
                                            &act_num);

                /* clear all AP info in global variable */
                memset(&ac_info, 0, sizeof(auto_conn_info_t));
                for (i=0; i<MAX_NUM_OF_AUTO_CONNECT; i++) {
                    set_auto_connect_info(i, &ac_info);
                }

                /* read AP info from FIM */
                for (i=0; i<act_num; i++) {
                    wifi_nvm_auto_connect_read(WIFI_NVM_ID_AUTO_CONNECT_AP_INFO,
                                        sizeof(mw_wifi_auto_connect_ap_info_t),
                                        i,
                                        (mw_wifi_auto_connect_ap_info_t *)&ac_info);

                    set_auto_connect_info(i, &ac_info);
                }

                /* update ap cfg */
                get_auto_connect_ap_cfg(&ac_cfg);
                ac_cfg.max_save_num = ap_num;
                ac_cfg.flag = false;
                ac_cfg.front = -1;
                ac_cfg.rear = act_num-1;
                set_auto_connect_ap_cfg(&ac_cfg);
                wifi_nvm_auto_connect_write(WIFI_NVM_ID_AUTO_CONNECT_AP_CFG, 
                                            sizeof(MwFimAutoConnectCFG_t),
                                            NULL,
                                            &ac_cfg);

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
 * @brief Command at+cwfastconn
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwfastconn(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    u8 en, ap_idx;
    mw_wifi_auto_connect_ap_info_t info;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    
    switch(mode)
    {
        case AT_CMD_MODE_READ:
            msg_print_uart1("\r\n+CWFASTCONN:%d,%d,%d,%d,%d,%d\r\n",
                            0, get_fast_connect_mode(0),
                            1, get_fast_connect_mode(1),
                            2, get_fast_connect_mode(2));
            msg_print_uart1("\r\nOK\r\n");
            break;

        case AT_CMD_MODE_EXECUTION:
            break;

        case AT_CMD_MODE_SET:
            if (argc == 3) {
                ap_idx = atoi(argv[1]);
                en = atoi(argv[2]);

                if (ap_idx >= MAX_NUM_OF_AUTO_CONNECT || en > AUTO_CONNECT_ENABLE) {
                    msg_print_uart1("\r\n+CWFASTCONN:%d\r\n", ERR_WIFI_CWFASTCONN_INVALID);
                    msg_print_uart1("\r\nERROR\r\n");
                    return FALSE;
                }

                wifi_nvm_auto_connect_read(WIFI_NVM_ID_AUTO_CONNECT_AP_INFO,
                                            sizeof(mw_wifi_auto_connect_ap_info_t),
                                            ap_idx,
                                            &info);

                if(info.bssid[0] == 0 && info.bssid[1] == 0) {
                    msg_print_uart1("\r\n+CWFASTCONN:%d\r\n", ERR_WIFI_CWFASTCONN_AP_NULL);
                    msg_print_uart1("\r\nERROR\r\n");
                    return FALSE;
                }
                
                set_fast_connect_mode(ap_idx, en);
                info.fast_connect = en;
                wifi_nvm_auto_connect_write(WIFI_NVM_ID_AUTO_CONNECT_AP_INFO,
                                            sizeof(mw_wifi_auto_connect_ap_info_t),
                                            ap_idx,
                                            &info);

                msg_print_uart1("\r\nOK\r\n");
            }
            else {
                msg_print_uart1("\r\n+CWFASTCONN:%d\r\n", ERR_WIFI_CWFASTCONN_PARAMETER_TOO_FEW);
                msg_print_uart1("\r\nERROR\r\n");
            }
            
            break;

        case AT_CMD_MODE_TESTING:
            break;

        default:
            break;
    }

    //msg_print_uart1("\r\nOK\r\n");
    
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
int _at_cmd_wifi_cwhostname(char *buf, int len, int mode)
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

/*
 * @brief Command at+cwstartsmart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwstartsmart(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwstopsmart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwstopsmart(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wps(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwscan
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwscan(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    wpa_cli_scan_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwrec
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwrec(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    wpa_cli_reconnect_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwssta
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_cwssta(char *buf, int len, int mode)
{
    /** TBD, we can implement it until the file system is ready */
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiinit
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_start(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiend
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_end(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifirsv
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_rsv(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiver
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifiver(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+tftp_client_put
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_tftp_client_put(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+tftp_client_get
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_tftp_client_get(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+tftp_client_buf_show
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_tftp_client_buf_show(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+at_cmd_wifi_wifistatus
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifistatus(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+getrssi
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_getrssi(char *buf, int len, int mode)
{
    wpa_cli_getrssi();
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+joinap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_joinap(char *buf, int len, int mode)
{
    //Step1. Connect to AP

    //Step2. Get IP from DHCP server

    //msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+leaveap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_leaveap(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    wpa_cli_disconnect_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiscan
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifiscan(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    wpa_cli_scan_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+startap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_startap(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+stopap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_stopap(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifigettxpwr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifigettxpwr(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifisettxpwr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifisettxpwr(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifipwrsave
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifipwrsave(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiinit
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifiinit(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifidown
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifidown(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+sntpstart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_sntpstart(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+sntpstop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_sntpstop(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_connect
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifi_p2p_connect(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_disconnect
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_wifi_p2p_disconnect(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_scan
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_p2p_scan(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_go_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_p2p_go_start(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_go_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_p2p_go_stop(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_wps_reg_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_p2p_wps_reg_start(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_wps_reg_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_p2p_wps_reg_stop(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_easy_setup_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_easy_setup_start(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_easy_setup_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_easy_setup_stop(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+joinap_wps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_joinap_wps(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiscan_wps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifiscan_wps(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_ap_wps_reg_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_ap_wps_reg_stop(char *buf, int len, int mode)
{
    msg_print_uart1("\r\nOK\r\n");
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
int at_cmd_wifi_mac_cfg(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int cfg_id;
    int argc = 0;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            wpa_driver_netlink_sta_cfg(MLME_CMD_GET_PARAM, E_WIFI_PARAM_SKIP_DTIM_PERIODS, &g_skip_dtim);
            
            msg_print_uart1("\r\n+WIFIMACCFG:%d,%d\r\n", AT_WIFI_SKIP_DTIM_CFG, g_skip_dtim);
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

                //TODO
                //should not invoke function from driver_netlink

                switch(cfg_id) {
                    case AT_WIFI_SKIP_DTIM_CFG:
                        g_skip_dtim = atoi(argv[2]);

                        //if (skip_dtim > WIFI_MAX_SKIP_DTIM_PERIODS) {
                        //    msg_print_uart1("\r\n+CWWIFIMACCFG:%d\r\n", ERR_COMM_INVALID);
                        //    msg_print_uart1("\r\nERROR\r\n");
                        //    return false;     
                        //}
                        
                        //TODO
                        //Update FIM
                        wpa_driver_netlink_sta_cfg(MLME_CMD_SET_PARAM, E_WIFI_PARAM_SKIP_DTIM_PERIODS, &g_skip_dtim);
                        msg_print_uart1("\r\nOK\r\n");
                        break;
                    default:
                        msg_print_uart1("\r\n+CWWIFIMACCFG:%d\r\n", ERR_COMM_INVALID);
                        msg_print_uart1("\r\nERROR\r\n");
                        break;
                }
            }
            break;
        default:
            break;
    }

    return true;
}


/*
 * @brief Command Sample code to do Wi-Fi test
 *
 * @return 0 fail 1 success
 *
 */
int _at_cmd_wifi_sample(void)
{
    /** Scan */
    _at_cmd_wifi_cwscan(0, NULL, 0);

    /** List APs */
    _at_cmd_wifi_cwlap(0, NULL, 0);

    return true;
}

/**
  * @brief AT Command Table for Wi-Fi Module
  *
  */
_at_command_t _gAtCmdTbl_Wifi[] =
{
    { "at+cwmode",                  _at_cmd_wifi_cwmode,       "Wi-Fi mode" },
    { "at+cwjap",                   _at_cmd_wifi_cwjap,        "Connect to AP" },
    { "at+cwlapopt",                _at_cmd_wifi_cwlapopt,     "Configuration for at+cwlap" },
    { "at+cwlap",                   _at_cmd_wifi_cwlap,        "List available APs" },
    { "at+cwqap",                   _at_cmd_wifi_cwqap,        "Disconnect from AP" },
    { "at+cwsap",                   _at_cmd_wifi_cwsap,        "Configuration for softAP" },
    { "at+cwlif",                   _at_cmd_wifi_cwlif,        "Get station's IP which is connected to an AP" },
    { "at+cwdhcp",                  _at_cmd_wifi_cwdhcp,       "Configuration for DHCP" },
    { "at+cwdhcps",                 _at_cmd_wifi_cwdhcps,      "Set DHCP IP range of softAP to flash" },
    { "at+cwautoconn",              _at_cmd_wifi_cwautoconn,   "Connect to AP automatically when power on" },
    { "at+cwfastconn",              _at_cmd_wifi_cwfastconn,   "Connect to AP used fast connect mechanism" },
    //{ "at+cipstamac",               _at_cmd_wifi_cipstamac,    "Set MAC address of station " },
    //{ "at+cipapmac",                _at_cmd_wifi_cipstamac,    "Set MAC address of AP " },
    //{ "at+cipsta",                  _at_cmd_wifi_cipsta,       "Set Station IP" },
    //{ "at+cipap",                   _at_cmd_wifi_cipap,        "Set softAP IP" },
    { "at+cwstartsmart",            _at_cmd_wifi_cwstartsmart, "Start smart config" },
    { "at+cwstopsmart",             _at_cmd_wifi_cwstopsmart,  "Stop smart config" },
    { "at+wps",                     _at_cmd_wifi_wps,          "Set WPS" },
    { "at+cwhostname",              _at_cmd_wifi_cwhostname,   "Configures the Host Name of Station" },
    { "at+cwscan",                  _at_cmd_wifi_cwscan,       "Wi-Fi Scan" },
    { "at+cwrec",                   _at_cmd_wifi_cwrec,        "Reconnect to AP" },
    { "at+cwssta",                  _at_cmd_wifi_cwssta,       "Set configuration of station" },
    { "at+wifiinit",                _at_cmd_wifi_start,        "Wi-Fi Start" },    //Back Door
    { "at+wifiend",                 _at_cmd_wifi_end,          "Wi-Fi End" },      //Back Door
    { "at+wifirsv",                 _at_cmd_wifi_rsv,          "Wi-Fi Reserved" }, //Back Door
    { "at+wifiver",                 _at_cmd_wifi_wifiver,      "Show Wi-Fi FW version" },
    { "at+tftp_client_put",         _at_cmd_tftp_client_put,   "Send data to tftp server" },
    { "at+tftp_client_get",         _at_cmd_tftp_client_get,   "Get data from tftp server" },
    { "at+tftp_client_buf_show",    _at_cmd_tftp_client_buf_show, "Show buffer data" },
    { "at+wifistatus",              _at_cmd_wifi_wifistatus,   "Show AP/STA mode status" },
    { "at+getrssi",                 _at_cmd_wifi_getrssi,      "Get RSSI" },
    { "at+joinap",                  _at_cmd_wifi_joinap,       "Connect to AP" },
    { "at+leaveap",                 _at_cmd_wifi_leaveap,      "Disconnect an AP" },
    { "at+wifiscan",                _at_cmd_wifi_wifiscan,     "Do Wi-Fi Scan & show all APs" },
    { "at+startap",                 _at_cmd_wifi_startap,      "Start Wi-Fi AP" },
    { "at+stopap",                  _at_cmd_wifi_stopap,       "Stop Wi-Fi AP" },
    { "at+wifigettxpwr",            _at_cmd_wifi_wifigettxpwr, "Show Wi-Fi TX Power" },
    { "at+wifisettxpwr",            _at_cmd_wifi_wifisettxpwr, "Set Wi-Fi TX Power" },
    { "at+wifipwrsave",             _at_cmd_wifi_wifipwrsave,  "Wi-Fi module enter Powersave mode" },
    { "at+wifiinit",                _at_cmd_wifi_wifiinit,     "Turn on Wi-Fi" },
    { "at+wifidown",                _at_cmd_wifi_wifidown,     "Turn off Wi-Fi" },
    { "at+sntpstart",               _at_cmd_wifi_sntpstart,    "Start SNTP service" },
    { "at+sntpstop",                _at_cmd_wifi_sntpstop,     "Stop SNTP service" },
    { "at+wifi_p2p_connect",        _at_cmd_wifi_wifi_p2p_connect,   "connect to P2P Group Owner" },
    { "at+wifi_p2p_disconnect",     _at_cmd_wifi_wifi_p2p_disconnect,"P2P disconnect" },
    { "at+wifi_p2p_scan",           _at_cmd_wifi_p2p_scan,           "P2P scan" },
    { "at+wifi_p2p_go_start",       _at_cmd_wifi_p2p_go_start,       "Start P2P Group Owner" },
    { "at+wifi_p2p_go_stop",        _at_cmd_wifi_p2p_go_stop,        "Stop P2P Group Owner" },
    { "at+wifi_p2p_wps_reg_start",  _at_cmd_wifi_p2p_wps_reg_start,  "Start P2P Group Owner WPS" },
    { "at+wifi_p2p_wps_reg_stop",   _at_cmd_wifi_p2p_wps_reg_stop,   "Stop P2P Group Owner WPS" },
    { "at+wifi_easy_setup_start",   _at_cmd_wifi_easy_setup_start,   "" },
    { "at+wifi_easy_setup_stop",    _at_cmd_wifi_easy_setup_stop,    "" },
    { "at+joinap_wps",              _at_cmd_wifi_joinap_wps,         "Use WPS to connect the remote AP" },
    { "at+wifiscan_wps",            _at_cmd_wifiscan_wps,            "Show APs which can support WPS" },
    { "at+wifi_ap_wps_reg_stop",    _at_cmd_wifi_ap_wps_reg_stop,    "Show APs which can support WPS" },
    { "at+wifimaccfg",              at_cmd_wifi_mac_cfg,          "Set related to Wi-Fi mac configuration" },    
    { NULL,                         NULL,                           NULL},
};

/*
 * @brief Global variable g_AtCmdTbl_Wifi_Ptr retention attribute segment
 *
 */
RET_DATA _at_command_t *_g_AtCmdTbl_Wifi_Ptr;

/*
 * @brief AT Command Interface Initialization for Wi-Fi modules
 *
 */
void _at_cmd_wifi_func_init(void)
{
    g_wpa_mode = WPA_MODE_STA;
    /** Command Table (Wi-Fi) */
    _g_AtCmdTbl_Wifi_Ptr = _gAtCmdTbl_Wifi;

}

