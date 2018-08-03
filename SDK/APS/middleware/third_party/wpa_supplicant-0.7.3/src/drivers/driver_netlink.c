/*
 * Driver interface for RADIUS server or WPS ER only (no driver)
 * Copyright (c) 2008, Atheros Communications
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
#include "includes.h"
#include "common.h"
#include "driver.h"
#include "driver_netlink.h"
#include "wpa_cli.h"
#include "controller_wifi_com.h"
#include "msg.h"
#include "wpa_supplicant_i.h"
#include "at_cmd_common.h"
#include "at_cmd_msg_ext.h"
#include "../../wpa_supplicant/config.h"
#include "wifi_nvm.h"

typedef int (*wifi_mac_xmit_eapol_frame_fn_t)(u8 *buf, size_t len, const u8 *eth_dest, u16 proto);

extern u8 g_bssid[6];
extern struct wpa_supplicant *wpa_s;
extern wifi_mac_xmit_eapol_frame_fn_t wifi_mac_xmit_eapol_frame;

//extern void wpa_supplicant_set_state(struct wpa_supplicant *wpa_s, enum wpa_states state);
//extern int wifi_mac_xmit_eapol_frame(u8 *buf, size_t len, const u8 *eth_dest, u16 proto);

struct wpa_driver_netlink_data {
	void *ctx;
	int ioctl_sock;
	struct netlink_data *netlink;
	char ifname[IFNAMSIZ + 1];
	u8 *assoc_req_ies;
	size_t assoc_req_ies_len;
	u8 *assoc_resp_ies;
	size_t assoc_resp_ies_len;
	int no_of_pmkid;
	struct ndis_pmkid_entry *pmkid;
	int we_version_compiled;
	int ap_scan;
	int scanning_done;
	u8 g_driver_down;
	BOOLEAN	bAddWepKey;
};


void * wpa_driver_netlink_init_impl(void *ctx, const char *ifname)
{
	//Reserved
	return NULL;
}


void wpa_driver_netlink_deinit_impl(void *priv)
{
	//Reserved
}


Boolean wpa_driver_netlink_scan_impl(int mode)
{
    //msg_print(LOG_HIGH_LEVEL, "wpa_driver_netlink_scan, mode:%d \r\n", mode);
    if((mode < SCAN_MODE_ACTIVE)||(mode >= SCAN_MODE_NUM)) return FALSE;
    if (wpa_s->wpa_state != WPA_COMPLETED) {
        wpa_supplicant_set_state(wpa_s, WPA_SCANNING);
    }
    wifi_scan_req(mode);
    return TRUE;
}


Boolean wpa_driver_netlink_get_scan_results_impl(struct wpa_scan_results * scan_res)
{
    scan_report_t *result = NULL;
    unsigned int apNum = 0;
    int i = 0;

    if(scan_res == NULL) return FALSE;

    wpa_driver_netlink_scan_results_clear(scan_res);

    result = wifi_get_scan_result();
    if (result == NULL) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: result == NULL \r\n");
        return FALSE;
    }

    apNum = (unsigned int) result->uScanApNum;
    if (apNum == 0) {
        msg_print(LOG_HIGH_LEVEL, "ap num=0 \r\n");
        return TRUE;
    }

    scan_res->res = os_malloc(sizeof(struct wpa_scan_res) * apNum);

    if(scan_res->res == NULL)
    {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: scan_res->res == NULL \r\n");
        return FALSE;
    }

    for (i = 0; i < apNum; i++) {
        memcpy(scan_res->res[i]->bssid, result->pScanInfo[i].bssid, ETH_ALEN);
        scan_res->res[i]->freq = (int) result->pScanInfo[i].ap_channel;
        scan_res->res[i]->beacon_int = (unsigned short) result->pScanInfo[i].beacon_interval;
        scan_res->res[i]->caps = (unsigned short) result->pScanInfo[i].capabilities;
        scan_res->res[i]->level = (int) result->pScanInfo[i].rssi;
    }

    scan_res->num = apNum;

    /** MSG Extend for AT */
    at_msg_ext_wifi_scan(AT_MSG_EXT_ESPRESSIF, result);

    return TRUE;
}

