/*
 * WPA Supplicant - command line interface for wpa_supplicant daemon
 * Copyright (c) 2004-2010, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "wpa_ctrl.h"
#include "common.h"
#include "version.h"
#include "config.h"
#include "defs.h"
#include "wpa_cli.h"
#include "controller_wifi_com.h"
#include "msg.h"
#include "setest.h"
#include "config_ssid.h"
#include "wpa_supplicant_i.h"
#include "scrt.h"
#include "../src/common/wpa_common.h"
#include "../src/drivers/driver.h"
#include "../src/drivers/driver_netlink.h"
#include "at_cmd_msg_ext.h"
#include "wpa_demo.h"
#include "wpa_debug.h"
#include "wifi_nvm.h"

#ifndef WPA_CLI_DBG
#define WPA_CLI_DBG TRUE
#endif

#ifndef SIZE_T
#define SIZE_T
#endif

RET_DATA u8 g_bssid[6];// = {0};

RET_DATA struct wpa_scan_results res;
RET_DATA struct wpa_config conf;
RET_DATA struct wpa_config rec_conf;
RET_DATA struct wpa_ssid ssid_data;
RET_DATA struct wpa_ssid rec_ssid_data;

extern struct wpa_supplicant *wpa_s;
extern char g_passphrase[MAX_LEN_OF_PASSWD];
extern auto_connect_cfg_t g_AutoConnect;
extern u8 gAutoConnMode;
extern u8 gsta_cfg_mac[MAC_ADDR_LEN];

Boolean isMAC(char *s) {
    int i;
    for(i = 0; i < 17; i++) {
        if(i % 3 != 2 && !isxdigit(s[i]))
            return FALSE;
        if(i % 3 == 2 && s[i] != ':')
            return FALSE;
    }
    if(s[17] != '\0')
        return FALSE;
    return TRUE;
}

//The parameters are designed only for open system connection, not include the security connection
int wpa_cli_connect_impl(struct wpa_config * conf)
{
    if (conf == NULL) return FALSE;
    if (conf->ssid == NULL) return FALSE;
    wpa_driver_netlink_connect(conf);
    return TRUE;
}

int wpa_cli_connect_by_bssid_impl(u8 *bssid)
{
    if (bssid == NULL) return FALSE;
    wpa_driver_netlink_connect_by_bssid(bssid);
    return TRUE;
}

int wpa_cli_connect_handler_impl(int argc, char *argv[])
{
    u8 bssid[6] = {0};
    int i;
    char passwd[MAX_LEN_OF_PASSWD] = {0};
    int len_passwd = 0;
    int len_ssid = 0;

    if(argc <= 1) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: invalid parameter \r\n");
        return FALSE;
    }

    memset(&conf, 0, sizeof(conf));
    memset(&ssid_data, 0, sizeof(ssid_data));
    conf.ssid=&ssid_data;

    memset(g_passphrase, 0, MAX_LEN_OF_PASSWD);
    memset(bssid, 0, sizeof(bssid)/sizeof(bssid[0]));
    if (conf.ssid == NULL){
        return FALSE;
    }

    if(isMAC(argv[1])) //wpa_connect "bssid" "passphase"
    {
        //bssid
        hwaddr_aton2(argv[1], bssid);

        for (i=0; i<ETH_ALEN; i++)
        {
            conf.ssid->bssid[i] = bssid[i];
            g_bssid[i] = bssid[i];
        }

        wpa_printf(MSG_DEBUG, "[CLI]WPA: bssid buffer is ready \r\n");
        wpa_printf(MSG_DEBUG, "[CLI]WPA: connect bssid=%02x:%02x:%02x:%02x:%02x:%02x \r\n",
                                  bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

        //passwd
        if(argc >= 3)
        {
            len_passwd = os_strlen(argv[2]);
            //msg_print(LOG_HIGH_LEVEL, "strlen(argv[2]):%d \r\n", len_passwd);

            if ((len_passwd >= MAX_LEN_OF_PASSWD) || (len_passwd < MIN_LEN_OF_PASSWD))
            {
                msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: invalid parameter \r\n");
                return FALSE;
            }

            memset(passwd, 0, MAX_LEN_OF_PASSWD);
            os_memcpy(passwd, argv[2], len_passwd);

            //conf.ssid->passphrase = os_malloc(MAX_LEN_OF_PASSWD);
            //memset(conf.ssid->passphrase, 0, MAX_LEN_OF_PASSWD);
            if(len_passwd > 0)
            {
                //os_memcpy(conf.ssid->passphrase, passwd, len_passwd);
                os_memcpy((void *)g_passphrase, (void *)passwd, MAX_LEN_OF_PASSWD);
            }
        }

    }
    else //wpa_connect "ssid" "passphase" "bssid of hidden AP"
    {
        //ssid
        len_ssid = os_strlen(argv[1]);
        if(len_ssid > MAX_LEN_OF_SSID)
        {
            len_ssid = MAX_LEN_OF_SSID;
        }
        conf.ssid->ssid = os_malloc(MAX_LEN_OF_SSID + 1);
        memset(conf.ssid->ssid, 0, (MAX_LEN_OF_SSID + 1));
        os_memcpy(conf.ssid->ssid, argv[1], len_ssid);

        wpa_printf(MSG_DEBUG, "[CLI]WPA: connect ssid=");
        for(i=0;i<len_ssid;i++)
        {
            wpa_printf(MSG_DEBUG, "%c", conf.ssid->ssid[i]);
        }
        wpa_printf(MSG_DEBUG, "\r\n");

        //passwd
        if(argc >= 3)
        {
            len_passwd = os_strlen(argv[2]);
            //msg_print(LOG_HIGH_LEVEL, "strlen(argv[2]):%d \r\n", len_passwd);

            if ((len_passwd >= MAX_LEN_OF_PASSWD) || (len_passwd < MIN_LEN_OF_PASSWD))
            {
                msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: invalid parameter \r\n");
                return FALSE;
            }

            memset(passwd, 0, MAX_LEN_OF_PASSWD);
            os_memcpy(passwd, argv[2], len_passwd);

            //conf.ssid->passphrase = os_malloc(MAX_LEN_OF_PASSWD);
            //memset(conf.ssid->passphrase, 0, MAX_LEN_OF_PASSWD);
            if(len_passwd > 0)
            {
                //os_memcpy(conf.ssid->passphrase, passwd, len_passwd);
                os_memcpy((void *)g_passphrase, (void *)passwd, MAX_LEN_OF_PASSWD);
            }
        }

        //bssid for hidden ap connect
        if(argc >= 4)
        {
            if(isMAC(argv[3]))
            {
                hwaddr_aton2(argv[3], bssid);

                for (i=0; i<ETH_ALEN; i++)
                {
                    conf.ssid->bssid[i] = bssid[i];
                    g_bssid[i] = bssid[i];
                }
            }
        }
    }

    //For compatible auto/manual connect
    if (get_auto_connect_mode() == AUTO_CONNECT_ENABLE) {
        set_auto_connect_mode(AUTO_CONNECT_MANUAL);
    }

    wpa_cli_connect(&conf);

    if (conf.ssid->ssid) {
        os_free(conf.ssid->ssid);
        conf.ssid->ssid = NULL;
    }
    
    return TRUE;
}

extern void os_sleep(os_time_t sec, os_time_t usec);
/*
 * wpa_cli_connect_for_demo
 *
 * Do Wi-Fi connection (For Demo on Mid. Sep.)
 *
 * @param [in] bssid
 *    BSSID of AP.
 *
 * @param [in] ssid
 *    SSID of network.
 *
 * @param [in] ssid_len
 *    Size of ssid. [<=32]
 *
 * @param [in] pwd
 *    Password to connect to a secured AP.
 *
 * @param [in] MsgLen
 *    Size of password. [<=16]
 *
 */
int wpa_cli_connect_for_demo_impl(u8 *bssid, u8 *ssid, int ssid_len, u8 *pwd, int pwd_len)
{
#if 0
    struct wpa_config m_conf;
    struct wpa_ssid m_ssid_data;

    printf("\r\nwpa_cli_connect_for_demo\r\n");

    if (bssid == NULL) return FALSE;

    os_memset(&m_conf, 0, sizeof(m_conf));
    os_memset(&m_ssid_data, 0, sizeof(m_ssid_data));
    m_conf.ssid = &m_ssid_data;
    os_memcpy(m_conf.ssid->bssid, bssid, ETH_ALEN);

    printf("\r\nwpa_cli_connect_for_demo, call wpa_driver_netlink_connect\r\n");

    wpa_driver_netlink_connect(&m_conf);
#endif //#if 0

#if 0
    //Ignore Input Parameters

    u8 s_bssid[6]={0};
    u8 s_ssid[32]={0};
    int s_ssid_len;
    scan_info_t *pInfo2 = NULL;
    int i;

    printf("[DEMO][CLI]WPA: wpa_cli_connect_for_demo \r\n");
    os_memset(s_bssid, 0, 6);
    os_memset(s_ssid, 0, 32);

    wpa_get_conn_info_for_demo(0, s_ssid, &s_ssid_len, 0, 0);

    printf("[DEMO][CLI]WPA: wpa_cli_connect_for_demo, s_ssid:");
    for(i=0;i<32;i++) {
        printf("%c", s_ssid[i]);
    }
    printf("\r\n");

    pInfo2 = wifi_get_scan_record_by_ssid(s_ssid);
    if(pInfo2 == NULL) {
        msg_print(LOG_HIGH_LEVEL, "[DEMO][CLI]WPA: wpa_cli_connect_for_demo, pInfo2 == NULL \r\n");
        return FALSE;
    }
    msg_print(LOG_HIGH_LEVEL, "[DEMO][CLI]WPA: wpa_cli_connect_for_demo, find a matched AP \r\n");

    os_memcpy(s_bssid, pInfo2->bssid, 6);
    msg_print(LOG_HIGH_LEVEL, "[DEMO][CLI]WPA: wpa_cli_connect_for_demo, s_bssid=");
    for (i=0; i<6; i++) {
        msg_print(LOG_HIGH_LEVEL, "%x", s_bssid[i]);
    }

    msg_print(LOG_HIGH_LEVEL, "[DEMO][CLI]WPA: wpa_cli_connect_for_demo, call wifi_sta_join() \r\n");

    //connect by bssid
    wifi_sta_join(s_bssid);
#endif

    return TRUE;
}

