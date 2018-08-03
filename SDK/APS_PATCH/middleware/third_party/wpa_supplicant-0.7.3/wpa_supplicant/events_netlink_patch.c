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

#include "includes.h"
#include "common.h"
#include "eapol_supp_sm.h"
#include "wpa.h"
#include "eloop.h"
#include "config.h"
#include "l2_packet.h"
#include "wpa_supplicant_i.h"
#include "driver_i.h"
#include "pcsc_funcs.h"
#include "preauth.h"
#include "pmksa_cache.h"
#include "wpa_ctrl.h"
#include "eap.h"
#include "notify.h"
#include "ieee802_11_defs.h"
#include "wpas_glue.h"
#include "wps_supplicant.h"
#include "sme.h"
#include "ap.h"
#include "bss.h"
#include "mlme.h"
#include "scan.h"
#include "controller_wifi_com.h"
#include "wpa_i.h"
#include "driver_netlink.h"
#include "events_netlink.h"
#include "Sha1-pbkdf2.h"
#include "at_cmd_common.h"
#include "events_netlink_patch.h"
#include "wpa_patch.h"
#include "wpa_debug_patch.h"
#include "controller_wifi_com_patch.h"
#include "wifi_api.h"
#include "events_netlink_patch.h"

extern struct wpa wpa;
extern char g_passphrase[MAX_LEN_OF_PASSWD];
extern u8 g_wpa_psk[32];
     
void wpa_supplicant_event_assoc_patch(struct wpa_supplicant *wpa_s,
				       union wpa_event_data *data)
{
	u8 bssid[ETH_ALEN]={0};
    asso_data *pdata;
    u8 mac_state;
    u8 *ssid;
    //int i;
//    int i, j;
    u8 *pReqIE;
    scan_info_t *pInfo = NULL;
    auto_conn_info_t *pacInfo = NULL;
    u8 rsn_ie_len;
    u8 wpa_ie_len;
    u8 asso_ie_len;
    //char passphrase[32] = "12345678";
    //2a a8 67 d0 ee 53 3e 90 a3 94 6d 5 41 9e 68 cc 7b 8a 91 65 ef d ad ee ee 6e b5 87 d9 e5 2e 2b
//    u8 temp_psk[32] = {0x2a, 0xa8, 0x67, 0xd0, 0xee, 0x53, 0x3e, 0x90,
//                       0xa3, 0x94, 0x6d, 0x5, 0x41, 0x9e, 0x68, 0xcc,
//                       0x7b, 0x8a, 0x91, 0x65, 0xef, 0xd, 0xad, 0xee,
//                       0xee, 0x6e, 0xb5, 0x87, 0xd9, 0xe5, 0x2e, 0x2b};

    struct wpa_sm *sm = NULL;

    if(wpa_s == NULL) return;

    sm = wpa.supp;

    //os_memset(passphrase, 0, 32);

    wpa_driver_netlink_get_bssid(bssid);
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: bssid=%02x:%02x:%02x:%02x:%02x:%02x \r\n", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

    os_memcpy(wpa_s->bssid, bssid, ETH_ALEN);

	//wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATED);

	//wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: Association event - clear replay counter \r\n");
	os_memcpy(sm->bssid, bssid, ETH_ALEN);
	os_memset(sm->rx_replay_counter, 0, WPA_REPLAY_COUNTER_LEN);
	sm->rx_replay_counter_set = 0;

    //Store ap data to bss_info
    pdata = wifi_get_asso_data();
	wpa_s->pairwise_cipher = pdata->pairwise_cipher;
	wpa_s->group_cipher = pdata->group_cipher;
    wpa_s->key_mgmt = pdata->key_mgmt;
	wpa_s->mgmt_group_cipher = pdata->mgmt_group_cipher;

#if 0
    wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: key_mgmt:%d pairwise_cipher:%d group_cipher:%d mgmt_group_cipher:%d \r\n",
                           wpa_s->key_mgmt,
                           wpa_s->pairwise_cipher,
                           wpa_s->group_cipher,
                           wpa_s->mgmt_group_cipher);