void wpa_driver_netlink_show_scan_results_impl(void)
{
    scan_report_t *result = NULL;
    unsigned int apNum = 0;
    int i = 0;

    result = wifi_get_scan_result();

    if (result == NULL) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: result == NULL\r\n");
        goto done;
    }

    apNum = (unsigned int) result->uScanApNum;

    msg_print(LOG_HIGH_LEVEL, "ap num=%d\r\n", apNum);

    if (apNum == 0) {
        goto done;
    }

    for (i = 0; i < apNum; i++) {
        msg_print(LOG_HIGH_LEVEL, "bssid=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                  result->pScanInfo[i].bssid[0], result->pScanInfo[i].bssid[1], result->pScanInfo[i].bssid[2],
                  result->pScanInfo[i].bssid[3], result->pScanInfo[i].bssid[4], result->pScanInfo[i].bssid[5]);
        msg_print(LOG_HIGH_LEVEL, "ssid=%s\r\n", result->pScanInfo[i].ssid);
        msg_print(LOG_HIGH_LEVEL, "freq=%d\r\n", result->pScanInfo[i].ap_channel);
        msg_print(LOG_HIGH_LEVEL, "caps=%d\r\n", result->pScanInfo[i].capabilities);
        msg_print(LOG_HIGH_LEVEL, "beacon_int=%d\r\n", result->pScanInfo[i].beacon_interval);
        msg_print(LOG_HIGH_LEVEL, "level=%d\r\n\r\n", result->pScanInfo[i].rssi);
    }

    /** MSG Extend for AT */
    //at_msg_ext_wifi_scan(AT_MSG_EXT_ESPRESSIF, result);

done:
    return;
}


Boolean wpa_driver_netlink_connect_impl(struct wpa_config * conf)
{
    int ret = 0;
    scan_info_t *pInfo = NULL;
    int i;

	if (conf == NULL) return FALSE;
    if (conf->ssid == NULL) return FALSE;

    if (conf->ssid->ssid == NULL) {
        wpa_printf(MSG_DEBUG, "[DRV]WPA: wpa_driver_netlink_connect, bssid:%x %x %x %x %x %x \r\n",
            conf->ssid->bssid[0], conf->ssid->bssid[1], conf->ssid->bssid[2],
            conf->ssid->bssid[3], conf->ssid->bssid[4], conf->ssid->bssid[5]);

        wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATING);

        ret = wifi_sta_join(conf->ssid->bssid);
    } else {
        pInfo = wifi_get_scan_record_by_ssid((char*)(conf->ssid->ssid));
        if(pInfo == NULL) {
            msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: wpa_driver_netlink_connect, pInfo == NULL, cannot find record \r\n");
            return FALSE;
        }

        for (i=0; i<ETH_ALEN; i++) {
            g_bssid[i] = pInfo->bssid[i];
        }

        wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATING);

        ret = wifi_sta_join(pInfo->bssid);
    }

    if(ret == 0) return TRUE;
    return FALSE;
}


Boolean wpa_driver_netlink_connect_by_bssid_impl(u8 *bssid)
{
    int ret = 0;
	if (bssid == NULL) return FALSE;
    ret = wifi_sta_join(bssid);
    wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATING);
    if(ret == 0) return TRUE;
    return FALSE;
}


Boolean wpa_driver_netlink_reconnect_impl(struct wpa_config * conf)
{
    int ret = 0;
	if (conf == NULL) return FALSE;
    if (conf->ssid == NULL) return FALSE;
    wpa_printf(MSG_DEBUG, "[DRV]WPA: wpa_driver_netlink_reconnect, bssid:%x %x %x %x %x %x \r\n",
        conf->ssid->bssid[0], conf->ssid->bssid[1], conf->ssid->bssid[2],
        conf->ssid->bssid[3], conf->ssid->bssid[4], conf->ssid->bssid[5]);
    ret = wifi_sta_join(conf->ssid->bssid);
    if(ret == 0) return TRUE;
    return FALSE;
}


Boolean wpa_driver_netlink_associate_impl(void *priv, struct wpa_driver_associate_params *params)
{
	return TRUE;
}


void wpa_driver_netlink_disconnect_impl(const u8 *bssid, u16 reason_code)
{
    wifi_sta_leave();
}


Boolean wpa_driver_netlink_deauthenticate_impl(void *priv, const u8 *addr, int reason_code)
{
    wifi_sta_leave();
	return TRUE;
}


