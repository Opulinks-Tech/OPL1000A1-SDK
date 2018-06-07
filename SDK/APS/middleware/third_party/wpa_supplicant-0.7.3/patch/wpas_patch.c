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
//#include "armcm3.h"
#include "msg.h"
#include "wpas_patch.h"

#include "wpa_common.h"
#include "wpa_i.h"
#include "wpa_ie.h"
#include "os.h"
#include "common.h"
#include "defs.h"
#include "wpa_debug.h"
//#include "driver_netlink.h"

#if 0
uint32_t wpa_cli_cmd_handler_v1(int argc, char *argv[])
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
        msg_print(LOG_HIGH_LEVEL, "not support yet \r\n");
    } else if (os_strncasecmp(WPA_CLI_CMD_STATE, argv[0], os_strlen(argv[0])) == 0) {
        //msg_print(LOG_HIGH_LEVEL, "state=%s \r\n", wpa_supplicant_state_txt(wpa_s->wpa_state));
    } else if (os_strncasecmp(WPA_CLI_CMD_ADD_NETWORK, argv[0], os_strlen(argv[0])) == 0) {
        msg_print(LOG_HIGH_LEVEL, "not support yet \r\n");
    } else if (os_strncasecmp(WPA_CLI_CMD_SET_NETWORK, argv[0], os_strlen(argv[0])) == 0) {
        msg_print(LOG_HIGH_LEVEL, "not support yet \r\n");
    } else if (os_strncasecmp(WPA_CLI_CMD_SAVE_CONFIG, argv[0], os_strlen(argv[0])) == 0) {
        msg_print(LOG_HIGH_LEVEL, "not support yet \r\n");
    } else if (os_strncasecmp(WPA_CLI_CMD_MAC, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_mac_by_param(argc, argv);
    } else if (os_strncasecmp(WPA_CLI_CMD_SCRT_DEBUG, argv[0], os_strlen(argv[0])) == 0) {
        wpa_cli_scrt_dbg_by_param(argc, argv);
    } else {
        //nothing
        msg_print(LOG_HIGH_LEVEL, "\r\n");
    }

    return TRUE;
}
#endif

