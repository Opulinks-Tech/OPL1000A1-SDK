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
#include "at_cmd_app_patch.h"
#include "at_cmd_msg_ext.h"
#include "at_cmd_wifi_patch.h"
#include "at_cmd_task_patch.h"
#include "at_cmd_common_patch.h"
#include "at_cmd_nvm.h"
#include "wpa_cli_patch.h"

#ifdef AT_CMD_WIFI_DBG
    #define AT_LOG                  printf
#else

    #define AT_LOG(...)
#endif

#define AT_BLE_WIFI_MODE 4
#define AT_SCAN_PASSIVE_INT_DEF 150 //ms

extern _at_command_t *_g_AtCmdTbl_Wifi_Ptr;
extern int g_wpa_mode;
extern int g_wifi_argc;
extern char *g_wifi_argv[AT_MAX_CMD_ARGS];

uint8_t g_wifi_init_mode = 0;

S_WIFI_MLME_SCAN_CFG gATScanCfg = {0};

typedef struct {
    uint8_t sort_en;
    uint8_t mask;
    uint8_t times;
    uint8_t probe_counters;
} at_lap_opt_t;

at_lap_opt_t gATLapOpt = {0};

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
    int cwmode;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    uint8_t ret = false;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    
    switch(mode)
    {
        case AT_CMD_MODE_READ:
        {
            msg_print_uart1("\r\n+CWMODE:%d\r\n", g_wifi_init_mode);
            ret = true;
        }
            break;

        case AT_CMD_MODE_SET:
            if(argc > 1) {
                if (at_cmd_get_para_as_digital(argv[1], &cwmode) != 0) {
                    goto exit;
                }
                
                switch (cwmode)
                {
                    case 0:
                    {
                        if (at_cmd_nvm_cw_ble_wifi_mode_set((uint8_t *)&cwmode))
                            goto exit;
                        
                        g_wifi_init_mode = 0;
                        
                        ret = true;
                    }
                        break;
                    
                    case 1:
                        if ((g_wifi_init_mode == AT_BLE_WIFI_MODE))
                            goto exit;
                        
                        //wpa_set_wpa_mode(WPA_MODE_STA);
                        g_wifi_init_mode = WPA_MODE_STA;
                        
                        //Initialize AT task (TCPIP data task, event loop task)
                        at_wifi_net_task_init();
                        
                        ret = true;
                        break;
                        
                    case 4:
                    {
                        if (g_wifi_init_mode == WPA_MODE_STA)
                            goto exit;
                        
                        uint8_t enable = 1;
                        if (at_cmd_nvm_cw_ble_wifi_mode_set(&enable))
                            goto exit;
                        
                        g_wifi_init_mode = AT_BLE_WIFI_MODE;
                        
                        //Initialize BleWifi task
                        at_blewifi_init();

                        ret = true;
                    }
                        break;
                    default:
                        break;
                }
            }
            break;

        case AT_CMD_MODE_TESTING:
            msg_print_uart1("\r\n+CWMODE:%d\r\n", g_wifi_init_mode);
            ret = true;
            break;

        default:
            break;
    }

exit:
    if (ret == true)
    {
        at_cmd_crlf_term_set(1); // Enable CR-LF termination for WiFi AT commands

        msg_print_uart1("\r\nOK\r\n");
    }
    else 
        msg_print_uart1("\r\nERROR\r\n");

    return ret;
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
int _at_cmd_wifi_cwlap_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0, i, iRet = 0;
    uint8_t mac[MAC_ADDR_LEN] = {0};
    int ch;
    char *pstr;
    S_WIFI_MLME_SCAN_CFG scan_cfg;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    g_wifi_argc = 1;
    
    memcpy(&scan_cfg, &gATScanCfg, sizeof(S_WIFI_MLME_SCAN_CFG));
    
    switch(mode)
    {
        case AT_CMD_MODE_EXECUTION:
        case AT_CMD_MODE_READ:
        case AT_CMD_MODE_TESTING:
            break;
        
        case AT_CMD_MODE_SET:
            if (argc >= 2) { //ssid
                g_wifi_argc++;
                if (strlen(argv[1]) != 0) {
                    if (strlen(argv[1]) > MAX_LEN_OF_SSID) {
                        goto done;
                    }
                    
                    g_wifi_argv[1] = malloc(strlen(argv[1]) + 1);
                    if(g_wifi_argv[1] == NULL) {
                        goto done;
                    }
                    
                    strcpy(g_wifi_argv[1], argv[1]);
                }
            }
            
            if (argc >= 3) { //mac address
                g_wifi_argc++;
                if (strlen(argv[2]) != 0) {
                    pstr = at_cmd_param_trim(argv[2]);
                    
                    if (!pstr)
                    {
                        goto done;
                    }
                    
                    if (hwaddr_aton2(pstr, mac) == -1) {
                        goto done;
                    }
                
                    if (is_broadcast_ether_addr(mac)) {
                        goto done;
                    }
                    
                    if (is_multicast_ether_addr(mac)) {
                        goto done;
                    }

                    if (is_zero_ether_addr(mac)) {
                        goto done;
                    }
            
                    g_wifi_argv[2] = malloc(strlen(argv[2]) + 1);
                    if(g_wifi_argv[2] == NULL) {
                        goto done;
                    }
                    
                    strcpy(g_wifi_argv[2], argv[2]);
                }
            }
            
            if (argc >= 4) { //channel
#if 1
                g_wifi_argc++;
#endif
                if (at_cmd_get_para_as_digital(argv[3], &ch) != 0) {
                    goto done;
                }
                
                if (ch < 1 || ch > 14) {
                    goto done;
                }
#if 1
                g_wifi_argv[3] = malloc(strlen(argv[3]) + 1);
                if(g_wifi_argv[3] == NULL) {
                    goto done;
                }
                
                strcpy(g_wifi_argv[3], argv[3]);
#endif
                scan_cfg.u8Channel = ch;
            }
            break;
            
        default:
            break;
    }
    
    iRet = 1;
    wpa_cli_scan_by_cfg(&scan_cfg);
    
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
        
        at_output("\r\nERROR\r\n");
    }
    
    return iRet;
}