Boolean wpa_driver_netlink_get_bssid_impl(u8 *bssid)
{
    u8 buf[6];

    if (bssid == NULL) return FALSE;

    os_memset(buf, 0, sizeof(buf));
	if (bssid != NULL) {
        wifi_get_bssid(buf);
        os_memcpy(bssid, buf, sizeof(buf));
        return TRUE;
    }

	return FALSE;
}


Boolean wpa_driver_netlink_get_ssid_impl(u8 *ssid)
{
    if (ssid == NULL) return FALSE;
    
    os_memcpy(ssid, wifi_get_ssid(), MAX_LEN_OF_SSID + 1);
    
    return TRUE;
}


Boolean wpa_driver_netlink_get_mac_impl(u8 *mac)
{
	if (mac == NULL) return FALSE;
    wifi_get_mac_addr(mac);
	return TRUE;
}

void wpa_driver_netlink_set_mac_impl(u8 *mac)
{
	if (mac == NULL) return;
    wifi_set_mac_addr(mac);
}

int wpa_driver_netlink_get_rssi_impl(void)
{
    int rssi = wifi_get_rssi();
    return rssi;
}

int wpa_driver_netlink_set_key_impl(const char *ifname, void *priv,
				     enum wpa_alg alg, const u8 *addr,
				     int key_idx, int set_tx,
				     const u8 *seq, size_t seq_len,
				     const u8 *key, size_t key_len)
{
    //Not Support Yet in link layer
	return 0;
}


void wpa_driver_netlink_scan_results_free_impl(struct wpa_scan_results *res)
{
    //trigger clean

}


void wpa_driver_netlink_scan_results_clear_impl(struct wpa_scan_results *res)
{
    if((res == NULL) || (res->res == NULL))
    {
        goto done;
    }

    os_free(res->res);
    memset(res, 0, sizeof(struct wpa_scan_results));

done:
    return;
}

int wpa_drv_send_eapol_impl(const u8 *dst, u16 proto, u8 *data, size_t data_len)
{
    msg_print(LOG_HIGH_LEVEL, "[KEY][DRV]WPA: ready to send eapol key frame, proto:%d data_len:%d \r\n", proto, data_len);
    //call data path api to send it out
    wifi_mac_xmit_eapol_frame(data, data_len, dst, proto);

    return 1;
}

Boolean wpa_driver_netlink_fast_connect_impl(u8 mode, u8 index)
{
    mw_wifi_auto_connect_ap_info_t info;

    if (mode > AUTO_CONNECT_ENABLE || index >= MAX_NUM_OF_AUTO_CONNECT) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: invalid parameter \r\n");
        return FALSE;
    }
    
    wifi_nvm_auto_connect_read(WIFI_NVM_ID_AUTO_CONNECT_AP_INFO, sizeof(mw_wifi_auto_connect_ap_info_t), index, &info);

    if(info.bssid[0] == 0 && info.bssid[1] == 0) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: the index of AP is empty \r\n");
        return FALSE;
    }

    set_fast_connect_mode(index, mode);
    info.fast_connect = mode;
    wifi_nvm_auto_connect_write(WIFI_NVM_ID_AUTO_CONNECT_AP_INFO, sizeof(mw_wifi_auto_connect_ap_info_t), index, &info);

    return TRUE;
}

Boolean wpa_driver_netlink_sta_cfg_impl(u8 mode, u8 cmd_idx, u8 *value)
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
            wifi_get_sta_cfg_from_share_memory(cmd_idx, value);
            break;
        case MLME_CMD_SET_PARAM:
            wifi_set_sta_cfg_req(cmd_idx, value);
            break;
        default:
            break;
    }
    
    return TRUE;
}