#if 0
void wpa_supplicant_process_1_of_4_v1(struct wpa_sm *sm,
					  const unsigned char *src_addr,
					  const struct wpa_eapol_key *key,
					  u16 ver)
{
	struct wpa_eapol_ie_parse ie;
	struct wpa_ptk *ptk;
	u8 buf[8];
	int res;
    int i;
    u8 mac[ETH_ALEN]={0};
    u8 bssid[ETH_ALEN]={0};


    //wpa_printf(MSG_DEBUG, "\r\n\r\n wpa_supplicant_process_1_of_4, sm->proto:%d ver:%d ", sm->proto, ver);
	wpa_sm_set_state(sm, WPA_4WAY_HANDSHAKE);
	//wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, RX message 1 of 4-Way Handshake from "
	//	   MACSTR " (ver=%d)", MAC2STR(src_addr), ver); //80:26:89:58:64:d3 (ver=2)  =>correct
	os_memset(&ie, 0, sizeof(ie));

//#ifndef CONFIG_NO_WPA2
	if (sm->proto == WPA_PROTO_RSN) { //enter
        //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, sm->proto == WPA_PROTO_RSN");

		/* RSN: msg 1/4 should contain PMKID for the selected PMK */
		const u8 *_buf = (const u8 *) (key + 1);
		size_t len = WPA_GET_BE16(key->key_data_length);

        //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, len:%d ", len); //key data len:22  =>correct
		wpa_hexdump(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, RSN: msg 1/4 key data", _buf, len); //dd 14 00 0f ac 04 ba 37 7b 1a e7 dd 3e e9 5b 06 ab dc 4a ad 81 54

		wpa_supplicant_parse_ies(_buf, len, &ie);

		if (ie.pmkid) { //enter
			wpa_hexdump(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, RSN: PMKID from "
				    "Authenticator", ie.pmkid, PMKID_LEN); //hexdump(len=16): ba 37 7b 1a e7 dd 3e e9 5b 06 ab dc 4a ad 81 54
		}
	}
//#endif /* CONFIG_NO_WPA2 */

	if (sm->renew_snonce) { //enter
		if (os_get_random(sm->snonce, WPA_NONCE_LEN)) { //no enter
			//wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
			//	"\r\n wpa_supplicant_process_1_of_4, WPA: Failed to get random data for SNonce ");
			goto failed;
		}
		sm->renew_snonce = 0;
		wpa_hexdump(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, Renewed SNonce ",
			    sm->snonce, WPA_NONCE_LEN); //hexdump(len=32): 02 07 01 06 06 03 09 00 00 05 07 02 02 01 05 04 02 07 07 00 04 05 07 02 06 09 07 02 02 01 07 00
	}

	/* Calculate PTK which will be stored as a temporary PTK until it has
	 * been verified when processing message 3/4. */
	ptk = &sm->tptk;

#if 0
    wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, before wpa_derive_ptk, src_addr[6]:%02x:%02x:%02x:%02x:%02x:%02x ",
                                                                                                                        src_addr[0],
                                                                                                                        src_addr[1],
                                                                                                                        src_addr[2],
                                                                                                                        src_addr[3],
                                                                                                                        src_addr[4],
                                                                                                                        src_addr[5]); //src_addr[6]:80:26:89:58:64:d3
#endif
    wpa_driver_netlink_get_mac(mac);
    os_memcpy(sm->own_addr, mac, ETH_ALEN);
    os_memcpy(sm->bssid, src_addr, ETH_ALEN);

#if 0
    wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, before wpa_derive_ptk, sm->own_addr[6]:%02x:%02x:%02x:%02x:%02x:%02x ", sm->own_addr[0],
                                                                                                           sm->own_addr[1],
                                                                                                           sm->own_addr[2],
                                                                                                           sm->own_addr[3],
                                                                                                           sm->own_addr[4],
                                                                                                           sm->own_addr[5]); //sm->own_addr[6]:22:33:44:55:66:76

    wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, before wpa_derive_ptk, sm->bssid[6]:%02x:%02x:%02x:%02x:%02x:%02x ", sm->bssid[0],
                                                                                                        sm->bssid[1],
                                                                                                        sm->bssid[2],
                                                                                                        sm->bssid[3],
                                                                                                        sm->bssid[4],
                                                                                                        sm->bssid[5]); //sm->bssid[6]:80:26:89:58:64:d3

    wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, sizeof(struct wpa_ptk):%d ", sizeof(struct wpa_ptk));
    wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, call wpa_derive_ptk ");
#endif

	wpa_derive_ptk(sm, src_addr, key, ptk);

    //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, after wpa_derive_ptk ");

	/* Supplicant: swap tx/rx Mic keys */

    sm->tptk_set = 1;

    //wpa_printf(MSG_INFO, "\r\n wpa_supplicant_process_1_of_4, call wpa_supplicant_send_2_of_4 ");

	if (wpa_supplicant_send_2_of_4(sm, sm->bssid, key, ver, sm->snonce,
				       sm->assoc_wpa_ie, sm->assoc_wpa_ie_len,
				       ptk))
		goto failed;

    //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, after wpa_supplicant_send_2_of_4 ");
    //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, key->key_nonce[]: ");
    //for(i=0;i<WPA_NONCE_LEN;i++) wpa_printf(MSG_DEBUG, "%02x ", key->key_nonce[i]);;

	os_memcpy(sm->anonce, key->key_nonce, WPA_NONCE_LEN);
	return;

failed:
	//wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}
#endif

void wpas_patch_init(void)
{
    //Sample1: Replace the WPA CLI CMD Handler, it's working
    //wpa_cli_cmd_handler = wpa_cli_cmd_handler_v1;

    //Sample2: Replace the Key Exchange behavior, it's working
    //wpa_supplicant_process_1_of_4 = wpa_supplicant_process_1_of_4_v1;
}

/*
  How to use it?
  In main_patch.c, in __Patch_EntryPoint(), call wpa_patch_init() to reassign the golbal pointer for WPA module
*/



