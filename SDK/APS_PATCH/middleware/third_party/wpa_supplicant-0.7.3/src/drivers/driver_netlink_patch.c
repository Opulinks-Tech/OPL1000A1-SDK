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
#include "defs.h"
#include "controller_wifi_com.h"
#include "driver_netlink.h"
#include "wpa_supplicant_i.h"
#include "driver_netlink_patch.h"
#include "controller_wifi_com_patch.h"
#include "driver_netlink_patch.h"

extern u8 g_bssid[6];
extern u8 g_fastconn;
extern struct wpa_supplicant *wpa_s;
extern u8 g_wifi_reconnection_counter;
extern char g_passphrase[MAX_LEN_OF_PASSWD];

Boolean wpa_driver_netlink_sta_cfg_patch(u8 mode, u8 cmd_idx, u8 *value)
{
    if (mode != MLME_CMD_SET_PARAM && mode != MLME_CMD_GET_PARAM) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: Invalid Parameter \r\n");
        return FALSE;
    }

    if (value == NULL) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: Invalid Parameter \r\n");
        return FALSE;
    }

    //if (wpa_s->wpa_state == WPA_COMPLETED || wpa_s->wpa_state == WPA_ASSOCIATED) {
    //    msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: Invalid wpa state \r\n");
    //    return FALSE;
    //}

    switch (mode) {
        case MLME_CMD_GET_PARAM:
            _wifi_get_sta_cfg_from_share_memory_patch(cmd_idx, value);
            break;
        case MLME_CMD_SET_PARAM:
            wifi_set_sta_cfg_req(cmd_idx, value);
            break;
        default:
            break;
    }
    
    return TRUE;
}

int target_ap_security_mode_chk(u8 *mac, char *ssid)
{
    scan_info_t *pInfo = NULL;
    
    // Get AP index in scan list
    if (!is_zero_ether_addr(mac)) {
        pInfo = wifi_get_scan_record(mac);
    }
    else {
        pInfo = wifi_get_scan_record_by_ssid(ssid);
    }

    if (pInfo == NULL) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: AP index is null \r\n");
        return false;
    }
        
    // AP security mode is off
    if (pInfo->rsn_ie[0] == 0 && pInfo->wpa_ie[0] == 0) {
        if (strlen(g_passphrase) != 0) {
            msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: AP mode is none security, should not has passphrase \r\n");
            return false;
        }
    }
    else {
        if (strlen(g_passphrase) == 0) {
            msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: AP mode is security, should has passphrase \r\n");
            return false;
        }
    }
    
    return TRUE;
}

Boolean wpa_driver_netlink_connect_patch(struct wpa_config * conf)
{
    int ret = 0;
    scan_info_t *pInfo = NULL;
    int i;
    hap_control_t *hap_temp;
    hap_temp=get_hap_control_struct();

    if (conf == NULL) return FALSE;
    if (conf->ssid == NULL) return FALSE;

    if (!target_ap_security_mode_chk(conf->ssid->bssid, (char*)conf->ssid->ssid)) {
        ret = -1;
        goto done;
    }
    
    if (wpa_driver_netlink_get_state() == WPA_ASSOCIATED ||
        wpa_driver_netlink_get_state() == WPA_COMPLETED) {
        set_repeat_conn(true);
    }

#ifdef __WIFI_AUTO_CONNECT__
    //For compatible auto/manual connect
    if (get_auto_connect_mode() == AUTO_CONNECT_ENABLE) {
        set_auto_connect_mode(AUTO_CONNECT_MANUAL);
    }
#endif
    
    if (conf->ssid->ssid == NULL) {
        wpa_printf(MSG_DEBUG, "[DRV]WPA: wpa_driver_netlink_connect, bssid:%x %x %x %x %x %x \r\n",
            conf->ssid->bssid[0], conf->ssid->bssid[1], conf->ssid->bssid[2],
            conf->ssid->bssid[3], conf->ssid->bssid[4], conf->ssid->bssid[5]);
            
        if (get_repeat_conn()) {
            msg_print(LOG_HIGH_LEVEL, "Disconnect since already connected before\r\n");
            wpa_driver_netlink_disconnect(NULL, 0);
            goto done;
        }
        
        wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATING);

        ret = wifi_sta_join(conf->ssid->bssid);
    } 
    else {
        pInfo = wifi_get_scan_record_by_ssid((char*)(conf->ssid->ssid));
        if (pInfo == NULL) {
            if (hap_temp->hap_final_index!=0){
                hap_temp->hap_en=1;
            }
            else{
                ret = -1;
                goto done; // not found target AP in the list
            }
        }

        for (i=0; i<ETH_ALEN; i++) {
            g_bssid[i] = pInfo->bssid[i];
        }
        
        if (get_repeat_conn()) {
            msg_print(LOG_HIGH_LEVEL, "Disconnect since already connected before\r\n");
            wpa_driver_netlink_disconnect(NULL, 0);
            goto done;
        }
        else if (hap_temp->hap_en) {
            hap_temp->hap_ap_info = malloc(sizeof(auto_conn_info_t));
            wifi_sta_join_for_hiddenap();
            goto done;
        }
        
        wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATING);
        g_fastconn = 0;
        g_wifi_reconnection_counter = 0;
        ret = wifi_sta_join(pInfo->bssid);
    }

done:
    if(ret == 0) return TRUE;
    return FALSE;
}

Boolean wpa_driver_netlink_scan_patch(int mode)
{
    //msg_print(LOG_HIGH_LEVEL, "wpa_driver_netlink_scan, mode:%d \r\n", mode);
    if((mode < SCAN_MODE_ACTIVE)||(mode >= SCAN_MODE_NUM)) return FALSE;
    if (!(wpa_s->wpa_state == WPA_COMPLETED || wpa_s->wpa_state == WPA_ASSOCIATED)) {
        wpa_supplicant_set_state(wpa_s, WPA_SCANNING);
    }
    wifi_scan_req(mode);
    return TRUE;
}

Boolean wpa_driver_netlink_scan_by_cfg(void *cfg)
{
    if (cfg == NULL) return FALSE;
    if (!(wpa_s->wpa_state == WPA_COMPLETED || wpa_s->wpa_state == WPA_ASSOCIATED)) {
        wpa_supplicant_set_state(wpa_s, WPA_SCANNING);
    }
    wifi_scan_req_by_cfg(cfg);
    return TRUE;
}

int wpa_driver_netlink_get_state(void)
{
    if (wpa_s == NULL) return 0;
	return ((int)wpa_s->wpa_state);
}

int wpa_driver_netlink_is_connected(void)
{
    if (wpa_driver_netlink_get_state() == WPA_ASSOCIATED ||
        wpa_driver_netlink_get_state() == WPA_COMPLETED) {
        return TRUE;
    }
    return FALSE;
}

/*
   Interface Initialization: WPA Driver
 */
void wpa_driver_func_init_patch(void)
{
    wpa_driver_netlink_sta_cfg = wpa_driver_netlink_sta_cfg_patch;
    wpa_driver_netlink_connect = wpa_driver_netlink_connect_patch;
    wpa_driver_netlink_scan    = wpa_driver_netlink_scan_patch;
    return;
}