int wpa_cli_scan_for_demo_impl(int mode)
{
#if 0
    printf("[DEMO][CLI]WPA: wpa_cli_scan_for_demo, mode:%d \r\n", mode);

    //1st demo scan
	wpa_driver_netlink_scan(mode);

    //2nd 3rd demo scan
    //wpa_cli_scan_for_demo_2(mode);
#endif

    return true;
}
//#endif //#ifdef DEMO_TC

int wpa_cli_disconnect_impl(const u8 *bssid, u16 reason_code)
{
	wpa_driver_netlink_disconnect(bssid, 0);
    return TRUE;
}

int wpa_cli_disconnect_handler_impl(int argc, char *argv[])
{
    wpa_cli_disconnect(NULL, 0);
    return TRUE;
}

int wpa_cli_reconnect_impl(struct wpa_config * conf)
{
    if (conf == NULL) return FALSE;
    if (conf->ssid == NULL) return FALSE;
    wpa_driver_netlink_reconnect(conf);
    return TRUE;
}

int wpa_cli_reconnect_handler_impl(int argc, char *argv[])
{
    int i;

    memset(&rec_conf, 0, sizeof(rec_conf));
    memset(&rec_ssid_data, 0, sizeof(rec_ssid_data));

    rec_conf.ssid=&rec_ssid_data;
    if(rec_conf.ssid == NULL) return FALSE;

    wpa_printf(MSG_DEBUG, "[CLI]WPA: bssid buffer is ready \r\n");
    for (i=0; i<ETH_ALEN; i++) {
        rec_conf.ssid->bssid[i] = g_bssid[i]; //conf.ssid->bssid => xxxxxxxxxxxx
    }
    wpa_printf(MSG_DEBUG, "[CLI]WPA: reconnect bssid=%02x:%02x:%02x:%02x:%02x:%02x \r\n",
                                rec_conf.ssid->bssid[0],
                                rec_conf.ssid->bssid[1],
                                rec_conf.ssid->bssid[2],
                                rec_conf.ssid->bssid[3],
                                rec_conf.ssid->bssid[4],
                                rec_conf.ssid->bssid[5]);
    wpa_cli_reconnect(&rec_conf);

    return 1;
}

int wpa_cli_scan_impl(int mode)
{
	return wpa_driver_netlink_scan(mode);
}

int wpa_cli_scan_handler_impl(int argc, char *argv[])
{
    int mode = SCAN_MODE_MIX;

    if(argc > 1) {
        mode = atoi(argv[1]);
        if (mode < SCAN_MODE_ACTIVE || mode > SCAN_MODE_MIX) {
            msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: invalid parameter \r\n");
            return FALSE;
        }
    }

    wpa_printf(MSG_DEBUG, "[CLI]WPA: scan mode=%d \r\n", mode);
    return wpa_cli_scan(mode);
}

int wpa_cli_getscanresults_impl(struct wpa_scan_results * res)
{
    if(res == NULL) return FALSE;
	return wpa_driver_netlink_get_scan_results(res);
}

void wpa_cli_showscanresults_handler_impl(int argc, char *argv[])
{
    #if 1
    wpa_driver_netlink_show_scan_results();
    #else
    memset(&res, 0, sizeof(res));
    wpa_cli_getscanresults(&res);
    #endif
}

void wpa_cli_getbssid_impl(u8 *bssid)
{
    if(bssid == NULL) return;
	wpa_driver_netlink_get_bssid(bssid);
}

void wpa_cli_showbssid_by_param_impl(int argc, char *argv[])
{
    u8 bssid[6] = {0};
    memset(bssid, 0, sizeof(bssid));
	wpa_driver_netlink_get_bssid(bssid);
    msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: bssid=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                              bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
}

void wpa_cli_getssid_impl(u8 *ssid)
{
    if(ssid == NULL) return;
	wpa_driver_netlink_get_ssid(ssid);
}

void wpa_cli_showssid_by_param_impl(int argc, char *argv[])
{
    u8 ssid[MAX_LEN_OF_SSID + 1] = {0};
    int i;

    memset(ssid, 0, sizeof(ssid));
    if(wpa_s->wpa_state == WPA_ASSOCIATED || wpa_s->wpa_state == WPA_COMPLETED)
    {
        memset(ssid, 0, sizeof(ssid));
        wpa_cli_getssid(ssid);
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: ssid=");
        for(i = 0; i < sizeof(ssid); i++)
        {
            msg_print(LOG_HIGH_LEVEL, "%c", ssid[i]);
        }
        msg_print(LOG_HIGH_LEVEL, "\r\n");
    }
}

void wpa_cli_getmac_impl(u8 *mac)
{
    if(mac == NULL) return;
	//wpa_driver_netlink_get_mac(mac);

    wpa_driver_netlink_sta_cfg(MLME_CMD_GET_PARAM, E_WIFI_PARAM_MAC_ADDRESS, mac);
    //wpa_driver_netlink_get_sta_cfg(E_WIFI_PARAM_MAC_ADDRESS, mac);
}

void wpa_cli_setmac_impl(u8 *mac)
{
    if(mac == NULL) return;
	//wpa_driver_netlink_set_mac(mac);

    if ((mac[0] == 0x00 && mac[1] == 0x00) ||
        (mac[0] == 0xFF) || (mac[0] == 0x01)) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: Invalid mac address \r\n");
        return;
    }
    
    if (wpa_s->wpa_state == WPA_COMPLETED || wpa_s->wpa_state == WPA_ASSOCIATED) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: Invalid wpa state \r\n");
        return;
    }
    
    memset(&gsta_cfg_mac[0], 0, MAC_ADDR_LEN);
    memcpy(&gsta_cfg_mac[0], &mac[0], MAC_ADDR_LEN);
    //wpa_driver_netlink_sta_cfg(MLME_CMD_SET_PARAM, E_WIFI_PARAM_MAC_ADDRESS, &gsta_cfg_mac[0]);

    wifi_nvm_sta_info_write(WIFI_NVM_STA_INFO_ID_MAC_ADDR, MAC_ADDR_LEN, gsta_cfg_mac);
}

void wpa_cli_mac_by_param_impl(int argc, char *argv[])
{
    u8 mac[6] = {0};

    if(argc == 1) //show mac
    {
        memset(mac, 0, sizeof(mac)/sizeof(mac[0]));
        wpa_cli_getmac(mac);
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: mac=%02x:%02x:%02x:%02x:%02x:%02x \r\n",
                                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else if (argc == 2) //set mac
    {
        memset(mac, 0, sizeof(mac)/sizeof(mac[0]));
        hwaddr_aton2(argv[1], mac);
        wpa_cli_setmac(mac);
    }
}

void wpa_cli_setdbgmode_by_param_impl(int argc, char *argv[])
{
    int mode;

    if(argc == 2) //set debug mode
    {
        mode = atoi(argv[1]);
        wpa_set_debug_mode(mode);
    }
}

int wpa_cli_getrssi_impl(void)
{
    int rssi = wpa_driver_netlink_get_rssi();
    msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: rssi=%d \r\n", rssi);

    /** MSG Extend for AT */
    at_msg_ext_wifi_get_rssi(AT_MSG_EXT_ESPRESSIF, rssi);

    return rssi;
}

extern void itp_scrt_test(void);
extern void aes_ccm_sample(void);
extern void aes_ecb_sample(void);
extern void hmac_sha_1_sample(void);

void wpa_cli_scrt_dbg_by_param_impl(int argc, char *argv[])
{
    itp_scrt_test();
    aes_ccm_sample();
    aes_ecb_sample();
    hmac_sha_1_sample();
}

void debug_auto_connect_impl(void)
{
    int i;
    MwFimAutoConnectCFG_t cfg;
    auto_conn_info_t info;
    
    msg_print(LOG_HIGH_LEVEL, "AP mode = %d\r\n", get_auto_connect_mode());
    msg_print(LOG_HIGH_LEVEL, "AP num = %d\r\n", get_auto_connect_ap_num());
    
    get_auto_connect_ap_cfg(&cfg);
    msg_print(LOG_HIGH_LEVEL, "AP cfg flag = %d, front = %d, rear = %d\r\n", cfg.flag, cfg.front, cfg.rear);
    msg_print(LOG_HIGH_LEVEL, "AP cfg max_save_num = %d\r\n", cfg.max_save_num);

    for (i=0; i<MAX_NUM_OF_AUTO_CONNECT; i++) {
        get_auto_connect_info(i, &info);
        msg_print(LOG_HIGH_LEVEL, "AP[%d] info channel = %d\r\n", i, info.ap_channel);
        msg_print(LOG_HIGH_LEVEL, "AP[%d] info fast mode = %d\r\n", i, info.fast_connect);
        msg_print(LOG_HIGH_LEVEL, "AP[%d] info bssid %02x %02x %02x %02x %02x %02x\r\n",
                    i, info.bssid[0], info.bssid[1], info.bssid[2], info.bssid[3], info.bssid[4],
                    info.bssid[5]);
        msg_print(LOG_HIGH_LEVEL, "AP[%d] info ssid = %s\r\n", i, info.ssid);
        msg_print(LOG_HIGH_LEVEL, "AP[%d] info psk = %02x %02x %02x %02x %02x\r\n",
                    i, info.psk[0], info.psk[1], info.psk[2], info.psk[3], info.psk[4]);
    }
}

int wpa_cli_conn_mode_impl(int argc, char *argv[])
{
    if (argc == 2) {

        s8 mode = atoi(argv[1]);

        if (mode < AUTO_CONNECT_DISABLE || mode > AUTO_CONNECT_ENABLE) {
            return FALSE;
        }

        wifi_nvm_auto_connect_write(WIFI_NVM_ID_AUTO_CONNECT_MODE, 1, NULL, &mode);
        gAutoConnMode = mode;
        g_AutoConnect.retryCount = 0; //restart fasct connect
        g_AutoConnect.targetIdx = 0;
        
        msg_print(LOG_HIGH_LEVEL, "connection mode = %d (0:disable, 1:fast connect)\r\n", get_auto_connect_mode());
        
        return TRUE;
    }
    
    return FALSE;
}

int wpa_cli_clear_ac_list_impl(int argc, char *argv[])
{
    // Reset Auto/Fast connect configuartion in the FIM
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_MODE, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_NUM, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_CFG, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 1);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 2);

    // Reset Auto/Fast connect configuration of global variables
    reset_auto_connect_list();

    return TRUE;
}

