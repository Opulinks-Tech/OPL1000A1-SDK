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
#include <string.h>
#include "common.h"
#include "defs.h"
#include "controller_wifi_com.h"
#include "wpa_supplicant_i.h"
#include "wifi_nvm.h"
#include "wpa_cli.h"
#include "driver_netlink.h"
#include "sys_common_ctrl.h"

#include "controller_wifi_com_patch.h"
#include "driver_netlink_patch.h"
#include "wpa_cli_patch.h"
#include "wifi_nvm_patch.h"
#include "mw_fim_default_group02_patch.h"
#include "mw_fim_default_group03_patch.h"

extern struct wpa_supplicant *wpa_s;
extern u8 gsta_cfg_mac[MAC_ADDR_LEN];
extern struct wpa_config conf;
extern struct wpa_ssid ssid_data;
extern char g_passphrase[MAX_LEN_OF_PASSWD];
extern u8 g_bssid[6];

extern Boolean isMAC(char *s);

int check_mac_addr_len(const char *txt)
{
    u8 count = 0;
    const char *pos = txt;

    while (*pos != NULL) {
        while (*pos == ':' || *pos == '.' || *pos == '-')
            pos++;

        *pos++;
        *pos++;
        count++;
    }

    if (count != MAC_ADDR_LEN)
        return -1;

    return 0;
}

void wpa_cli_setmac_patch(u8 *mac)
{
    if(mac == NULL) return;

    if (is_broadcast_ether_addr(mac)) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: Invalid mac address, all of mac if 0xFF \r\n");
        return;
    }

    if (is_multicast_ether_addr(mac)) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: Invalid mac address, not allow multicast mac address \r\n");
        return;
    }
    
    if (is_zero_ether_addr(mac)) {
        msg_print(LOG_HIGH_LEVEL, "[CLI]WPA: Invalid mac address, all of mac is zero. \r\n");
        return;
    }
    
    memset(&gsta_cfg_mac[0], 0, MAC_ADDR_LEN);
    memcpy(&gsta_cfg_mac[0], &mac[0], MAC_ADDR_LEN);

    wifi_nvm_sta_info_write(WIFI_NVM_STA_INFO_ID_MAC_ADDR, MAC_ADDR_LEN, &gsta_cfg_mac[0]);
}

void wpa_cli_mac_by_param_patch(int argc, char *argv[])
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
        
        if (check_mac_addr_len(argv[1]) == -1) {
            msg_print(LOG_HIGH_LEVEL, "Invalid mac address, wrong length of mac address \r\n");
            return;
        }
        
        if (hwaddr_aton2(argv[1], mac) == -1) {
            msg_print(LOG_HIGH_LEVEL, "Invalid mac address \r\n");
            return;
        }
        
        wpa_cli_setmac(mac);
    }
}

void debug_auto_connect_patch(void)
{
    int i;
    MwFimAutoConnectCFG_t cfg;
    auto_conn_info_t info;
    u8 name[STA_INFO_MAX_MANUF_NAME_SIZE] = {0};
    u8 mac[MAC_ADDR_LEN] = {0};
    
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
        msg_print(LOG_HIGH_LEVEL, "AP[%d] dtim period = %d\r\n", i, info.dtim_prod);
    }
    
    wpa_cli_getmac(&mac[0]);
    wifi_nvm_sta_info_read(WIFI_NVM_STA_INFO_MANUFACTURE_NAME, STA_INFO_MAX_MANUF_NAME_SIZE, &name[0]);
    
    msg_print(LOG_HIGH_LEVEL, "STA info mac = %02x %02x %02x %02x %02x %02x\r\n", 
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    msg_print(LOG_HIGH_LEVEL, "STA info manufacture = %s\r\n", name);
}

int wpa_cli_connect_handler_patch(int argc, char *argv[])
{
    u8 bssid[6] = {0};
    int i;
    char passwd[MAX_LEN_OF_PASSWD] = {0};
    int len_passwd = 0;
    int len_ssid = 0;
    u8 ret = FALSE;
    
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
        ret=FALSE;
        goto DONE;
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

        //wpa_printf(MSG_DEBUG, "[CLI]WPA: bssid buffer is ready \r\n");
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
                ret=FALSE;
                goto DONE;
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
                ret=FALSE;
                goto DONE;
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

    ret = wpa_cli_connect(&conf);

DONE:    
    if (conf.ssid->ssid) {
        os_free(conf.ssid->ssid);
        conf.ssid->ssid = NULL;
    }
    
    return (ret)?TRUE:FALSE;
}

int wpa_cli_connect_patch(struct wpa_config * conf)
{
    u8 ret = FALSE;
    if (conf == NULL) return FALSE;
    if (conf->ssid == NULL) return FALSE;
    ret = wpa_driver_netlink_connect(conf);
    if (ret == FALSE) return FALSE;
    return TRUE;
}