#endif

    wpa_sm_set_param(wpa_s->wpa, WPA_PARAM_PROTO, pdata->proto);
    wpa_sm_set_param(wpa_s->wpa, WPA_PARAM_PAIRWISE, wpa_s->pairwise_cipher);
    wpa_sm_set_param(wpa_s->wpa, WPA_PARAM_GROUP, wpa_s->group_cipher);
    wpa_sm_set_param(wpa_s->wpa, WPA_PARAM_KEY_MGMT, wpa_s->key_mgmt);
    wpa_sm_set_param(wpa_s->wpa, WPA_PARAM_MGMT_GROUP, wpa_s->mgmt_group_cipher);


	//wpa_s->eapol_received = 0;

    //Check if it's open system connection, it means it's connected
    //Check if it's security connection, it means it's not the final state, the following eapol key frame event will come
    mac_state = wifi_get_mac_state();
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: mac state:%d \r\n", mac_state);

#if 0
    if (mac_state == MAC_STA_4) {
        wpa_supplicant_set_state(wpa_s, WPA_COMPLETED);
        wpa_printf_dbg(MSG_DEBUG, "\r\nconnected\r\n");
    }
#else
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: key_mgmt:%d \r\n", wpa_s->key_mgmt);
	if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
	    wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE){
        wpa_supplicant_set_state(wpa_s, WPA_COMPLETED);
        wpa_printf_dbg(MSG_DEBUG, "\r\n\r\nconnected\r\n\r\n");
    }
#endif

#if 0
	if ((wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
	     wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) &&
	    wpa_s->current_ssid && wpa_drv_get_capa(wpa_s, &capa) == 0 &&
	    capa.flags & WPA_DRIVER_FLAGS_SET_KEYS_AFTER_ASSOC_DONE) {
		/* Set static WEP keys again */
		wpa_set_wep_keys(wpa_s, wpa_s->current_ssid);
	}