int wpa_cli_fast_connect_impl(int argc, char *argv[])
{
    u8 mode, ap_index;
    
    if(argc != 3) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: invalid parameter \r\n");
        return FALSE;
    }

    mode = atoi(argv[1]);
    ap_index = atoi(argv[2]);

    if (mode > AUTO_CONNECT_ENABLE || ap_index >= MAX_NUM_OF_AUTO_CONNECT) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: invalid parameter \r\n");
        return FALSE;
    }

    wpa_driver_netlink_fast_connect(mode, ap_index);
    
    return TRUE;
}

/* debug use */
int wpa_cli_dbg_impl(int argc, char *argv[])
{
    u8 mac[MAC_ADDR_LEN] = {0};
    
    if (!strcmp(argv[1], "h")) {
        msg_print(LOG_HIGH_LEVEL, "wpa debug :\r\n");
        msg_print(LOG_HIGH_LEVEL, "   h : help\r\n");
        msg_print(LOG_HIGH_LEVEL, "   p : print memory variable of auto connect/CBS ...\r\n");
        msg_print(LOG_HIGH_LEVEL, "   ia : Test input mac addr/manufacture name for CBS\r\n");
        return TRUE;
    }
    
    if (!strcmp(argv[1], "p") || argc == 1) {
        debug_auto_connect();
        return TRUE;
    }

    if (!strcmp(argv[1], "ia")) {
        if (argc >= 3) { // debug for CBS
            hwaddr_aton2(argv[2], mac);
            wpa_cli_setmac(mac);
        }
        
        if (argc >= 4) {
            wifi_nvm_sta_info_write(WIFI_NVM_STA_INFO_MANUFACTURE_NAME, STA_INFO_MAX_MANUF_NAME_SIZE, (u8 *)argv[3]);
        }
    }
    
    return TRUE;
}

