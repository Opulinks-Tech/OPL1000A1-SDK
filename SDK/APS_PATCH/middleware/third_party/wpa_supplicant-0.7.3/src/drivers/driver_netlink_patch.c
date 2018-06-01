/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlink Communication Corp. (C) 2017
******************************************************************************/
#include <stdlib.h>
#include "common.h"
#include "driver.h"
#include "controller_wifi_com.h"
#include "defs.h"
#include "at_cmd_msg_ext.h"
#include "driver_netlink.h"
#include "driver_netlink_patch.h"
#include "wifi_mac_data.h"
#include "wpabuf.h"
#include "wpa_supplicant_i.h"
#include "wpa_common_patch.h"
#include "controller_wifi_com_patch.h"

extern struct wpa_supplicant *wpa_s;

RET_DATA wpa_driver_netlink_show_scan_results_fp_t wpa_driver_netlink_show_scan_results;
RET_DATA wpa_driver_netlink_scan_results_free_fp_t wpa_driver_netlink_scan_results_clear;
RET_DATA wpa_driver_netlink_fast_connect_fp_t wpa_driver_netlink_fast_connect;
RET_DATA wpa_driver_netlink_sta_cfg_fp_t wpa_driver_netlink_sta_cfg;
RET_DATA wpa_driver_netlink_get_sta_cfg_fp_t wpa_driver_netlink_get_sta_cfg;

Boolean wpa_driver_netlink_get_scan_results_patch(struct wpa_scan_results * scan_res)
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

void wpa_driver_netlink_show_scan_results_patch(void)
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

void wpa_driver_netlink_scan_results_clear_patch(struct wpa_scan_results *res)
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

int wpa_drv_send_eapol_patch(const u8 *dst, u16 proto, u8 *data, size_t data_len)
{
    //msg_print(LOG_HIGH_LEVEL, "[KEY][DRV]WPA: ready to send eapol key frame, proto:%d data_len:%d \r\n", proto, data_len);
    //call data path api to send it out
    wifi_mac_xmit_eapol_frame(data, data_len, dst, proto);

    return 1;
}

Boolean wpa_driver_netlink_get_ssid_patch(u8 *ssid)
{
    if (ssid == NULL) return FALSE;

    os_memcpy(ssid, wifi_get_ssid(), MAX_LEN_OF_SSID + 1);

    return TRUE;
}

Boolean wpa_driver_netlink_scan_patch(int mode)
{
    //msg_print(LOG_HIGH_LEVEL, "wpa_driver_netlink_scan, mode:%d \r\n", mode);
    if((mode < SCAN_MODE_ACTIVE_EXT)||(mode >= SCAN_MODE_NUM_EXT)) return FALSE;
    if (wpa_s->wpa_state != WPA_COMPLETED) {
        wpa_supplicant_set_state(wpa_s, WPA_SCANNING);
    }
    wifi_scan_req(mode);
    return TRUE;
}

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

Boolean wpa_driver_netlink_fast_connect_patch(u8 mode, u8 index)
{
    auto_connect_cfg_t *pac_info = NULL;
    mw_wifi_auto_connect_ap_info_t mw_ac_info;
    
    if (mode > TRUE) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: Invalid Parameter \r\n");
        return FALSE;
    }

    if (index >= MAX_NUM_OF_AUTO_CONNECT) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: Invalid Parameter \r\n");
        return FALSE;
    }

    pac_info = wifi_get_ac_result();
        
    if (pac_info->pFCInfo[index].bssid[0] == 0 || pac_info->pFCInfo[index].ap_channel == 0) {
        msg_print(LOG_HIGH_LEVEL, "[DRV]WPA: No target AP available \r\n");
        return FALSE;
    }

    pac_info->pFCInfo[index].fast_connect = TRUE;


    read_auto_conn_ap_info_from_flash(index, &mw_ac_info);
    mw_ac_info.fast_connect = mode;
    write_auto_connect_ap_info_to_flash(index, &mw_ac_info);
    
    wifi_sta_join_fast(index);
    return TRUE;
}

Boolean wpa_driver_netlink_get_sta_cfg_patch(u8 cfg_idx, void *ptr)
{
    if (ptr == NULL) {
        return false;
    }
    
    if (cfg_idx > E_WIFI_PARAM_BSS_DTIM_PERIOD) {
        return false;
    }

    wifi_get_sta_cfg_from_share_memory(cfg_idx, ptr);
    
    return TRUE;
}

/*
   Interface Initialization: WPA Driver
 */
void wpa_driver_func_init_patch(void)
{
    wpa_driver_netlink_get_scan_results = wpa_driver_netlink_get_scan_results_patch;
    wpa_driver_netlink_show_scan_results = wpa_driver_netlink_show_scan_results_patch;
    wpa_driver_netlink_scan_results_clear = wpa_driver_netlink_scan_results_clear_patch;
    wpa_drv_send_eapol = wpa_drv_send_eapol_patch;
    wpa_driver_netlink_get_ssid = wpa_driver_netlink_get_ssid_patch;
    wpa_driver_netlink_scan = wpa_driver_netlink_scan_patch;
    wpa_driver_netlink_fast_connect = wpa_driver_netlink_fast_connect_patch;
    wpa_driver_netlink_sta_cfg = wpa_driver_netlink_sta_cfg_patch;
    wpa_driver_netlink_get_sta_cfg = wpa_driver_netlink_get_sta_cfg_patch;
    return;
}