int wpa_cli_clear_ac_list_patch(int argc, char *argv[])
{
    // Reset Auto/Fast connect configuration in the FIM
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_MODE, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_NUM, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_CFG, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 1);
    MwFim_FileWriteDefault(MW_FIM_IDX_WIFI_AUTO_CONN_AP_INFO, 2);

    // Reset Auto/Fast connect configuration of global variables
    reset_auto_connect_list();

    //Reset Sta information
    MwFim_FileWriteDefault(MW_FIM_IDX_GP03_PATCH_STA_MAC_ADDR, 0);
    MwFim_FileWriteDefault(MW_FIM_IDX_GP02_PATCH_STA_SKIP_DTIM, 0);
    
    return TRUE;
}

void debug_cli_mac_addr_src(void)
{
    u8 type;

    msg_print(LOG_HIGH_LEVEL, "Mac Address Source:\r\n");
    msg_print(LOG_HIGH_LEVEL, "   0:OTP, 1:Flash\r\n");
    base_mac_addr_src_get_cfg(BASE_NVM_MAC_SRC_IFACE_ID_STA, &type);
    msg_print(LOG_HIGH_LEVEL, "   WIFI STA : %d", type);
    base_mac_addr_src_get_cfg(BASE_NVM_MAC_SRC_IFACE_ID_SOFTAP, &type);
    msg_print(LOG_HIGH_LEVEL, "   SoftAP: %d", type);
    base_mac_addr_src_get_cfg(BASE_NVM_MAC_SRC_IFACE_ID_BLE, &type);
    msg_print(LOG_HIGH_LEVEL, "   BLE : %d\r\n", type);
}

/* debug use */
int wpa_cli_dbg_patch(int argc, char *argv[])
{
    u8 mac[MAC_ADDR_LEN] = {0};
    
    if (!strcmp(argv[1], "h")) {
        msg_print(LOG_HIGH_LEVEL, "wpa debug :\r\n");
        msg_print(LOG_HIGH_LEVEL, "   h : help\r\n");
        msg_print(LOG_HIGH_LEVEL, "   p : print memory variable of auto connect/CBS ...\r\n");
        msg_print(LOG_HIGH_LEVEL, "   ia : Test input mac addr/manufacture name for CBS\r\n");
        msg_print(LOG_HIGH_LEVEL, "   ib : Test input mac address source config\r\n");
        msg_print(LOG_HIGH_LEVEL, "        iface, type\r\n");
        return TRUE;
    }
    
    if (!strcmp(argv[1], "p") || argc == 1) {
        debug_auto_connect();
        debug_cli_mac_addr_src();
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

    if (!strcmp(argv[1], "ib")) {
        u8 iface, type;
        iface = atoi(argv[2]);
        type = atoi(argv[3]);
        base_mac_addr_src_set_cfg(iface, type);
    }

    return TRUE;
}

int wpa_cli_scan_by_cfg(void *cfg)
{
    if (cfg == NULL) return FALSE;

    S_WIFI_MLME_SCAN_CFG *scan_cfg = (S_WIFI_MLME_SCAN_CFG *)cfg;

    if (scan_cfg->u32ActiveScanDur < SCAN_ACTIVE_MIN_DUR_TIME_DEF || 
        scan_cfg->u32ActiveScanDur > SCAN_MAX_NUM_OF_DUR_TIME) {
        scan_cfg->u32ActiveScanDur = SCAN_ACTIVE_MIN_DUR_TIME_DEF;
    }

    if (scan_cfg->u32PassiveScanDur < SCAN_PASSIVE_MIN_DUR_TIME_DEF ||
        scan_cfg->u32PassiveScanDur > SCAN_MAX_NUM_OF_DUR_TIME) {
        scan_cfg->u32PassiveScanDur = SCAN_PASSIVE_MIN_DUR_TIME_DEF;
    }
    
    if (scan_cfg->u8Channel > WIFI_MLME_SCAN_MAX_NUM_CHANNELS) {
        scan_cfg->u8Channel = WIFI_MLME_SCAN_ALL_CHANNELS;
    }
    
    wpa_driver_netlink_scan_by_cfg(scan_cfg);

    return TRUE;
}

/*
   Interface Initialization: WPA CLI
 */
void wpa_cli_func_init_patch(void)
{
    wpa_cli_setmac          = wpa_cli_setmac_patch;
    debug_auto_connect      = debug_auto_connect_patch;
    wpa_cli_connect_handler = wpa_cli_connect_handler_patch;
    wpa_cli_connect         = wpa_cli_connect_patch;
    wpa_cli_clear_ac_list   = wpa_cli_clear_ac_list_patch;
    wpa_cli_mac_by_param    = wpa_cli_mac_by_param_patch;
    wpa_cli_dbg             = wpa_cli_dbg_patch;
    return;
}