uint32_t wpa_cli_cmd_handler_impl(int argc, char *argv[])
{
    if (argc < 1) return FALSE;
    msg_print(LOG_HIGH_LEVEL, "\r\n");

	if (os_strncasecmp(WPA_CLI_CMD_SCAN, argv[0], os_strlen(argv[0])) == 0){
        wpa_cli_scan_handler(argc, argv);
	} else if (os_strncasecmp(WPA_CLI_CMD_SCAN_RESULTS, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_showscanresults_handler(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_CONNECT, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_connect_handler(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_RECONNECT, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_reconnect_handler(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_DISCONNECT, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_disconnect_handler(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_BSSID, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_showbssid_by_param(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_SSID, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_showssid_by_param(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_HELP, argv[0], os_strlen(argv[0])) == 0) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: not support yet \r\n");
    } else if (os_strncasecmp(WPA_CLI_CMD_STATE, argv[0], os_strlen(argv[0])) == 0) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: state=%s \r\n", wpa_supplicant_state_txt(wpa_s->wpa_state));
    } else if (os_strncasecmp(WPA_CLI_CMD_ADD_NETWORK, argv[0], os_strlen(argv[0])) == 0) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: not support yet \r\n");
    } else if (os_strncasecmp(WPA_CLI_CMD_SET_NETWORK, argv[0], os_strlen(argv[0])) == 0) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: not support yet \r\n");
    } else if (os_strncasecmp(WPA_CLI_CMD_SAVE_CONFIG, argv[0], os_strlen(argv[0])) == 0) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: not support yet \r\n");
    } else if (os_strncasecmp(WPA_CLI_CMD_MAC, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_mac_by_param(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_SCRT_DEBUG, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_scrt_dbg_by_param(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_RSSI, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_getrssi();
    }

//#ifdef DEMO_TC
    else if (os_strncasecmp(WPA_CLI_CMD_DEMO_CONNECT, argv[0], os_strlen(argv[0])) == 0) {
        //u8 bssid_con[ETH_ALEN] = {0};
        //format: wpa_demo_connect
        //os_memset(bssid_con, 0, ETH_ALEN);
        //wpa_get_conn_info_for_demo(bssid_con, NULL, NULL, NULL, NULL);
        //printf("[CLI]WPA: Get WPA_CLI_CMD_DEMO_CONNECT, bssid_con[6]: %02x:%02x:%02x:%02x:%02x:%02x", bssid_con[0], bssid_con[1], bssid_con[2], bssid_con[3], bssid_con[4], bssid_con[5]);
        //if( (bssid_con[0] == 0) && (bssid_con[1] == 0)) return FALSE;
        //wpa_cli_connect_for_demo(bssid_con, NULL, 0, NULL, 0);

        //For Demo
        wpa_demo_connect(argc, argv);
    }
//#endif

    else if (os_strncasecmp(WPA_CLI_CMD_DBG_MODE, argv[0], os_strlen(WPA_CLI_CMD_DBG_MODE)) == 0) {
        wpa_cli_setdbgmode_by_param(argc, argv);
    }
    else if (os_strncasecmp(WPA_CLI_CMD_CONN_MODE, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_conn_mode(argc, argv);
    }
    else if (os_strncasecmp(WPA_CLI_CMD_CLEAR_AC_LIST, argv[0], os_strlen(argv[0])) == 0){
        wpa_cli_clear_ac_list(argc, argv);
    }
    else if (os_strncasecmp(WPA_CLI_CMD_FAST_CONNECT, argv[0], os_strlen(argv[0])) == 0){
        wpa_cli_fast_connect(argc, argv);
    }
    else if (os_strncasecmp(WPA_CLI_CMD_DBG, argv[0], os_strlen(WPA_CLI_CMD_DBG)) == 0){
        wpa_cli_dbg(argc, argv);
    }
    else {
        //nothing
        msg_print(LOG_HIGH_LEVEL, "\r\n");
    }

    return TRUE;
}


void wpa_cli_parse_impl(char* pszData) //pszData => wpa_xxx parm1 parm2 ...
{
    char *argv[WPA_CLI_CMD_NUM_MAX] = {0}; //argv[0]:wap_xxx argv[1]:parm1 argv[2]:parm3
    int count = 0;
    char *p = NULL;

    /* get the first word from the message, seperated by
          space character */
    p = strtok(pszData, " "); //word1: wpa_xxx
    argv[count] = p; //count = 0
    count++;

    /* the following loop gets the rest of the words until the
	 * end of the message */
	while ((p = strtok(NULL, " ")) != NULL){ //p: parmx
	    argv[count] = p;
        count++;
    }

    wpa_cli_cmd_handler(count, argv);
}


RET_DATA wpa_cli_connect_fp_t wpa_cli_connect;
RET_DATA wpa_cli_connect_by_bssid_fp_t wpa_cli_connect_by_bssid;
RET_DATA wpa_cli_connect_handler_fp_t wpa_cli_connect_handler;
RET_DATA wpa_cli_disconnect_fp_t wpa_cli_disconnect;
RET_DATA wpa_cli_disconnect_handler_fp_t wpa_cli_disconnect_handler;
RET_DATA wpa_cli_reconnect_fp_t wpa_cli_reconnect;
RET_DATA wpa_cli_reconnect_handler_fp_t wpa_cli_reconnect_handler;
RET_DATA wpa_cli_scan_fp_t wpa_cli_scan;
RET_DATA wpa_cli_scan_handler_fp_t wpa_cli_scan_handler;
RET_DATA wpa_cli_getscanresults_fp_t wpa_cli_getscanresults;
RET_DATA wpa_cli_showscanresults_handler_fp_t wpa_cli_showscanresults_handler;
RET_DATA wpa_cli_getbssid_fp_t wpa_cli_getbssid;
RET_DATA wpa_cli_showbssid_by_param_fp_t wpa_cli_showbssid_by_param;
RET_DATA wpa_cli_getssid_fp_t wpa_cli_getssid;
RET_DATA wpa_cli_showssid_by_param_fp_t wpa_cli_showssid_by_param;
RET_DATA wpa_cli_getmac_fp_t wpa_cli_getmac;
RET_DATA wpa_cli_mac_by_param_fp_t wpa_cli_mac_by_param;
RET_DATA wpa_cli_setmac_fp_t wpa_cli_setmac;
RET_DATA wpa_cli_parse_fp_t wpa_cli_parse;
RET_DATA wpa_cli_scrt_dbg_by_param_fp_t wpa_cli_scrt_dbg_by_param;
RET_DATA wpa_cli_cmd_handler_fp_t wpa_cli_cmd_handler;
RET_DATA wpa_cli_getrssi_fp_t wpa_cli_getrssi;
RET_DATA wpa_cli_setdbgmode_by_param_fp_t wpa_cli_setdbgmode_by_param;
RET_DATA debug_auto_connect_fp_t debug_auto_connect;
RET_DATA wpa_cli_conn_mode_fp_t wpa_cli_conn_mode;
RET_DATA wpa_cli_clear_ac_list_fp_t wpa_cli_clear_ac_list;
RET_DATA wpa_cli_fast_connect_fp_t wpa_cli_fast_connect;
RET_DATA wpa_cli_dbg_fp_t wpa_cli_dbg;

//#ifdef DEMO_TC
RET_DATA wpa_cli_connect_for_demo_fp_t wpa_cli_connect_for_demo;
RET_DATA wpa_cli_scan_for_demo_fp_t wpa_cli_scan_for_demo;
//#endif


/*
   Interface Initialization: WPA CLI
 */
void wpa_cli_func_init(void)
{
    wpa_cli_connect = wpa_cli_connect_impl;
    wpa_cli_connect_by_bssid = wpa_cli_connect_by_bssid_impl;
    wpa_cli_connect_handler = wpa_cli_connect_handler_impl;
    wpa_cli_disconnect = wpa_cli_disconnect_impl;
    wpa_cli_disconnect_handler = wpa_cli_disconnect_handler_impl;
    wpa_cli_reconnect = wpa_cli_reconnect_impl;
    wpa_cli_reconnect_handler = wpa_cli_reconnect_handler_impl;
    wpa_cli_scan = wpa_cli_scan_impl;
    wpa_cli_scan_handler = wpa_cli_scan_handler_impl;
    wpa_cli_getscanresults = wpa_cli_getscanresults_impl;
    wpa_cli_showscanresults_handler = wpa_cli_showscanresults_handler_impl;
    wpa_cli_getbssid = wpa_cli_getbssid_impl;
    wpa_cli_showbssid_by_param = wpa_cli_showbssid_by_param_impl;
    wpa_cli_getssid = wpa_cli_getssid_impl;
    wpa_cli_showssid_by_param = wpa_cli_showssid_by_param_impl;
    wpa_cli_getmac = wpa_cli_getmac_impl;
    wpa_cli_mac_by_param = wpa_cli_mac_by_param_impl;
    wpa_cli_setmac = wpa_cli_setmac_impl;
    wpa_cli_parse = wpa_cli_parse_impl;
    wpa_cli_scrt_dbg_by_param = wpa_cli_scrt_dbg_by_param_impl;
    wpa_cli_cmd_handler = wpa_cli_cmd_handler_impl;
    wpa_cli_getrssi = wpa_cli_getrssi_impl;
    wpa_cli_setdbgmode_by_param = wpa_cli_setdbgmode_by_param_impl;
    debug_auto_connect = debug_auto_connect_impl;
    wpa_cli_conn_mode = wpa_cli_conn_mode_impl;
    wpa_cli_clear_ac_list = wpa_cli_clear_ac_list_impl;
    wpa_cli_fast_connect  = wpa_cli_fast_connect_impl;
    wpa_cli_dbg           = wpa_cli_dbg_impl;
    
//#ifdef DEMO_TC
    wpa_cli_connect_for_demo = wpa_cli_connect_for_demo_impl;
    wpa_cli_scan_for_demo = wpa_cli_scan_for_demo_impl;
//#endif

    
}


#if 0
//struct wpa_ctrl *ctrl_conn;
//struct wpa_ctrl *mon_conn;
//const char *ctrl_iface_dir = "/var/run/wpa_supplicant";
//RET_DATA char *ctrl_ifname;
//RET_DATA int wpa_cli_quit;
//RET_DATA int wpa_cli_attached;
//RET_DATA int wpa_cli_connected;
//RET_DATA int wpa_cli_last_id;
//RET_DATA const char *pid_file;
//RET_DATA const char *action_file;
//RET_DATA int ping_interval;// = 5;
//RET_DATA int interactive;
void print_help();
#endif


#if 0
void usage(void)
{
	printf("wpa_cli [-p<path to ctrl sockets>] [-i<ifname>] [-hvB] "
	       "[-a<action file>] \\\n"
	       "        [-P<pid file>] [-g<global ctrl>] [-G<ping interval>]  "
	       "[command..]\n"
	       "  -h = help (show this usage text)\n"
	       "  -v = shown version information\n"
	       "  -a = run in daemon mode executing the action file based on "
	       "events from\n"
	       "       wpa_supplicant\n"
	       "  -B = run a daemon in the background\n"
	       "  default path: /var/run/wpa_supplicant\n"
	       "  default interface: first interface found in socket path\n");
	print_help();
}
#endif


#if 0
int wpa_cli_open_connection(const char *ifname, int attach)
{
	char *cfile;
	int flen, res;

	if (ifname == NULL)
		return -1;

	flen = os_strlen(ctrl_iface_dir) + os_strlen(ifname) + 2;
	cfile = os_malloc(flen);
	if (cfile == NULL)
		return -1L;
	res = os_snprintf(cfile, flen, "%s/%s", ctrl_iface_dir, ifname);
	if (res < 0 || res >= flen) {
		os_free(cfile);
		return -1;
	}

	ctrl_conn = wpa_ctrl_open(cfile);
	if (ctrl_conn == NULL) {
		os_free(cfile);
		return -1;
	}

	if (attach && interactive)
		mon_conn = wpa_ctrl_open(cfile);
	else
		mon_conn = NULL;
	os_free(cfile);


	if (mon_conn) {
		if (wpa_ctrl_attach(mon_conn) == 0) {
			wpa_cli_attached = 1;
		} else {
			printf("Warning: Failed to attach to "
			       "wpa_supplicant.\n");
			return -1;
		}
	}

	return 0;
}
#endif


#if 0
void wpa_cli_close_connection(void)
{
	if (ctrl_conn == NULL)
		return;

	if (wpa_cli_attached) {
		wpa_ctrl_detach(interactive ? mon_conn : ctrl_conn);
		wpa_cli_attached = 0;
	}
	wpa_ctrl_close(ctrl_conn);
	ctrl_conn = NULL;
	if (mon_conn) {
		wpa_ctrl_close(mon_conn);
		mon_conn = NULL;
	}
}
#endif


#if 0
void wpa_cli_msg_cb(char *msg, size_t len)
{
	printf("%s\n", msg);
}
#endif


#if 0
int _wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, int print)
{
	char buf[2048];
	size_t len;
	int ret;

	if (ctrl_conn == NULL) {
		printf("Not connected to wpa_supplicant - command dropped.\n");
		return -1;
	}
	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(ctrl, cmd, os_strlen(cmd), buf, &len,
			       wpa_cli_msg_cb);
	if (ret == -2) {
		printf("'%s' command timed out.\n", cmd);
		return -2;
	} else if (ret < 0) {
		printf("'%s' command failed.\n", cmd);
		return -1;
	}
	if (print) {
		buf[len] = '\0';
		printf("%s", buf);
	}

	return 0;
}
#endif


#if 0
int wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd)
{
	return _wpa_ctrl_command(ctrl, cmd, 1);
}
#endif


#if 0
int wpa_cli_cmd_status(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	int verbose = argc > 0 && os_strcmp(argv[0], "verbose") == 0;
	return wpa_ctrl_command(ctrl, verbose ? "STATUS-VERBOSE" : "STATUS");
}
#endif


#if 0
int wpa_cli_cmd_ping(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	return wpa_ctrl_command(ctrl, "PING");
}
#endif


#if 0
int wpa_cli_cmd_mib(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	return wpa_ctrl_command(ctrl, "MIB");
}
#endif


#if 0
int wpa_cli_cmd_pmksa(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	return wpa_ctrl_command(ctrl, "PMKSA");
}
#endif


#if 0
int wpa_cli_cmd_help(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	print_help();
	return 0;
}
#endif


#if 0
int wpa_cli_cmd_license(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	printf("%s\n\n%s\n", wpa_cli_version, wpa_cli_full_license);
	return 0;
}
#endif


#if 0
int wpa_cli_cmd_quit(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	wpa_cli_quit = 1;
	return 0;
}
#endif


#if 0
int wpa_cli_cmd_set(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc == 0) {
		wpa_cli_show_variables();
		return 0;
	}

	if (argc != 2) {
		printf("Invalid SET command: needs two arguments (variable "
		       "name and value)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "SET %s %s", argv[0], argv[1]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long SET command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_reassociate(struct wpa_ctrl *ctrl, int argc,
				   char *argv[])
{
	return wpa_ctrl_command(ctrl, "REASSOCIATE");
}
#endif


#if 0
int wpa_cli_cmd_preauthenticate(struct wpa_ctrl *ctrl, int argc,
				       char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 1) {
		printf("Invalid PREAUTH command: needs one argument "
		       "(BSSID)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "PREAUTH %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long PREAUTH command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_ap_scan(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 1) {
		printf("Invalid AP_SCAN command: needs one argument (ap_scan "
		       "value)\n");
		return -1;
	}
	res = os_snprintf(cmd, sizeof(cmd), "AP_SCAN %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long AP_SCAN command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_stkstart(struct wpa_ctrl *ctrl, int argc,
				char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 1) {
		printf("Invalid STKSTART command: needs one argument "
		       "(Peer STA MAC address)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "STKSTART %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long STKSTART command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_ft_ds(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 1) {
		printf("Invalid FT_DS command: needs one argument "
		       "(Target AP MAC address)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "FT_DS %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long FT_DS command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_wps_pbc(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc == 0) {
		/* Any BSSID */
		return wpa_ctrl_command(ctrl, "WPS_PBC");
	}

	/* Specific BSSID */
	res = os_snprintf(cmd, sizeof(cmd), "WPS_PBC %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long WPS_PBC command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_wps_pin(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc == 0) {
		printf("Invalid WPS_PIN command: need one or two arguments:\n"
		       "- BSSID: use 'any' to select any\n"
		       "- PIN: optional, used only with devices that have no "
		       "display\n");
		return -1;
	}

	if (argc == 1) {
		/* Use dynamically generated PIN (returned as reply) */
		res = os_snprintf(cmd, sizeof(cmd), "WPS_PIN %s", argv[0]);
		if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
			printf("Too long WPS_PIN command.\n");
			return -1;
		}
		return wpa_ctrl_command(ctrl, cmd);
	}

	/* Use hardcoded PIN from a label */
	res = os_snprintf(cmd, sizeof(cmd), "WPS_PIN %s %s", argv[0], argv[1]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long WPS_PIN command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#ifdef CONFIG_WPS_OOB
#if 0
int wpa_cli_cmd_wps_oob(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 3 && argc != 4) {
		printf("Invalid WPS_OOB command: need three or four "
		       "arguments:\n"
		       "- DEV_TYPE: use 'ufd' or 'nfc'\n"
		       "- PATH: path of OOB device like '/mnt'\n"
		       "- METHOD: OOB method 'pin-e' or 'pin-r', "
		       "'cred'\n"
		       "- DEV_NAME: (only for NFC) device name like "
		       "'pn531'\n");
		return -1;
	}

	if (argc == 3)
		res = os_snprintf(cmd, sizeof(cmd), "WPS_OOB %s %s %s",
				  argv[0], argv[1], argv[2]);
	else
		res = os_snprintf(cmd, sizeof(cmd), "WPS_OOB %s %s %s %s",
				  argv[0], argv[1], argv[2], argv[3]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long WPS_OOB command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif
#endif /* CONFIG_WPS_OOB */


#if 0
int wpa_cli_cmd_wps_reg(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc == 2)
		res = os_snprintf(cmd, sizeof(cmd), "WPS_REG %s %s",
				  argv[0], argv[1]);
	else if (argc == 6) {
		char ssid_hex[2 * 32 + 1];
		char key_hex[2 * 64 + 1];
		int i;

		ssid_hex[0] = '\0';
		for (i = 0; i < 32; i++) {
			if (argv[2][i] == '\0')
				break;
			os_snprintf(&ssid_hex[i * 2], 3, "%02x", argv[2][i]);
		}

		key_hex[0] = '\0';
		for (i = 0; i < 64; i++) {
			if (argv[5][i] == '\0')
				break;
			os_snprintf(&key_hex[i * 2], 3, "%02x", argv[5][i]);
		}

		res = os_snprintf(cmd, sizeof(cmd),
				  "WPS_REG %s %s %s %s %s %s",
				  argv[0], argv[1], ssid_hex, argv[3], argv[4],
				  key_hex);
	} else {
		printf("Invalid WPS_REG command: need two arguments:\n"
		       "- BSSID: use 'any' to select any\n"
		       "- AP PIN\n");
		printf("Alternatively, six arguments can be used to "
		       "reconfigure the AP:\n"
		       "- BSSID: use 'any' to select any\n"
		       "- AP PIN\n"
		       "- new SSID\n"
		       "- new auth (OPEN, WPAPSK, WPA2PSK)\n"
		       "- new encr (NONE, WEP, TKIP, CCMP)\n"
		       "- new key\n");
		return -1;
	}

	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long WPS_REG command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_wps_er_start(struct wpa_ctrl *ctrl, int argc,
				    char *argv[])
{
	return wpa_ctrl_command(ctrl, "WPS_ER_START");

}
#endif


#if 0
int wpa_cli_cmd_wps_er_stop(struct wpa_ctrl *ctrl, int argc,
				   char *argv[])
{
	return wpa_ctrl_command(ctrl, "WPS_ER_STOP");

}
#endif


#if 0
int wpa_cli_cmd_wps_er_pin(struct wpa_ctrl *ctrl, int argc,
				  char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 2) {
		printf("Invalid WPS_ER_PIN command: need two arguments:\n"
		       "- UUID: use 'any' to select any\n"
		       "- PIN: Enrollee PIN\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "WPS_ER_PIN %s %s",
			  argv[0], argv[1]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long WPS_ER_PIN command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_wps_er_pbc(struct wpa_ctrl *ctrl, int argc,
				  char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 1) {
		printf("Invalid WPS_ER_PBC command: need one argument:\n"
		       "- UUID: Specify the Enrollee\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "WPS_ER_PBC %s",
			  argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long WPS_ER_PBC command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_wps_er_learn(struct wpa_ctrl *ctrl, int argc,
				    char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 2) {
		printf("Invalid WPS_ER_LEARN command: need two arguments:\n"
		       "- UUID: specify which AP to use\n"
		       "- PIN: AP PIN\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "WPS_ER_LEARN %s %s",
			  argv[0], argv[1]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long WPS_ER_LEARN command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_ibss_rsn(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 1) {
		printf("Invalid IBSS_RSN command: needs one argument "
		       "(Peer STA MAC address)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "IBSS_RSN %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long IBSS_RSN command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_level(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256];
	int res;

	if (argc != 1) {
		printf("Invalid LEVEL command: needs one argument (debug "
		       "level)\n");
		return -1;
	}
	res = os_snprintf(cmd, sizeof(cmd), "LEVEL %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long LEVEL command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_identity(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256], *pos, *end;
	int i, ret;

	if (argc < 2) {
		printf("Invalid IDENTITY command: needs two arguments "
		       "(network id and identity)\n");
		return -1;
	}

	end = cmd + sizeof(cmd);
	pos = cmd;
	ret = os_snprintf(pos, end - pos, WPA_CTRL_RSP "IDENTITY-%s:%s",
			  argv[0], argv[1]);
	if (ret < 0 || ret >= end - pos) {
		printf("Too long IDENTITY command.\n");
		return -1;
	}
	pos += ret;
	for (i = 2; i < argc; i++) {
		ret = os_snprintf(pos, end - pos, " %s", argv[i]);
		if (ret < 0 || ret >= end - pos) {
			printf("Too long IDENTITY command.\n");
			return -1;
		}
		pos += ret;
	}

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_password(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256], *pos, *end;
	int i, ret;

	if (argc < 2) {
		printf("Invalid PASSWORD command: needs two arguments "
		       "(network id and password)\n");
		return -1;
	}

	end = cmd + sizeof(cmd);
	pos = cmd;
	ret = os_snprintf(pos, end - pos, WPA_CTRL_RSP "PASSWORD-%s:%s",
			  argv[0], argv[1]);
	if (ret < 0 || ret >= end - pos) {
		printf("Too long PASSWORD command.\n");
		return -1;
	}
	pos += ret;
	for (i = 2; i < argc; i++) {
		ret = os_snprintf(pos, end - pos, " %s", argv[i]);
		if (ret < 0 || ret >= end - pos) {
			printf("Too long PASSWORD command.\n");
			return -1;
		}
		pos += ret;
	}

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_new_password(struct wpa_ctrl *ctrl, int argc,
				    char *argv[])
{
	char cmd[256], *pos, *end;
	int i, ret;

	if (argc < 2) {
		printf("Invalid NEW_PASSWORD command: needs two arguments "
		       "(network id and password)\n");
		return -1;
	}

	end = cmd + sizeof(cmd);
	pos = cmd;
	ret = os_snprintf(pos, end - pos, WPA_CTRL_RSP "NEW_PASSWORD-%s:%s",
			  argv[0], argv[1]);
	if (ret < 0 || ret >= end - pos) {
		printf("Too long NEW_PASSWORD command.\n");
		return -1;
	}
	pos += ret;
	for (i = 2; i < argc; i++) {
		ret = os_snprintf(pos, end - pos, " %s", argv[i]);
		if (ret < 0 || ret >= end - pos) {
			printf("Too long NEW_PASSWORD command.\n");
			return -1;
		}
		pos += ret;
	}

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_pin(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256], *pos, *end;
	int i, ret;

	if (argc < 2) {
		printf("Invalid PIN command: needs two arguments "
		       "(network id and pin)\n");
		return -1;
	}

	end = cmd + sizeof(cmd);
	pos = cmd;
	ret = os_snprintf(pos, end - pos, WPA_CTRL_RSP "PIN-%s:%s",
			  argv[0], argv[1]);
	if (ret < 0 || ret >= end - pos) {
		printf("Too long PIN command.\n");
		return -1;
	}
	pos += ret;
	for (i = 2; i < argc; i++) {
		ret = os_snprintf(pos, end - pos, " %s", argv[i]);
		if (ret < 0 || ret >= end - pos) {
			printf("Too long PIN command.\n");
			return -1;
		}
		pos += ret;
	}
	return wpa_ctrl_command(ctrl, cmd);

}
#endif


#if 0
int wpa_cli_cmd_otp(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256], *pos, *end;
	int i, ret;

	if (argc < 2) {
		printf("Invalid OTP command: needs two arguments (network "
		       "id and password)\n");
		return -1;
	}

	end = cmd + sizeof(cmd);
	pos = cmd;
	ret = os_snprintf(pos, end - pos, WPA_CTRL_RSP "OTP-%s:%s",
			  argv[0], argv[1]);
	if (ret < 0 || ret >= end - pos) {
		printf("Too long OTP command.\n");
		return -1;
	}
	pos += ret;
	for (i = 2; i < argc; i++) {
		ret = os_snprintf(pos, end - pos, " %s", argv[i]);
		if (ret < 0 || ret >= end - pos) {
			printf("Too long OTP command.\n");
			return -1;
		}
		pos += ret;
	}

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_passphrase(struct wpa_ctrl *ctrl, int argc,
				  char *argv[])
{
	char cmd[256], *pos, *end;
	int i, ret;

	if (argc < 2) {
		printf("Invalid PASSPHRASE command: needs two arguments "
		       "(network id and passphrase)\n");
		return -1;
	}

	end = cmd + sizeof(cmd);
	pos = cmd;
	ret = os_snprintf(pos, end - pos, WPA_CTRL_RSP "PASSPHRASE-%s:%s",
			  argv[0], argv[1]);
	if (ret < 0 || ret >= end - pos) {
		printf("Too long PASSPHRASE command.\n");
		return -1;
	}
	pos += ret;
	for (i = 2; i < argc; i++) {
		ret = os_snprintf(pos, end - pos, " %s", argv[i]);
		if (ret < 0 || ret >= end - pos) {
			printf("Too long PASSPHRASE command.\n");
			return -1;
		}
		pos += ret;
	}

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_bssid(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[256], *pos, *end;
	int i, ret;

	if (argc < 2) {
		printf("Invalid BSSID command: needs two arguments (network "
		       "id and BSSID)\n");
		return -1;
	}

	end = cmd + sizeof(cmd);
	pos = cmd;
	ret = os_snprintf(pos, end - pos, "BSSID");
	if (ret < 0 || ret >= end - pos) {
		printf("Too long BSSID command.\n");
		return -1;
	}
	pos += ret;
	for (i = 0; i < argc; i++) {
		ret = os_snprintf(pos, end - pos, " %s", argv[i]);
		if (ret < 0 || ret >= end - pos) {
			printf("Too long BSSID command.\n");
			return -1;
		}
		pos += ret;
	}

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_list_networks(struct wpa_ctrl *ctrl, int argc,
				     char *argv[])
{
	return wpa_ctrl_command(ctrl, "LIST_NETWORKS");
}
#endif


#if 0
int wpa_cli_cmd_select_network(struct wpa_ctrl *ctrl, int argc,
				      char *argv[])
{
	char cmd[32];
	int res;

	if (argc < 1) {
		printf("Invalid SELECT_NETWORK command: needs one argument "
		       "(network id)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "SELECT_NETWORK %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_enable_network(struct wpa_ctrl *ctrl, int argc,
				      char *argv[])
{
	char cmd[32];
	int res;

	if (argc < 1) {
		printf("Invalid ENABLE_NETWORK command: needs one argument "
		       "(network id)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_disable_network(struct wpa_ctrl *ctrl, int argc,
				       char *argv[])
{
	char cmd[32];
	int res;

	if (argc < 1) {
		printf("Invalid DISABLE_NETWORK command: needs one argument "
		       "(network id)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "DISABLE_NETWORK %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_add_network(struct wpa_ctrl *ctrl, int argc,
				   char *argv[])
{
	return wpa_ctrl_command(ctrl, "ADD_NETWORK");
}
#endif


#if 0
int wpa_cli_cmd_remove_network(struct wpa_ctrl *ctrl, int argc,
				      char *argv[])
{
	char cmd[32];
	int res;

	if (argc < 1) {
		printf("Invalid REMOVE_NETWORK command: needs one argument "
		       "(network id)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "REMOVE_NETWORK %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
void wpa_cli_show_network_variables(void)
{
	printf("set_network variables:\n"
	       "  ssid (network name, SSID)\n"
	       "  psk (WPA passphrase or pre-shared key)\n"
	       "  key_mgmt (key management protocol)\n"
	       "  identity (EAP identity)\n"
	       "  password (EAP password)\n"
	       "  ...\n"
	       "\n"
	       "Note: Values are entered in the same format as the "
	       "configuration file is using,\n"
	       "i.e., strings values need to be inside double quotation "
	       "marks.\n"
	       "For example: set_network 1 ssid \"network name\"\n"
	       "\n"
	       "Please see wpa_supplicant.conf documentation for full list "
	       "of\navailable variables.\n");
}
#endif


#if 0
int wpa_cli_cmd_set_network(struct wpa_ctrl *ctrl, int argc,
				   char *argv[])
{
	char cmd[256];
	int res;

	if (argc == 0) {
		wpa_cli_show_network_variables();
		return 0;
	}

	if (argc != 3) {
		printf("Invalid SET_NETWORK command: needs three arguments\n"
		       "(network id, variable name, and value)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "SET_NETWORK %s %s %s",
			  argv[0], argv[1], argv[2]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long SET_NETWORK command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_get_network(struct wpa_ctrl *ctrl, int argc,
				   char *argv[])
{
	char cmd[256];
	int res;

	if (argc == 0) {
		wpa_cli_show_network_variables();
		return 0;
	}

	if (argc != 2) {
		printf("Invalid GET_NETWORK command: needs two arguments\n"
		       "(network id and variable name)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "GET_NETWORK %s %s",
			  argv[0], argv[1]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long GET_NETWORK command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_disconnect(struct wpa_ctrl *ctrl, int argc,
				  char *argv[])
{
	return wpa_ctrl_command(ctrl, "DISCONNECT");
}
#endif


#if 0
int wpa_cli_cmd_reconnect(struct wpa_ctrl *ctrl, int argc,
				  char *argv[])
{
	return wpa_ctrl_command(ctrl, "RECONNECT");
}
#endif


#if 0
int wpa_cli_cmd_scan(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	//return wpa_ctrl_command(ctrl, "SCAN");
	return wpa_driver_netlink_scan(NULL, NULL);
}
#endif


#if 0
int wpa_cli_cmd_scan_results(struct wpa_ctrl *ctrl, int argc,
				    char *argv[])
{
	return wpa_ctrl_command(ctrl, "SCAN_RESULTS");
}
#endif


#if 0
int wpa_cli_cmd_bss(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[64];
	int res;

	if (argc != 1) {
		printf("Invalid BSS command: need one argument (index or "
		       "BSSID)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "BSS %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_get_capability(struct wpa_ctrl *ctrl, int argc,
				      char *argv[])
{
	char cmd[64];
	int res;

	if (argc < 1 || argc > 2) {
		printf("Invalid GET_CAPABILITY command: need either one or "
		       "two arguments\n");
		return -1;
	}

	if ((argc == 2) && os_strcmp(argv[1], "strict") != 0) {
		printf("Invalid GET_CAPABILITY command: second argument, "
		       "if any, must be 'strict'\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "GET_CAPABILITY %s%s", argv[0],
			  (argc == 2) ? " strict" : "");
	if (res < 0 || (size_t) res >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_list_interfaces(struct wpa_ctrl *ctrl)
{
	printf("Available interfaces:\n");
	return wpa_ctrl_command(ctrl, "INTERFACES");
}
#endif


#if 0
int wpa_cli_cmd_interface(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	if (argc < 1) {
		wpa_cli_list_interfaces(ctrl);
		return 0;
	}

	wpa_cli_close_connection();
	os_free(ctrl_ifname);
	ctrl_ifname = os_strdup(argv[0]);

	if (wpa_cli_open_connection(ctrl_ifname, 1)) {
		printf("Connected to interface '%s.\n", ctrl_ifname);
	} else {
		printf("Could not connect to interface '%s' - re-trying\n",
		       ctrl_ifname);
	}
	return 0;
}
#endif


#if 0
int wpa_cli_cmd_reconfigure(struct wpa_ctrl *ctrl, int argc,
				   char *argv[])
{
	return wpa_ctrl_command(ctrl, "RECONFIGURE");
}
#endif


#if 0
int wpa_cli_cmd_terminate(struct wpa_ctrl *ctrl, int argc,
				 char *argv[])
{
	return wpa_ctrl_command(ctrl, "TERMINATE");
}
#endif


#if 0
int wpa_cli_cmd_interface_add(struct wpa_ctrl *ctrl, int argc,
				     char *argv[])
{
	char cmd[256];
	int res;

	if (argc < 1) {
		printf("Invalid INTERFACE_ADD command: needs at least one "
		       "argument (interface name)\n"
		       "All arguments: ifname confname driver ctrl_interface "
		       "driver_param bridge_name\n");
		return -1;
	}

	/*
	 * INTERFACE_ADD <ifname>TAB<confname>TAB<driver>TAB<ctrl_interface>TAB
	 * <driver_param>TAB<bridge_name>
	 */
	res = os_snprintf(cmd, sizeof(cmd),
			  "INTERFACE_ADD %s\t%s\t%s\t%s\t%s\t%s",
			  argv[0],
			  argc > 1 ? argv[1] : "", argc > 2 ? argv[2] : "",
			  argc > 3 ? argv[3] : "", argc > 4 ? argv[4] : "",
			  argc > 5 ? argv[5] : "");
	if (res < 0 || (size_t) res >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_interface_remove(struct wpa_ctrl *ctrl, int argc,
					char *argv[])
{	char cmd[128];
	int res;

	if (argc != 1) {
		printf("Invalid INTERFACE_REMOVE command: needs one argument "
		       "(interface name)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "INTERFACE_REMOVE %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
int wpa_cli_cmd_interface_list(struct wpa_ctrl *ctrl, int argc,
				      char *argv[])
{
	return wpa_ctrl_command(ctrl, "INTERFACE_LIST");
}
#endif


#if 0
int wpa_cli_cmd_suspend(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	return wpa_ctrl_command(ctrl, "SUSPEND");
}
#endif


#if 0
int wpa_cli_cmd_resume(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	return wpa_ctrl_command(ctrl, "RESUME");
}
#endif


#if 0
int wpa_cli_cmd_drop_sa(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	return wpa_ctrl_command(ctrl, "DROP_SA");
}
#endif


#if 0
int wpa_cli_cmd_roam(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	char cmd[128];
	int res;

	if (argc != 1) {
		printf("Invalid ROAM command: needs one argument "
		       "(target AP's BSSID)\n");
		return -1;
	}

	res = os_snprintf(cmd, sizeof(cmd), "ROAM %s", argv[0]);
	if (res < 0 || (size_t) res >= sizeof(cmd) - 1) {
		printf("Too long ROAM command.\n");
		return -1;
	}
	return wpa_ctrl_command(ctrl, cmd);
}
#endif


#if 0
enum wpa_cli_cmd_flags {
	cli_cmd_flag_none		= 0x00,
	cli_cmd_flag_sensitive		= 0x01
};
#endif


#if 0
struct wpa_cli_cmd {
	const char *cmd;
	int (*handler)(struct wpa_ctrl *ctrl, int argc, char *argv[]);
	enum wpa_cli_cmd_flags flags;
	//const char *usage;
};
#endif


#if 0
struct wpa_cli_cmd wpa_cli_commands[] = {
	{ "status", wpa_cli_cmd_status,
	  cli_cmd_flag_none,
	  "[verbose] = get current WPA/EAPOL/EAP status" },
	/*
	{ "ping", wpa_cli_cmd_ping,
	  cli_cmd_flag_none,
	  "= pings wpa_supplicant" },
	{ "mib", wpa_cli_cmd_mib,
	  cli_cmd_flag_none,
	  "= get MIB variables (dot1x, dot11)" },
	{ "help", wpa_cli_cmd_help,
	  cli_cmd_flag_none,
	  "= show this usage help" },
	{ "interface", wpa_cli_cmd_interface,
	  cli_cmd_flag_none,
	  "[ifname] = show interfaces/select interface" },
	{ "level", wpa_cli_cmd_level,
	  cli_cmd_flag_none,
	  "<debug level> = change debug level" },
	{ "license", wpa_cli_cmd_license,
	  cli_cmd_flag_none,
	  "= show full wpa_cli license" },
	{ "quit", wpa_cli_cmd_quit,
	  cli_cmd_flag_none,
	  "= exit wpa_cli" },
	{ "set", wpa_cli_cmd_set,
	  cli_cmd_flag_none,
	  "= set variables (shows list of variables when run without "
	  "arguments)" },
	{ "logon", wpa_cli_cmd_logon,
	  cli_cmd_flag_none,
	  "= IEEE 802.1X EAPOL state machine logon" },
	{ "logoff", wpa_cli_cmd_logoff,
	  cli_cmd_flag_none,
	  "= IEEE 802.1X EAPOL state machine logoff" },
	{ "pmksa", wpa_cli_cmd_pmksa,
	  cli_cmd_flag_none,
	  "= show PMKSA cache" },
	*/
	/*
	{ "reassociate", wpa_cli_cmd_reassociate,
	  cli_cmd_flag_none,
	  "= force reassociation" },
	{ "preauthenticate", wpa_cli_cmd_preauthenticate,
	  cli_cmd_flag_none,
	  "<BSSID> = force preauthentication" },
        */
	/*
	{ "identity", wpa_cli_cmd_identity,
	  cli_cmd_flag_none,
	  "<network id> <identity> = configure identity for an SSID" },
	{ "password", wpa_cli_cmd_password,
	  cli_cmd_flag_sensitive,
	  "<network id> <password> = configure password for an SSID" },
	*/
	/*
	{ "new_password", wpa_cli_cmd_new_password,
	  cli_cmd_flag_sensitive,
	  "<network id> <password> = change password for an SSID" },
	{ "pin", wpa_cli_cmd_pin,
	  cli_cmd_flag_sensitive,
	  "<network id> <pin> = configure pin for an SSID" },
	{ "otp", wpa_cli_cmd_otp,
	  cli_cmd_flag_sensitive,
	  "<network id> <password> = configure one-time-password for an SSID"},
	{ "passphrase", wpa_cli_cmd_passphrase,
	  cli_cmd_flag_sensitive,
	  "<network id> <passphrase> = configure private key passphrase\n"
	  "  for an SSID" },
	{ "bssid", wpa_cli_cmd_bssid,
	  cli_cmd_flag_none,
	  "<network id> <BSSID> = set preferred BSSID for an SSID" },
	{ "list_networks", wpa_cli_cmd_list_networks,
	  cli_cmd_flag_none,
	  "= list configured networks" },
	{ "select_network", wpa_cli_cmd_select_network,
	  cli_cmd_flag_none,
	  "<network id> = select a network (disable others)" },
	{ "enable_network", wpa_cli_cmd_enable_network,
	  cli_cmd_flag_none,
	  "<network id> = enable a network" },
	{ "disable_network", wpa_cli_cmd_disable_network,
	  cli_cmd_flag_none,
	  "<network id> = disable a network" },
	{ "add_network", wpa_cli_cmd_add_network,
	  cli_cmd_flag_none,
	  "= add a network" },
	{ "remove_network", wpa_cli_cmd_remove_network,
	  cli_cmd_flag_none,
	  "<network id> = remove a network" },
	{ "set_network", wpa_cli_cmd_set_network,
	  cli_cmd_flag_sensitive,
	  "<network id> <variable> <value> = set network variables (shows\n"
	  "  list of variables when run without arguments)" },
	{ "get_network", wpa_cli_cmd_get_network,
	  cli_cmd_flag_none,
	  "<network id> <variable> = get network variables" },
	*/
	/*
	{ "save_config", wpa_cli_cmd_save_config,
	  cli_cmd_flag_none,
	  "= save the current configuration" },
	*/
	/*
	{ "disconnect", wpa_cli_cmd_disconnect,
	  cli_cmd_flag_none,
	  "= disconnect and wait for reassociate/reconnect command before\n"
	  "  connecting" },
	{ "reconnect", wpa_cli_cmd_reconnect,
	  cli_cmd_flag_none,
	  "= like reassociate, but only takes effect if already disconnected"
	},
	{ "scan", wpa_cli_cmd_scan,
	  cli_cmd_flag_none,
	  "= request new BSS scan" },
	{ "scan_results", wpa_cli_cmd_scan_results,
	  cli_cmd_flag_none,
	  "= get latest scan results" },
	{ "bss", wpa_cli_cmd_bss,
	  cli_cmd_flag_none,
	  "<<idx> | <bssid>> = get detailed scan result info" },
    */
	/*
	{ "get_capability", wpa_cli_cmd_get_capability,
	  cli_cmd_flag_none,
	  "<eap/pairwise/group/key_mgmt/proto/auth_alg> = get capabilies" },
	{ "reconfigure", wpa_cli_cmd_reconfigure,
	  cli_cmd_flag_none,
	  "= force wpa_supplicant to re-read its configuration file" },
	{ "terminate", wpa_cli_cmd_terminate,
	  cli_cmd_flag_none,
	  "= terminate wpa_supplicant" },
	{ "interface_add", wpa_cli_cmd_interface_add,
	  cli_cmd_flag_none,
	  "<ifname> <confname> <driver> <ctrl_interface> <driver_param>\n"
	  "  <bridge_name> = adds new interface, all parameters but <ifname>\n"
	  "  are optional" },
	{ "interface_remove", wpa_cli_cmd_interface_remove,
	  cli_cmd_flag_none,
	  "<ifname> = removes the interface" },
	{ "interface_list", wpa_cli_cmd_interface_list,
	  cli_cmd_flag_none,
	  "= list available interfaces" },
	{ "ap_scan", wpa_cli_cmd_ap_scan,
	  cli_cmd_flag_none,
	  "<value> = set ap_scan parameter" },
	{ "stkstart", wpa_cli_cmd_stkstart,
	  cli_cmd_flag_none,
	  "<addr> = request STK negotiation with <addr>" },
	{ "ft_ds", wpa_cli_cmd_ft_ds,
	  cli_cmd_flag_none,
	  "<addr> = request over-the-DS FT with <addr>" },
	{ "wps_pbc", wpa_cli_cmd_wps_pbc,
	  cli_cmd_flag_none,
	  "[BSSID] = start Wi-Fi Protected Setup: Push Button Configuration" },
	{ "wps_pin", wpa_cli_cmd_wps_pin,
	  cli_cmd_flag_sensitive,
	  "<BSSID> [PIN] = start WPS PIN method (returns PIN, if not "
	  "hardcoded)" },
	 */
#ifdef CONFIG_WPS_OOB
#if 0
	/*
	{ "wps_oob", wpa_cli_cmd_wps_oob,
	  cli_cmd_flag_sensitive,
	  "<DEV_TYPE> <PATH> <METHOD> [DEV_NAME] = start WPS OOB" },
	*/
#endif
#endif /* CONFIG_WPS_OOB */
	/*
	{ "wps_reg", wpa_cli_cmd_wps_reg,
	  cli_cmd_flag_sensitive,
	  "<BSSID> <AP PIN> = start WPS Registrar to configure an AP" },
	{ "wps_er_start", wpa_cli_cmd_wps_er_start,
	  cli_cmd_flag_none,
	  "= start Wi-Fi Protected Setup External Registrar" },
	{ "wps_er_stop", wpa_cli_cmd_wps_er_stop,
	  cli_cmd_flag_none,
	  "= stop Wi-Fi Protected Setup External Registrar" },
	{ "wps_er_pin", wpa_cli_cmd_wps_er_pin,
	  cli_cmd_flag_sensitive,
	  "<UUID> <PIN> = add an Enrollee PIN to External Registrar" },
	{ "wps_er_pbc", wpa_cli_cmd_wps_er_pbc,
	  cli_cmd_flag_none,
	  "<UUID> = accept an Enrollee PBC using External Registrar" },
	{ "wps_er_learn", wpa_cli_cmd_wps_er_learn,
	  cli_cmd_flag_sensitive,
	  "<UUID> <PIN> = learn AP configuration" },
	{ "ibss_rsn", wpa_cli_cmd_ibss_rsn,
	  cli_cmd_flag_none,
	  "<addr> = request RSN authentication with <addr> in IBSS" },
	 */
	/*
	{ "suspend", wpa_cli_cmd_suspend, cli_cmd_flag_none,
	  "= notification of suspend/hibernate" },
	{ "resume", wpa_cli_cmd_resume, cli_cmd_flag_none,
	  "= notification of resume/thaw" },
	{ "drop_sa", wpa_cli_cmd_drop_sa, cli_cmd_flag_none,
	  "= drop SA without deauth/disassoc (test command)" },
	{ "roam", wpa_cli_cmd_roam,
	  cli_cmd_flag_none,
	  "<addr> = roam to the specified BSS" },
	 */
	{ NULL, NULL, cli_cmd_flag_none, NULL }
};
#endif


#if 0
/*
 * Prints command usage, lines are padded with the specified string.
 */
void print_cmd_help(struct wpa_cli_cmd *cmd, const char *pad)
{
	char c;
	size_t n;

	printf("%s%s ", pad, cmd->cmd);
	for (n = 0; (c = cmd->usage[n]); n++) {
		printf("%c", c);
		if (c == '\n')
			printf("%s", pad);
	}
	printf("\n");
}
#endif


#if 0
void print_help(void)
{
	int n;
	printf("commands:\n");
	for (n = 0; wpa_cli_commands[n].cmd; n++)
		print_cmd_help(&wpa_cli_commands[n], "  ");
}
#endif


#if 0
int wpa_request(struct wpa_ctrl *ctrl, int argc, char *argv[])
{
	struct wpa_cli_cmd *cmd, *match = NULL;
	int count;
	int ret = 0;

	count = 0;
	cmd = wpa_cli_commands;
	while (cmd->cmd) {
		if (os_strncasecmp(cmd->cmd, argv[0], os_strlen(argv[0])) == 0)
		{
			match = cmd;
			if (os_strcasecmp(cmd->cmd, argv[0]) == 0) {
				/* we have an exact match */
				count = 1;
				break;
			}
			count++;
		}
		cmd++;
	}

	if (count > 1) {
		printf("Ambiguous command '%s'; possible commands:", argv[0]);
		cmd = wpa_cli_commands;
		while (cmd->cmd) {
			if (os_strncasecmp(cmd->cmd, argv[0],
					   os_strlen(argv[0])) == 0) {
				printf(" %s", cmd->cmd);
			}
			cmd++;
		}
		printf("\n");
		ret = 1;
	} else if (count == 0) {
		printf("Unknown command '%s'\n", argv[0]);
		ret = 1;
	} else {
		ret = match->handler(ctrl, argc - 1, &argv[1]);
	}

	return ret;
}
#endif


#if 0
int str_match(const char *a, const char *b)
{
	return os_strncmp(a, b, os_strlen(b)) == 0;
}
#endif


#if 0
int wpa_cli_exec(const char *program, const char *arg1,
			const char *arg2)
{

	char *cmd;
	size_t len;
	int res;
	int ret = 0;

	len = os_strlen(program) + os_strlen(arg1) + os_strlen(arg2) + 3;
	cmd = os_malloc(len);
	if (cmd == NULL)
		return -1;
	res = os_snprintf(cmd, len, "%s %s %s", program, arg1, arg2);
	if (res < 0 || (size_t) res >= len) {
		os_free(cmd);
		return -1;
	}
	cmd[len - 1] = '\0';

	os_free(cmd);

	return ret;
}
#endif


#if 0
void wpa_cli_action_process(const char *msg)
{
	const char *pos;
	char *copy = NULL, *id, *pos2;

	pos = msg;
	if (*pos == '<') {
		/* skip priority */
		pos = os_strchr(pos, '>');
		if (pos)
			pos++;
		else
			pos = msg;
	}

	if (str_match(pos, WPA_EVENT_CONNECTED)) {
		int new_id = -1;
		/*
		os_unsetenv("WPA_ID");
		os_unsetenv("WPA_ID_STR");
		os_unsetenv("WPA_CTRL_DIR");
		*/

		pos = os_strstr(pos, "[id=");
		if (pos)
			copy = os_strdup(pos + 4);

		if (copy) {
			pos2 = id = copy;
			while (*pos2 && *pos2 != ' ')
				pos2++;
			*pos2++ = '\0';
			new_id = atoi(id);
			//os_setenv("WPA_ID", id, 1);
			while (*pos2 && *pos2 != '=')
				pos2++;
			if (*pos2 == '=')
				pos2++;
			id = pos2;
			while (*pos2 && *pos2 != ']')
				pos2++;
			*pos2 = '\0';
			//os_setenv("WPA_ID_STR", id, 1);
			os_free(copy);
		}

		//os_setenv("WPA_CTRL_DIR", ctrl_iface_dir, 1);

		if (!wpa_cli_connected || new_id != wpa_cli_last_id) {
			wpa_cli_connected = 1;
			wpa_cli_last_id = new_id;
			wpa_cli_exec(action_file, ctrl_ifname, "CONNECTED");
		}
	} else if (str_match(pos, WPA_EVENT_DISCONNECTED)) {
		if (wpa_cli_connected) {
			wpa_cli_connected = 0;
			wpa_cli_exec(action_file, ctrl_ifname, "DISCONNECTED");
		}
	} else if (str_match(pos, WPA_EVENT_TERMINATING)) {
		printf("wpa_supplicant is terminating - stop monitoring\n");
		wpa_cli_quit = 1;
	}
}
#endif


#if 0
#ifndef CONFIG_ANSI_C_EXTRA
void wpa_cli_action_cb(char *msg, size_t len)
{
	wpa_cli_action_process(msg);
}
#endif /* CONFIG_ANSI_C_EXTRA */
#endif


#if 0
void wpa_cli_reconnect(void)
{
	wpa_cli_close_connection();
	wpa_cli_open_connection(ctrl_ifname, 1);
}
#endif


#if 0
void wpa_cli_recv_pending(struct wpa_ctrl *ctrl, int in_read,
				 int action_monitor)
{
	int first = 1;
	if (ctrl_conn == NULL) {
		wpa_cli_reconnect();
		return;
	}
	while (wpa_ctrl_pending(ctrl) > 0) {
		char buf[256];
		size_t len = sizeof(buf) - 1;
		if (wpa_ctrl_recv(ctrl, buf, &len) == 0) {
			buf[len] = '\0';
			if (action_monitor)
				wpa_cli_action_process(buf);
			else {
				if (in_read && first)
					printf("\r");
				first = 0;
				printf("%s\n", buf);
			}
		} else {
			printf("Could not read pending message.\n");
			break;
		}
	}

	if (wpa_ctrl_pending(ctrl) < 0) {
		printf("Connection to wpa_supplicant lost - trying to "
		       "reconnect\n");
		wpa_cli_reconnect();
	}
}
#endif


#if 0
void wpa_cli_interactive(void)
{
#define max_args 10
	char cmdbuf[256], *cmd, *argv[max_args], *pos;
	int argc;

	printf("\nInteractive mode\n\n");

	do {
		wpa_cli_recv_pending(mon_conn, 0, 0);

		printf("> ");
		cmd = fgets(cmdbuf, sizeof(cmdbuf), stdin);

		if (cmd == NULL)
			break;
		wpa_cli_recv_pending(mon_conn, 0, 0);
		pos = cmd;
		while (*pos != '\0') {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		argc = 0;
		pos = cmd;
		for (;;) {
			while (*pos == ' ')
				pos++;
			if (*pos == '\0')
				break;
			argv[argc] = pos;
			argc++;
			if (argc == max_args)
				break;
			if (*pos == '"') {
				char *pos2 = os_strrchr(pos, '"');
				if (pos2)
					pos = pos2 + 1;
			}
			while (*pos != '\0' && *pos != ' ')
				pos++;
			if (*pos == ' ')
				*pos++ = '\0';
		}
		if (argc)
			wpa_request(ctrl_conn, argc, argv);

		if (cmd != cmdbuf)
			free(cmd);
	} while (!wpa_cli_quit);
}
#endif


#if 0
void wpa_cli_action(struct wpa_ctrl *ctrl)
{
#ifdef CONFIG_ANSI_C_EXTRA
	/* TODO: ANSI C version(?) */
	printf("Action processing not supported in ANSI C build.\n");
#else /* CONFIG_ANSI_C_EXTRA */
	fd_set rfds;
	int fd, res;
	struct timeval tv;
	char buf[256]; /* note: large enough to fit in unsolicited messages */
	size_t len;

	fd = wpa_ctrl_get_fd(ctrl);

	while (!wpa_cli_quit) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		tv.tv_sec = ping_interval;
		tv.tv_usec = 0;
		res = select(fd + 1, &rfds, NULL, NULL, &tv);
		if (res < 0 && errno != EINTR) {
			perror("select");
			break;
		}

		if (FD_ISSET(fd, &rfds))
			wpa_cli_recv_pending(ctrl, 0, 1);
		else {
			/* verify that connection is still working */
			len = sizeof(buf) - 1;
			if (wpa_ctrl_request(ctrl, "PING", 4, buf, &len,
					     wpa_cli_action_cb) < 0 ||
			    len < 4 || os_memcmp(buf, "PONG", 4) != 0) {
				printf("wpa_supplicant did not reply to PING "
				       "command - exiting\n");
				break;
			}
		}
	}
#endif /* CONFIG_ANSI_C_EXTRA */
}
#endif


#if 0
void wpa_cli_cleanup(void)
{
	wpa_cli_close_connection();
	if (pid_file)
		os_daemonize_terminate(pid_file);

	os_program_deinit();
}
#endif


#if 0
void wpa_cli_terminate(int sig)
{
	wpa_cli_cleanup();
	//exit(0);
}
#endif


#if 0
#ifndef CONFIG_NATIVE_WINDOWS
void wpa_cli_alarm(int sig)
{
#if 0
	if (ctrl_conn && _wpa_ctrl_command(ctrl_conn, "PING", 0)) {
		printf("Connection to wpa_supplicant lost - trying to "
		       "reconnect\n");
		wpa_cli_close_connection();
	}
	if (!ctrl_conn)
		wpa_cli_reconnect();
	if (mon_conn)
		wpa_cli_recv_pending(mon_conn, 1, 0);
	alarm(ping_interval);
#endif //#if 0

}
#endif /* CONFIG_NATIVE_WINDOWS */
#endif


#if 0
char * wpa_cli_get_default_ifname(void)
{

	char *ifname = NULL;

	return ifname;
}
#endif

#if 0
static const char *wpa_cli_version =
"wpa_cli v" VERSION_STR "\n"
"Copyright (c) 2004-2010, Jouni Malinen <j@w1.fi> and contributors";


static const char *wpa_cli_license =
"This program is free software. You can distribute it and/or modify it\n"
"under the terms of the GNU General Public License version 2.\n"
"\n"
"Alternatively, this software may be distributed under the terms of the\n"
"BSD license. See README and COPYING for more details.\n";

static const char *wpa_cli_full_license =
"This program is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License version 2 as\n"
"published by the Free Software Foundation.\n"
"\n"
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n"
"\n"
"You should have received a copy of the GNU General Public License\n"
"along with this program; if not, write to the Free Software\n"
"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA\n"
"\n"
"Alternatively, this software may be distributed under the terms of the\n"
"BSD license.\n"
"\n"
"Redistribution and use in source and binary forms, with or without\n"
"modification, are permitted provided that the following conditions are\n"
"met:\n"
"\n"
"1. Redistributions of source code must retain the above copyright\n"
"   notice, this list of conditions and the following disclaimer.\n"
"\n"
"2. Redistributions in binary form must reproduce the above copyright\n"
"   notice, this list of conditions and the following disclaimer in the\n"
"   documentation and/or other materials provided with the distribution.\n"
"\n"
"3. Neither the name(s) of the above-listed copyright holder(s) nor the\n"
"   names of its contributors may be used to endorse or promote products\n"
"   derived from this software without specific prior written permission.\n"
"\n"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
"\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n"
"A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT\n"
"OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n"
"SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT\n"
"LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n"
"DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n"
"THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
"(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
"\n";
#endif