#endif

    pReqIE = wifi_get_asso_ie();
    asso_ie_len = pReqIE[1] + 2; //pReqIE[1];
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: Asso IE length: %d \r\n", asso_ie_len);
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: Asso IE: ");
    //for (i = 0; i < asso_ie_len; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", *(pReqIE+i));
    //wpa_printf_dbg(MSG_DEBUG, "\r\n");

    //Set ASSOC IE
    wpa_sm_set_assoc_wpa_ie(wpa_s->wpa, pReqIE, asso_ie_len);


    pInfo = wifi_get_scan_record(wpa_s->bssid);
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: pInfo->bssid:%02x:%02x:%02x:%02x:%02x:%02x \r\n", pInfo->bssid[0],pInfo->bssid[1],pInfo->bssid[2],pInfo->bssid[3],pInfo->bssid[4],pInfo->bssid[5]);

    wpa_s->assoc_freq = pInfo->ap_channel;
    
    //Get RSN IE
    rsn_ie_len = pInfo->rsn_ie[1];
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: RSN IE length:%d \r\n", rsn_ie_len);
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: rsn_ie:");
    //for(i = 0; i < rsn_ie_len; i++) wpa_printf_dbg(MSG_DEBUG, "%x ", pInfo->rsn_ie[i]);
    //wpa_printf_dbg(MSG_DEBUG, "\r\n");

    //Get WPA IE
    wpa_ie_len = pInfo->wpa_ie[1];
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: WPA IE length: %d \r\n", wpa_ie_len);
    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: wpa_ie: ");
    //for(i = 0; i < wpa_ie_len; i++) wpa_printf_dbg(MSG_DEBUG, "%x ", pInfo->wpa_ie[i]);
    //wpa_printf_dbg(MSG_DEBUG, "\r\n");

    //Set RSN/WPA IE
    //wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_event_assoc, call wpa_sm_set_ap_rsn_ie \r\n");
    wpa_sm_set_ap_rsn_ie(wpa_s->wpa, pInfo->rsn_ie, rsn_ie_len);

    //wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_event_assoc, call wpa_sm_set_ap_wpa_ie \r\n");
    wpa_sm_set_ap_wpa_ie(wpa_s->wpa, pInfo->wpa_ie, wpa_ie_len);

    //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: call wpa_sm_set_pmk \r\n");
    //wpa_supplicant_set_suites(wpa_s, NULL, ssid, wpa_ie, &wpa_ie_len)

    if (mac_state == MAC_STA_3) {

        //SSID
        ssid = wifi_get_ssid();

#if 0
        wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: ssid: ");
        for(i=0; i<IEEE80211_MAX_SSID_LEN; i++){
            wpa_printf_dbg(MSG_DEBUG, "%c", *(ssid + i));
        }
        wpa_printf_dbg(MSG_DEBUG, "\r\n");
#endif

        //We should copy ssid to structure here, TBD
        //Passphrase, use a fixed value here temporarily
        //We should copy ssid to structure here, TBD
        //wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_event_assoc, passphrase: ");
        //for(i=0; i<os_strlen(ssid); i++){
        //    wpa_printf_dbg(MSG_DEBUG, "%c", *(passphrase + i));

#if 0
        wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: g_passphrase: ");
        for(i=0; i<os_strlen(g_passphrase); i++){
            wpa_printf_dbg(MSG_DEBUG, "%c", *(g_passphrase + i));
        }
        wpa_printf_dbg(MSG_DEBUG, "\r\n");
#endif

        //wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_event_assoc, wpa.psk(before): ");
        //for(i = 0; i < PMK_LEN; i++) wpa_printf_dbg(MSG_DEBUG, "%x ", wpa.psk[i]);

        //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: caculate PMK, os_strlen(ssid):%d, call pbkdf2_sha1 \r\n", os_strlen((const char*)ssid));

        //1.Caculate the PMK
        //pbkdf2_sha1(passphrase, (char*)ssid, os_strlen((const char*)ssid), 4096, wpa.psk, 32);
        
        if (get_auto_connect_mode() != AUTO_CONNECT_ENABLE) {
            pbkdf2_sha1(g_passphrase, (char*)ssid, os_strlen((const char*)ssid), 4096, wpa.psk, 32);
            wpa_sm_set_pmk(wpa_s->wpa, wpa.psk, PMK_LEN);
            memcpy(&g_wpa_psk[0], wpa.psk, 32);
        }
        else {
            pacInfo = wifi_get_ac_record(wpa_s->bssid);
            wpa_sm_set_pmk(wpa_s->wpa, pacInfo->psk, PMK_LEN);
        }
        
        //os_memcpy(wpa.psk, temp_psk, 32);

        //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: after pbkdf2_sha1, wpa.psk:");
        //for(i = 0; i < PMK_LEN; i++) wpa_printf_dbg(MSG_DEBUG, "%x ", wpa.psk[i]);
        //wpa_printf_dbg(MSG_DEBUG, "\r\n");

        //wpa_printf_dbg(MSG_DEBUG, "[HDL_ASSO]WPA: set pmk \r\n");

        //2.Set the PMK
        //wpa_sm_set_pmk(wpa_s->wpa, wpa.psk, PMK_LEN);
    }
#ifdef __WIFI_AUTO_CONNECT__
    else { //Open connection
        /* Set successfully connect info to Auto Connect list */
        switch(get_auto_connect_mode()) {
            case AUTO_CONNECT_MANUAL:
                add_auto_connect_list();
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
                break;
            default:
                break;
        }
    }
#endif
    //wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_event_assoc, PMK[%d]: ", PMK_LEN);
    //for(j = 0; j < PMK_LEN; j++){
    //    //wpa_printf_dbg(MSG_DEBUG, "%d ", wpa_s->wpa.pmk[j]);
    //}
    //wpa_printf_dbg(MSG_DEBUG, "\r\n");
}

/*
   Interface Initialization: WPA Events
 */
void wpa_events_func_init_patch(void)
{
    wpa_supplicant_event_assoc = wpa_supplicant_event_assoc_patch;
}