RET_DATA wpa_driver_netlink_init_fp_t wpa_driver_netlink_init;
RET_DATA wpa_driver_netlink_deinit_fp_t wpa_driver_netlink_deinit ;
RET_DATA wpa_driver_netlink_scan_fp_t wpa_driver_netlink_scan;
RET_DATA wpa_driver_netlink_get_scan_results_fp_t wpa_driver_netlink_get_scan_results;
RET_DATA wpa_driver_netlink_connect_fp_t wpa_driver_netlink_connect;
RET_DATA wpa_driver_netlink_reconnect_fp_t wpa_driver_netlink_reconnect;
RET_DATA wpa_driver_netlink_associate_fp_t wpa_driver_netlink_associate;
RET_DATA wpa_driver_netlink_disconnect_fp_t wpa_driver_netlink_disconnect;
RET_DATA wpa_driver_netlink_deauthenticate_fp_t wpa_driver_netlink_deauthenticate;
RET_DATA wpa_driver_netlink_get_bssid_fp_t wpa_driver_netlink_get_bssid;
RET_DATA wpa_driver_netlink_get_ssid_fp_t wpa_driver_netlink_get_ssid;
RET_DATA wpa_driver_netlink_get_mac_fp_t wpa_driver_netlink_get_mac;
RET_DATA wpa_driver_netlink_set_key_fp_t wpa_driver_netlink_set_key;
RET_DATA wpa_driver_netlink_scan_results_free_fp_t wpa_driver_netlink_scan_results_free;
//RET_DATA wpa_drv_set_key_fp_t wpa_drv_set_key;
RET_DATA wpa_drv_send_eapol_fp_t wpa_drv_send_eapol;
RET_DATA wpa_driver_netlink_connect_by_bssid_fp_t wpa_driver_netlink_connect_by_bssid;
RET_DATA wpa_driver_netlink_set_mac_fp_t wpa_driver_netlink_set_mac;
RET_DATA wpa_driver_netlink_get_rssi_fp_t wpa_driver_netlink_get_rssi;
RET_DATA wpa_driver_netlink_show_scan_results_fp_t wpa_driver_netlink_show_scan_results;
RET_DATA wpa_driver_netlink_scan_results_free_fp_t wpa_driver_netlink_scan_results_clear;
RET_DATA wpa_driver_netlink_fast_connect_fp_t wpa_driver_netlink_fast_connect;
RET_DATA wpa_driver_netlink_sta_cfg_fp_t wpa_driver_netlink_sta_cfg;

/*
   Interface Initialization: WPA Driver
 */
void wpa_driver_func_init(void)
{
    wpa_driver_netlink_init =wpa_driver_netlink_init_impl;
    wpa_driver_netlink_deinit = wpa_driver_netlink_deinit_impl;
    wpa_driver_netlink_scan = wpa_driver_netlink_scan_impl;
    wpa_driver_netlink_get_scan_results = wpa_driver_netlink_get_scan_results_impl;
    wpa_driver_netlink_connect = wpa_driver_netlink_connect_impl;
    wpa_driver_netlink_reconnect = wpa_driver_netlink_reconnect_impl;
    wpa_driver_netlink_associate = wpa_driver_netlink_associate_impl;
    wpa_driver_netlink_disconnect = wpa_driver_netlink_disconnect_impl;
    wpa_driver_netlink_deauthenticate = wpa_driver_netlink_deauthenticate_impl;
    wpa_driver_netlink_get_bssid = wpa_driver_netlink_get_bssid_impl;
    wpa_driver_netlink_get_ssid = wpa_driver_netlink_get_ssid_impl;
    wpa_driver_netlink_get_mac = wpa_driver_netlink_get_mac_impl;
    wpa_driver_netlink_set_key = wpa_driver_netlink_set_key_impl;
    wpa_driver_netlink_scan_results_free = wpa_driver_netlink_scan_results_free_impl;
    //wpa_drv_set_key = wpa_drv_set_key_impl;
    wpa_drv_send_eapol = wpa_drv_send_eapol_impl;
    wpa_driver_netlink_connect_by_bssid = wpa_driver_netlink_connect_by_bssid_impl;
    wpa_driver_netlink_set_mac = wpa_driver_netlink_set_mac_impl;
    wpa_driver_netlink_get_rssi = wpa_driver_netlink_get_rssi_impl;
    wpa_driver_netlink_show_scan_results = wpa_driver_netlink_show_scan_results_impl;
    wpa_driver_netlink_scan_results_clear = wpa_driver_netlink_scan_results_clear_impl;
    wpa_driver_netlink_fast_connect        = wpa_driver_netlink_fast_connect_impl;
    wpa_driver_netlink_sta_cfg             = wpa_driver_netlink_sta_cfg_impl;
}