int _at_cmd_wifi_cwlapopt_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    int sort_en, mask, times, probe_counters;
    int iRet = 0;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            at_output("\r\n+CWLAPOPT:%d,%d,%d,%d\r\n", 
                      gATLapOpt.sort_en, gATLapOpt.mask, gATLapOpt.times,
                      gATLapOpt.probe_counters);
            iRet = 1;
            break;
        
        case AT_CMD_MODE_EXECUTION:
        case AT_CMD_MODE_TESTING:
            break;

        case AT_CMD_MODE_SET:
            if (argc >= 2) {
                if (at_cmd_get_para_as_digital(argv[1], &sort_en) != 0) {
                    goto done;
                }
                
                if (sort_en > 1 || sort_en < 0) {
                    AT_LOG("\r\n invalid para: <sort_enable> should be 0 or 1 \r\n");
                    goto done;
                }
                
                gATLapOpt.sort_en = sort_en;
                
                if (argc >= 3) {
                    if (at_cmd_get_para_as_digital(argv[2], &mask) != 0) {
                        goto done;
                    }
                    
                    if (mask > 31 || mask < 0) {
                        AT_LOG("\r\n invalid para: <mask> should be 0~31 \r\n");
                        goto done;
                    }
                    
                    gATLapOpt.mask = mask;
                }
                
                set_sorting(gATLapOpt.sort_en, gATLapOpt.mask);
            }

            if (argc >= 4) {
                if (at_cmd_get_para_as_digital(argv[3], &times) != 0) {
                    goto done;
                }
                
                if (times > 10 || times < 1) {
                    AT_LOG("\r\n invalid para: <times> should be 1~10 \r\n");
                    goto done;
                }
                
                gATLapOpt.times = times;
                gATScanCfg.u32PassiveScanDur = AT_SCAN_PASSIVE_INT_DEF * times;
                
            }
            
            if (argc >= 5) {
                if (at_cmd_get_para_as_digital(argv[4], &probe_counters) != 0) {
                    goto done;
                }
                
                if (probe_counters > 5 || probe_counters < 1) {
                    AT_LOG("\r\n invalid para: <probe_counters> should 1 ~ 5\r\n");
                    goto done;
                }
                else if (probe_counters > 1) {
                    gATScanCfg.u32PassiveScanDur = gATScanCfg.u32PassiveScanDur / probe_counters;
                    if (gATScanCfg.u32PassiveScanDur < SCAN_MIN_DURATION_TIME) {
                        gATScanCfg.u32PassiveScanDur = SCAN_MIN_DURATION_TIME;
                    }
                }
                
                gATLapOpt.probe_counters = probe_counters;
                gATScanCfg.u8ResendCnt = gATLapOpt.probe_counters;
            }
            
            iRet = 1;
            break;
        default:
            break;
    }

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
 * @brief AT Command Interface Initialization for Wi-Fi modules
 *
 */
#if defined(__AT_CMD_SUPPORT__)
void _at_cmd_wifi_func_init_patch(void)
{
    /* Initialize the configuartion. */
    gATScanCfg.tScanType = WIFI_MLME_SCAN_TYPE_MIX;
    gATScanCfg.u32ActiveScanDur = 100;
    gATScanCfg.u32PassiveScanDur = 150;
    gATScanCfg.u8Channel = 0;
    
    gATLapOpt.mask = 31;
    gATLapOpt.times = 1;
    gATLapOpt.probe_counters = 1;
    
    /** Command Table (Wi-Fi) */
    _g_AtCmdTbl_Wifi_Ptr[0].cmd_handle = _at_cmd_wifi_cwmode_patch;
    _g_AtCmdTbl_Wifi_Ptr[1].cmd_handle = _at_cmd_wifi_cwjap_patch;
    _g_AtCmdTbl_Wifi_Ptr[2].cmd_handle = _at_cmd_wifi_cwlapopt_patch;
    _g_AtCmdTbl_Wifi_Ptr[3].cmd_handle = _at_cmd_wifi_cwlap_patch;
    _g_AtCmdTbl_Wifi_Ptr[9].cmd_handle = _at_cmd_wifi_cwautoconn_patch;
    _g_AtCmdTbl_Wifi_Ptr[14].cmd_handle = _at_cmd_wifi_cwhostname_patch;
    _g_AtCmdTbl_Wifi_Ptr[51].cmd_handle = at_cmd_wifi_mac_cfg_patch;
    
}
#endif
