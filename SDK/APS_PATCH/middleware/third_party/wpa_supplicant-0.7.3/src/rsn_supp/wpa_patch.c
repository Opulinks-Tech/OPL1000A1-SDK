/*
 * WPA Supplicant - WPA state machine and EAPOL-Key processing
 * Copyright (c) 2003-2010, Jouni Malinen <j@w1.fi>
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
#include "aes_wrap.h"
#include "crypto.h"
#include "ieee802_11_defs.h"
#include "eapol_supp_sm.h"
#include "wpa.h"
#include "eloop.h"
#include "preauth.h"
#include "pmksa_cache.h"
#include "wpa_i.h"
#include "wpa_ie.h"
#include "peerkey.h"
#include "driver_netlink.h"
#include "config_ssid.h"
#include "wpa.h"
#include "controller_wifi_com.h"
#include "at_cmd_common.h"
#include "wpa_debug_patch.h"
#include "wpa_patch.h"
#include "controller_wifi_com_patch.h"
#include "wifi_api.h"

extern struct ieee802_1x_hdr msg_1_4_hdr;
extern struct wpa_ptk g_ptk;
extern struct wpa_gtk_data g_gtk;

extern char g_passphrase[MAX_LEN_OF_PASSWD];
extern u16 g_key_info;
extern unsigned int g_frame_len;
extern u16 g_key_info_1_4;
extern u16 g_key_info_3_4;
//extern u8 g_kck[16]; /* EAPOL-Key Key Confirmation Key (KCK) */ //For GTK Exchange Usage

extern struct wpa_supplicant *wpa_s;
extern struct wpa_ptk ptk;

extern int g_DbgMode;

extern wpa_alloc_eapol_fp_t wpa_alloc_eapol;
extern wpa_derive_ptk_fp_t wpa_derive_ptk;

RET_DATA u8 g_wpa_psk[32];

#if 0
void wpa_printf_dbg(int level, char *fmt, ...)
{
    if(g_DbgMode)
    {
        msg_print(LOG_HIGH_LEVEL, fmt);
    }
}
#endif

int wpa_derive_ptk_patch(struct wpa_sm *sm, const unsigned char *src_addr,
			  const struct wpa_eapol_key *key,
			  struct wpa_ptk *ptk);
int wpa_derive_ptk_patch(struct wpa_sm *sm, const unsigned char *src_addr,
			  const struct wpa_eapol_key *key,
			  struct wpa_ptk *ptk)
{
	size_t ptk_len = sm->pairwise_cipher == WPA_CIPHER_CCMP ? 48 : 64;
    int i;

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: sm->pairwise_cipher:%d ptk_len:%d sm->pmk_len:%d \r\n", sm->pairwise_cipher, ptk_len, sm->pmk_len); //sm->pairwise_cipher:16 ptk_len:48 //sm->pmk_len:32

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: sm->pmk:");       //sm->pmk:30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30
    for(i=0; i<PMK_LEN; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", sm->pmk[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: sm->own_addr:"); //sm->own_addr:22 33 44 55 66 76
    for(i=0; i<ETH_ALEN; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", sm->own_addr[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: sm->bssid:");    //sm->bssid:80 26 89 58 64 d3
    for(i=0; i<ETH_ALEN; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", sm->bssid[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: sm->snonce:");   //sm->snonce:02 07 01 06 06 03 09 00 00 05 07 02 02 01 05 04 02 07 07 00 04 05 07 02 06 09 07 02 02 01 07 00
    for(i=0; i<WPA_NONCE_LEN; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", sm->snonce[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: key->key_nonce:"); //key->key_nonce:59 b5 5e d7 b5 ad 96 8d 6a 4f 6d 67 47 eb 5c 59 a1 41 ae f2 80 0e 0f ca ad 47 b0 99 da 80 ef 1a
    for(i=0; i<WPA_NONCE_LEN; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", key->key_nonce[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ptk:");     //ptk:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    for(i=0; i<ptk_len; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", ptk[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: sm->key_mgmt:%d \r\n", sm->key_mgmt);  //sm->key_mgmt:2

	wpa_pmk_to_ptk(sm->pmk, sm->pmk_len, "Pairwise key expansion",
		       sm->own_addr, sm->bssid, sm->snonce, key->key_nonce,
		       (u8 *) ptk, ptk_len,
		       wpa_key_mgmt_sha256(sm->key_mgmt));

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ptk->kck:"); //ptk->kck[16]: 0e 7c 24 4c 85 25 d9 aa 16 6a bc f5 1c 2f c8 1a
    for(i=0; i<16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", ptk->kck[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ptk->kek:"); //ptk->kek[16]: e4 d4 4d 81 87 2f 3e 7e 3b e9 94 10 2c e9 8b df
    for(i=0; i<16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", ptk->kek[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ptk->tk1:"); //ptk->tk1[16]: 52 d0 d6 5a 1a db 3b d4 2c 67 c2 5b cf a5 1d 73
    for(i=0; i<16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", ptk->tk1[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ptk->u.tk2:"); //ptk->u.tk2[16]: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    for(i=0; i<16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", ptk->u.tk2[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

	return 0;
}

void wpa_supplicant_process_1_of_4_patch(struct wpa_sm *sm,
					  const unsigned char *src_addr,
					  const struct wpa_eapol_key *key,
					  u16 ver)
{
	struct wpa_eapol_ie_parse ie;
	struct wpa_ptk *ptk;
	u8 buf[8];
//	int res;
    //int i;
    u8 mac[ETH_ALEN]={0};
//    u8 bssid[ETH_ALEN]={0};
/*
	if (wpa_sm_get_network_ctx(sm) == NULL) {
		wpa_printf_dbg(MSG_WARNING, "WPA: No SSID info found (msg 1 of "
			   "4).");
		return;
	}
*/

    wpa_printf_dbg(MSG_DEBUG, "[1_4]WPA: sm->proto:%d ver:%d \r\n", sm->proto, ver);

	wpa_sm_set_state(sm, WPA_4WAY_HANDSHAKE);

	//wpa_printf_dbg(MSG_DEBUG, "[1_4]WPA: RX message 1 of 4-Way Handshake from " MACSTR " (ver=%d) \r\n", MAC2STR(src_addr), ver); //80:26:89:58:64:d3 (ver=2)  =>correct

	os_memset(&ie, 0, sizeof(ie));

#ifndef CONFIG_NO_WPA2
	if (sm->proto == WPA_PROTO_RSN) { //enter
		/* RSN: msg 1/4 should contain PMKID for the selected PMK */
		const u8 *_buf = (const u8 *) (key + 1);
		size_t len = WPA_GET_BE16(key->key_data_length);

		wpa_hexdump(MSG_DEBUG, "[1_4]WPA: RSN: msg 1/4 key data",  _buf, len); //dd 14 00 0f ac 04 ba 37 7b 1a e7 dd 3e e9 5b 06 ab dc 4a ad 81 54

		wpa_supplicant_parse_ies(_buf, len, &ie);

		if (ie.pmkid) { //enter
			wpa_hexdump(MSG_DEBUG, "[1_4]WPA: RSN: PMKID from Authenticator ", ie.pmkid, PMKID_LEN); //hexdump(len=16): ba 37 7b 1a e7 dd 3e e9 5b 06 ab dc 4a ad 81 54
        }
	}
#endif /* CONFIG_NO_WPA2 */

	//res = wpa_supplicant_get_pmk(sm, src_addr, ie.pmkid);

/*
	if (res == -2) {
		wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, RSN: Do not reply to msg 1/4 - "
			   "requesting full EAP authentication\r\n");
		return;
	}
	if (res)
		goto failed;
*/

	if (sm->renew_snonce) { //enter
		if (os_get_random(sm->snonce, WPA_NONCE_LEN)) { //no enter
			wpa_printf_dbg(MSG_DEBUG, "[1_4]WPA: Failed to get random data for SNonce \r\n");
			goto failed;
		}

/*
        wpa_printf_dbg(MSG_DEBUG, "\r\n random[]:");
        for(i=0; i<WPA_NONCE_LEN; i++){
            wpa_printf_dbg(MSG_DEBUG, "%d ", sm->snonce[i]);
        }
*/

		sm->renew_snonce = 0;
		//wpa_hexdump(MSG_DEBUG, "[1_4]WPA: Renewed SNonce ", sm->snonce, WPA_NONCE_LEN); //hexdump(len=32): 02 07 01 06 06 03 09 00 00 05 07 02 02 01 05 04 02 07 07 00 04 05 07 02 06 09 07 02 02 01 07 00
    }

	/* Calculate PTK which will be stored as a temporary PTK until it has
	 * been verified when processing message 3/4. */
	ptk = &sm->tptk;

    //Initialize the key data to 0
    os_memset(ptk, 0, sizeof(sm->tptk));

    wpa_driver_netlink_get_mac(mac);
    os_memcpy(sm->own_addr, mac, ETH_ALEN);
    //wpa_driver_netlink_get_bssid(bssid);
    os_memcpy(sm->bssid, src_addr, ETH_ALEN);

#if 0
    wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, before wpa_derive_ptk, sm->own_addr[6]:%02x:%02x:%02x:%02x:%02x:%02x ", sm->own_addr[0],
                                                                                                           sm->own_addr[1],
                                                                                                           sm->own_addr[2],
                                                                                                           sm->own_addr[3],
                                                                                                           sm->own_addr[4],
                                                                                                           sm->own_addr[5]); //sm->own_addr[6]:22:33:44:55:66:76

    wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_process_1_of_4, before wpa_derive_ptk, sm->bssid[6]:%02x:%02x:%02x:%02x:%02x:%02x ", sm->bssid[0],
                                                                                                        sm->bssid[1],
                                                                                                        sm->bssid[2],
                                                                                                        sm->bssid[3],
                                                                                                        sm->bssid[4],
                                                                                                        sm->bssid[5]); //sm->bssid[6]:80:26:89:58:64:d3
#endif

    //ptk = os_malloc(sizeof(struct wpa_ptk)); //size=64
	wpa_derive_ptk(sm, src_addr, key, ptk);

	/* Supplicant: swap tx/rx Mic keys */
	os_memcpy(buf, ptk->u.auth.tx_mic_key, 8);
	os_memcpy(ptk->u.auth.tx_mic_key, ptk->u.auth.rx_mic_key, 8);
	os_memcpy(ptk->u.auth.rx_mic_key, buf, 8);
	sm->tptk_set = 1;

#if 0
    wpa_printf_dbg(MSG_DEBUG, "[1_4]WPA: ptk->u.auth.tx_mic_key:%02x%02x%02x%02x%02x%02x%02x%02x0 \r\n",
                          ptk->u.auth.tx_mic_key[0],
                          ptk->u.auth.tx_mic_key[1],
                          ptk->u.auth.tx_mic_key[2],
                          ptk->u.auth.tx_mic_key[3],
                          ptk->u.auth.tx_mic_key[4],
                          ptk->u.auth.tx_mic_key[5],
                          ptk->u.auth.tx_mic_key[6],
                          ptk->u.auth.tx_mic_key[7]);
    wpa_printf_dbg(MSG_DEBUG, "[1_4]WPA: ptk->u.auth.rx_mic_key:%02x%02x%02x%02x%02x%02x%02x%02x0 \r\n",
                      ptk->u.auth.rx_mic_key[0],
                      ptk->u.auth.rx_mic_key[1],
                      ptk->u.auth.rx_mic_key[2],
                      ptk->u.auth.rx_mic_key[3],
                      ptk->u.auth.rx_mic_key[4],
                      ptk->u.auth.rx_mic_key[5],
                      ptk->u.auth.rx_mic_key[6],
                      ptk->u.auth.rx_mic_key[7]);
#endif

    //sm->tptk_set = 1;

	if (wpa_supplicant_send_2_of_4(sm, sm->bssid, key, ver, sm->snonce, sm->assoc_wpa_ie, sm->assoc_wpa_ie_len, ptk)) goto failed;

#if 0
    wpa_printf_dbg(MSG_DEBUG, "[1_4]WPA: copy key_nonce to anonce, key->key_nonce:");
    for(i=0;i<WPA_NONCE_LEN;i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", key->key_nonce[i]);;
    wpa_printf_dbg(MSG_DEBUG, "\r\n");
#endif

	os_memcpy(sm->anonce, key->key_nonce, WPA_NONCE_LEN);
	return;

failed:
	wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);

}


/**
 * wpa_supplicant_send_2_of_4 - Send message 2 of WPA/RSN 4-Way Handshake
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @dst: Destination address for the frame
 * @key: Pointer to the EAPOL-Key frame header
 * @ver: Version bits from EAPOL-Key Key Info
 * @nonce: Nonce value for the EAPOL-Key frame
 * @wpa_ie: WPA/RSN IE
 * @wpa_ie_len: Length of the WPA/RSN IE
 * @ptk: PTK to use for keyed hash and encryption
 * Returns: 0 on success, -1 on failure
 */
int wpa_supplicant_send_2_of_4_patch(struct wpa_sm *sm, const unsigned char *dst,
			       const struct wpa_eapol_key *key,
			       int ver, const u8 *nonce,
			       const u8 *wpa_ie, size_t wpa_ie_len,
			       struct wpa_ptk *ptk)
{
	size_t rlen;
	struct wpa_eapol_key *reply;
	u8 *rbuf;
	//u8 *rsn_ie_buf = NULL;
    int i;
    int len_total;
    int ret;

    //wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: ver:%d wpa_ie_len:%d wpa_ie:", ver, wpa_ie_len); //wpa_ie_len:20  wpa_ie[]: 30 14 1 0 0 f ac 4 1 0 0 f ac 4 1 0 0 f ac 2 => no rns capabilityes 2 bytes, why?

    //for(i=0;i<wpa_ie_len;i++){
    //    wpa_printf_dbg(MSG_WARNING, "%02x ", wpa_ie[i]);
    //}
    //wpa_printf_dbg(MSG_WARNING, "\r\n");

	if (wpa_ie == NULL) {
		wpa_printf_dbg(MSG_WARNING, "[2_4]WPA: No wpa_ie set - cannot generate msg 2/4");
		return -1;
	}

	//wpa_hexdump(MSG_DEBUG, "[2_4]WPA: WPA IE for msg 2/4 ", wpa_ie, wpa_ie_len); //hexdump(len=20): 30 14 01 00 00 0f ac 04 01 00 00 0f ac 04 01 00 00 0f ac 02
    //wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: wpa_ie_len:%d \r\n", wpa_ie_len); //sizeof(*reply):95  wpa_ie_len:20

#if 0
#ifdef CONFIG_IEEE80211R
	if (wpa_key_mgmt_ft(sm->key_mgmt)) {
		int res;

		/*
		 * Add PMKR1Name into RSN IE (PMKID-List) and add MDIE and
		 * FTIE from (Re)Association Response.
		 */
		rsn_ie_buf = os_malloc(wpa_ie_len + 2 + 2 + PMKID_LEN +
				       sm->assoc_resp_ies_len);
		if (rsn_ie_buf == NULL)
			return -1;
		os_memcpy(rsn_ie_buf, wpa_ie, wpa_ie_len);
		res = wpa_insert_pmkid(rsn_ie_buf, wpa_ie_len,
				       sm->pmk_r1_name);
		if (res < 0) {
			os_free(rsn_ie_buf);
			return -1;
		}
		wpa_ie_len += res;

		if (sm->assoc_resp_ies) {
			os_memcpy(rsn_ie_buf + wpa_ie_len, sm->assoc_resp_ies,
				  sm->assoc_resp_ies_len);
			wpa_ie_len += sm->assoc_resp_ies_len;
		}

		wpa_ie = rsn_ie_buf;
	}
#endif /* CONFIG_IEEE80211R */
#endif //#if 0

    /*
          Total Length-
              sizeof(*reply) + wpa_ie_len
                  95         +     20    
    */
    len_total = sizeof(*reply) + wpa_ie_len;



#if 0
	rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY,
				  NULL, sizeof(*reply) + wpa_ie_len,
				  &rlen, (void *) &reply);
#else
    //here, ver should be the same as message 1's version of ieee802_1x_hdr
    /*
    rbuf = wpa_alloc_eapol(ver, IEEE802_1X_TYPE_EAPOL_KEY,
				  NULL, len_total,
				  &rlen, (void *) &reply);
    */
    rbuf = wpa_alloc_eapol((int)msg_1_4_hdr.version, IEEE802_1X_TYPE_EAPOL_KEY,
			  NULL, len_total,
			  &rlen, (void *) &reply);
    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: len_total:%d rlen:%d \r\n", len_total, rlen); //len_total:117 rlen:121
#endif //#if 0

	if (rbuf == NULL) {
		//os_free(rsn_ie_buf);
        wpa_printf_dbg(MSG_WARNING, "[2_4]WPA: rbuf == NULL \r\n");
		return -1;
	}

	reply->type = sm->proto == WPA_PROTO_RSN ? EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;

    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: reply->type:%d sm->proto:%d \r\n", reply->type, sm->proto); //reply->type:2 sm->proto:2

	WPA_PUT_BE16(reply->key_info, ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC); //correct

    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: reply->key_info:%x %x \r\n", reply->key_info[0], reply->key_info[1]);

#if 1
	if (sm->proto == WPA_PROTO_RSN){
		WPA_PUT_BE16(reply->key_length, 0);
	}else{
		os_memcpy(reply->key_length, key->key_length, 2);
		//WPA_PUT_BE16(reply->key_length, 0);
    }
#else
    WPA_PUT_BE16(reply->key_length, 0);
#endif

    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: reply->key_length:%x %x ,key->key_length:%x %x \r\n", reply->key_length[0],
                                                                                           reply->key_length[1],
                                                                                           key->key_length[0],
                                                                                           key->key_length[1]); //reply->key_length:0x0 0x10 ,key->key_length:0x0 0x10

	os_memcpy(reply->replay_counter, key->replay_counter, WPA_REPLAY_COUNTER_LEN);

    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: reply->replay_counter:%02x %02x %02x %02x %02x %02x %02x %02x \r\n",
                                                                                           reply->replay_counter[0],
                                                                                           reply->replay_counter[1],
                                                                                           reply->replay_counter[2],
                                                                                           reply->replay_counter[3],
                                                                                           reply->replay_counter[4],
                                                                                           reply->replay_counter[5],
                                                                                           reply->replay_counter[6],
                                                                                           reply->replay_counter[7]); //reply->replay_counter[]: 00 00 00 00 00 00 00 00


	WPA_PUT_BE16(reply->key_data_length, wpa_ie_len);
	//WPA_PUT_BE16(reply->key_data_length, wpa_ie_len + 2);

    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: reply->key_data_length:%x %x \r\n", reply->key_data_length[0], reply->key_data_length[1]); //reply->key_data_length[]: 00 16  => ok

	os_memcpy(reply + 1, wpa_ie, wpa_ie_len);

	//os_free(rsn_ie_buf);

	os_memcpy(reply->key_nonce, nonce, WPA_NONCE_LEN);

	wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: Sending EAPOL-Key 2/4 \r\n");

#if 0
    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: ptk->kck:"); //ptk->kck[16]: d2 3f a2 42 e7 c0 4a 47 9a 94 af 2b 80 6c fd 10
    for (i=0;i<16;i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", ptk->kck[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: dst:"); //dst: 80 26 89 58 64 d3 =>correct
    for (i=0;i<6;i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", *(dst+i));
    wpa_printf_dbg(MSG_DEBUG, "\r\n");
#endif

    //wpa_printf_dbg(MSG_DEBUG, "\r\n wpa_supplicant_send_2_of_4, ETH_P_EAPOL:%x ", ETH_P_EAPOL); //ETH_P_EAPOL:888e

    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: rbuf[%d]:", rlen);   //rbuf[121]: 02 03 00 75 02 01 0a 00 00 00 00 00 00 00 00 00 00 02 07 01 06 06 03 09 00 00 05 07 02 02 01 05 04 02 07 07 00 04 05 07 02 06 09 07 02 02 01 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 14 30 14 01 00 00 0f ac 04 01 00 00 0f ac 04 01 00 00 0f ac 02 00 00
    for (i=0;i<rlen;i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", *(rbuf+i));
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

#if 0
    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: reply->key_mic:"); //reply->key_mic[]: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  =>correct
    for (i=0;i<16;i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", reply->key_mic[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");
#endif

    //key_mic
    ret = wpa_eapol_key_mic(ptk->kck, ver, rbuf, rlen, reply->key_mic);
    // Prevent compiler warnings
    //(void)ret;

#if 0
    wpa_printf_dbg(MSG_DEBUG, "[2_4]WPA: reply->key_mic:"); //key_mic[16]:
    for(i=0;i<16;i++) wpa_printf_dbg(MSG_ERROR, "%02x ", reply->key_mic[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");
#endif

    //Send EAPOL Key Frame
	wpa_eapol_key_send(sm, ptk->kck, ver, dst, ETH_P_EAPOL, rbuf, rlen, reply->key_mic);

	return ret;
}

void wpa_supplicant_process_3_of_4_patch(struct wpa_sm *sm,
					  const struct wpa_eapol_key *key,
					  u16 ver)
{
	u16 key_info, keylen, len;
	const u8 *pos;
	struct wpa_eapol_ie_parse ie;
    int i;

	//wpa_sm_set_state(sm, WPA_4WAY_HANDSHAKE);
	wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: RX message 3 of 4-Way Handshake from "
		   MACSTR " (ver=%d) \r\n", MAC2STR(sm->bssid), ver);

	key_info = WPA_GET_BE16(key->key_info); //0x13ca => OK
	pos = (const u8 *) (key + 1);
	len = WPA_GET_BE16(key->key_data_length);

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: key_info:%x len:%d \r\n", key_info, len);
	wpa_hexdump(MSG_DEBUG, "[3_4]WPA: WPA: IE KeyData", pos, len);

	wpa_supplicant_parse_ies(pos, len, &ie);

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: after wpa_supplicant_parse_ies, os_strlen(ie.gtk):%d \r\n", os_strlen((const char*)(ie.gtk)));

    if (ie.gtk != NULL) { //enter
        wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: ie.gtk:");
        for(i=0; i< os_strlen((const char*)(ie.gtk)); i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", *(ie.gtk + i));
        wpa_printf_dbg(MSG_DEBUG, "\r\n");
    }

	if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) { //no enter
		wpa_printf_dbg(MSG_WARNING, "[3_4]WPA: GTK IE in unencrypted key data \r\n");
		goto failed;
	}

	if (wpa_supplicant_validate_ie(sm, (unsigned char *) sm->bssid, &ie) < 0)
    {
        wpa_printf_dbg(MSG_WARNING, "[3_4]WPA: Validate IE fail \r\n");
		goto failed;
    }

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: sm->anonce:");
    for(i=0; i<WPA_NONCE_LEN; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", sm->anonce[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: key->key_nonce:");
    for(i=0; i<WPA_NONCE_LEN; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", key->key_nonce[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

	if (os_memcmp(sm->anonce, key->key_nonce, WPA_NONCE_LEN) != 0) {
		wpa_printf_dbg(MSG_WARNING, "[3_4]WPA: ANonce from message 1 of 4-Way "
			   "Handshake differs from 3 of 4-Way Handshake - drop"
			   " packet (src=" MACSTR ") \r\n", MAC2STR(sm->bssid));
		goto failed;
	}

	keylen = WPA_GET_BE16(key->key_length);

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: keylen:%d pairwise_cipher:%d \r\n", keylen, sm->pairwise_cipher);

	switch (sm->pairwise_cipher) {
	case WPA_CIPHER_CCMP:
		if (keylen != 16) {
			wpa_printf_dbg(MSG_WARNING, "[3_4]WPA: Invalid CCMP key length %d (src=" MACSTR ") \r\n", keylen, MAC2STR(sm->bssid));
			goto failed;
		}
		break;
	case WPA_CIPHER_TKIP:
		if (keylen != 32) {
			wpa_printf_dbg(MSG_WARNING, "[3_4]WPA: WPA: Invalid TKIP key length %d (src=" MACSTR ") \r\n",
				   keylen, MAC2STR(sm->bssid));
			goto failed;
		}
		break;
	}

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: key_info:%x \r\n", key_info);

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: sm->ptk.kck:");
    for(i=0; i<16; i++) wpa_printf_dbg(MSG_WARNING, "%02x ", sm->ptk.kck[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: sm->ptk.kek:");
    for(i=0; i<16; i++) wpa_printf_dbg(MSG_WARNING, "%02x ", sm->ptk.kek[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: sm->ptk.tk1:");
    for(i=0; i<16; i++) wpa_printf_dbg(MSG_WARNING, "%02x ", sm->ptk.tk1[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

	/* SNonce was successfully used in msg 3/4, so mark it to be renewed
	 * for the next 4-Way Handshake. If msg 3 is received again, the old
	 * SNonce will still be used to avoid changing PTK. */
	sm->renew_snonce = 1;

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: key_info:%x \r\n", key_info);

	if (key_info & WPA_KEY_INFO_INSTALL) {
        wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: call wpa_supplicant_install_ptk \r\n");
		if (wpa_supplicant_install_ptk(sm, key))
			goto failed;
	}

    if (wpa_supplicant_send_4_of_4(sm, sm->bssid, key, ver, key_info, NULL, 0, &sm->ptk)) {
        wpa_printf_dbg(MSG_WARNING, "[3_4]WPA: wpa_supplicant_send_4_of_4 return fail \r\n");
        goto failed;
    }

#if 0
	if (key_info & WPA_KEY_INFO_SECURE) {
		wpa_sm_mlme_setprotection(
			sm, sm->bssid, MLME_SETPROTECTION_PROTECT_TYPE_RX,
			MLME_SETPROTECTION_KEY_TYPE_PAIRWISE);
		eapol_sm_notify_portValid(sm->eapol, TRUE);
	}
#endif

    //Group Key for WPA PSK, TBD here
#if 1
	wpa_sm_set_state(sm, WPA_GROUP_HANDSHAKE);

    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: key_info:%x \r\n", key_info);
    wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: ie.gtk_len:%d ie.gtk:", ie.gtk_len);
    for(i=0; i<ie.gtk_len; i++) wpa_printf_dbg(MSG_WARNING, "%02x ", *(ie.gtk + i));
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

	if (ie.gtk && wpa_supplicant_pairwise_gtk(sm, key, ie.gtk, ie.gtk_len, key_info) < 0)
    {
		wpa_printf_dbg(MSG_DEBUG, "[3_4]WPA: RSN: Failed to configure GTK \r\n");
		goto failed;
	}
#endif

    wpa_clr_key_info();

    //wpa_printf_dbg(MSG_INFO, "\r\nsecured connected\r\n\r\n");
    //msg_print(LOG_HIGH_LEVEL, "\r\nsecured connected\r\n\r\n");
    wifi_sta_join_complete(1); // 1 means success

#ifdef __WIFI_AUTO_CONNECT__
    /* Set successfully connect info to Auto Connect list */
    switch(get_auto_connect_mode()) {
        case AUTO_CONNECT_ENABLE:
            break;
        case AUTO_CONNECT_DISABLE:
            add_auto_connect_list();
            break;
        case AUTO_CONNECT_MANUAL:
            add_auto_connect_list();
            //compatible auto/manual connect
            set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            break;
        default:
            break;
    }
#endif

#if 0
	if (ieee80211w_set_keys(sm, &ie) < 0) {
		wpa_printf_dbg(MSG_DEBUG, "RSN: Failed to configure IGTK");
		goto failed;
	}
#endif

	return;

failed:
	//wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
	//need to trigger a deauthenticate here

    return;
}

/**
 * wpa_supplicant_send_4_of_4 - Send message 4 of WPA/RSN 4-Way Handshake
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @dst: Destination address for the frame
 * @key: Pointer to the EAPOL-Key frame header
 * @ver: Version bits from EAPOL-Key Key Info
 * @key_info: Key Info
 * @kde: KDEs to include the EAPOL-Key frame
 * @kde_len: Length of KDEs
 * @ptk: PTK to use for keyed hash and encryption
 * Returns: 0 on success, -1 on failure
 */
int wpa_supplicant_send_4_of_4_patch(struct wpa_sm *sm, const unsigned char *dst,
			       const struct wpa_eapol_key *key,
			       u16 ver, u16 key_info,
			       const u8 *kde, size_t kde_len,
			       struct wpa_ptk *ptk)
{
	size_t rlen;
	struct wpa_eapol_key *reply;
	u8 *rbuf;
    int i;
    int len_total;
    int ret;

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: ver:%x key_info:%x key->type:%d key->key_info:%x %x \r\n",
                          ver,
                          key_info,
                          key->type,
                          key->key_info[0],
                          key->key_info[1]);

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: key->key_length:%02x%02x key->key_data_length:%02x%02x \r\n",
                          key->key_length[0],
                          key->key_length[1],
                          key->key_data_length[0],
                          key->key_data_length[1]);

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: kde:");
    for(i=0; i< kde_len; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", *(kde + i));
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: sm->ptk.kck:");
    for(i=0; i< 16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", sm->ptk.kck[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: ptk->kck:");
    for(i=0; i< 16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", ptk->kck[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: sm->snonce:");
    for(i=0; i< WPA_NONCE_LEN; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", sm->snonce[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: key->key_mic:");
    for(i=0; i< 16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", key->key_mic[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

#if 0
	if (kde)
		wpa_hexdump(MSG_DEBUG, "WPA: KDE for msg 4/4", kde, kde_len);
#endif

    if (sm->proto == WPA_PROTO_RSN)
    {
        len_total = sizeof(*reply);
    }
    else
    {
        len_total = sizeof(*reply);
    }
#if 0
	rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
				  len_total,
				  &rlen, (void *) &reply);
#endif
    rbuf = wpa_alloc_eapol(ver, IEEE802_1X_TYPE_EAPOL_KEY,
			  NULL, len_total,
			  &rlen, (void *) &reply);

	if (rbuf == NULL)
		return -1;

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: len_total:%d rlen:%d sm->proto:%d \r\n", len_total, rlen, sm->proto); //len_total:95 rlen:99

	reply->type = sm->proto == WPA_PROTO_RSN ? EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;

	key_info &= WPA_KEY_INFO_SECURE;
	key_info |= ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC;
	WPA_PUT_BE16(reply->key_info, key_info);

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: key_info:%x reply->key_info:%x %x \r\n",
                          key_info,
                          reply->key_info[0],
                          reply->key_info[1]);

#if 0
	if (sm->proto == WPA_PROTO_RSN)
    {
		WPA_PUT_BE16(reply->key_length, 0);
	}else
	{
	    //os_memcpy(reply->key_length, key->key_length, 2);
        WPA_PUT_BE16(reply->key_length, 0);
    }
#else
    WPA_PUT_BE16(reply->key_length, 0);
#endif

    //os_memcpy(reply->key_nonce, sm->snonce, WPA_NONCE_LEN);

	os_memcpy(reply->replay_counter, key->replay_counter, WPA_REPLAY_COUNTER_LEN); //ok

	//WPA_PUT_BE16(reply->key_data_length, kde_len);
	//reply->key_data_length = 0;
    //os_memcpy(reply->key_length, key->key_length, sizeof(key->key_length));
    os_memset(reply->key_data_length, 0, sizeof(reply->key_data_length));

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: reply->key_length:%02x%02x reply->key_data_length:%02x%02x \r\n", reply->key_length[0], reply->key_length[1], reply->key_data_length[0], reply->key_data_length[1]);

#if 0
	if (kde)
		os_memcpy(reply + 1, kde, kde_len);
#endif

    ret = wpa_eapol_key_mic(ptk->kck, ver, rbuf, rlen, reply->key_mic);
    // Prevent compiler warnings
    (void)ret;

    wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: reply->key_mic:");
    for(i=0; i<16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", reply->key_mic[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

	wpa_printf_dbg(MSG_DEBUG, "[4_4]WPA: Sending EAPOL-Key 4/4 \r\n");

	wpa_eapol_key_send(sm, ptk->kck, ver, dst, ETH_P_EAPOL, rbuf, rlen, reply->key_mic);

	return 0;
}

/**
 * wpa_ether_send - Send Ethernet frame
 * @dest: Destination MAC address
 * @proto: Ethertype in host byte order
 * @buf: Frame payload starting from IEEE 802.1X header
 * @len: Frame payload length
 * Returns: >=0 on success, <0 on failure
 */
int wpa_ether_send_patch(const u8 *dest, u16 proto, u8 *buf, size_t len)
{
    //msg_print(LOG_HIGH_LEVEL, "[KEY]WPA: send eapol key frame, proto:%d len:%d \r\n", proto, len);
	return wpa_drv_send_eapol(dest, proto, buf, len);
}

void wpa_eapol_key_dump_patch(const struct wpa_eapol_key *key)
{
    if (key == NULL) return;

#ifndef CONFIG_NO_STDOUT_DEBUG
	//u16 key_info = WPA_GET_BE16(key->key_info);

	//wpa_printf_dbg(MSG_DEBUG, "\r\n[DUMP] EAPOL-Key type=%d \r\n", key->type);//  2
#if 0
	wpa_printf_dbg(MSG_DEBUG, "[DUMP] key_info 0x%x (ver=%d keyidx=%d rsvd=%d %s%s%s%s%s%s%s%s) \r\n",
		   key_info, key_info & WPA_KEY_INFO_TYPE_MASK,
		   (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
		   WPA_KEY_INFO_KEY_INDEX_SHIFT,
		   (key_info & (BIT(13) | BIT(14) | BIT(15))) >> 13,
		   key_info & WPA_KEY_INFO_KEY_TYPE ? "Pairwise" : "Group",
		   key_info & WPA_KEY_INFO_INSTALL ? " Install" : "",
		   key_info & WPA_KEY_INFO_ACK ? " Ack" : "",
		   key_info & WPA_KEY_INFO_MIC ? " MIC" : "",
		   key_info & WPA_KEY_INFO_SECURE ? " Secure" : "",
		   key_info & WPA_KEY_INFO_ERROR ? " Error" : "",
		   key_info & WPA_KEY_INFO_REQUEST ? " Request" : "",
		   key_info & WPA_KEY_INFO_ENCR_KEY_DATA ? " Encr" : "");  //key_info:0x8a=138 ver:2 keyidx:0 reserved:0 "Pairwise Ack"
#endif

    //wpa_printf_dbg(MSG_DEBUG, "[DUMP] key_info 0x%x%x \r\n", key->key_info[0], key->key_info[1]);
	//wpa_printf(MSG_DEBUG,  "[DUMP] key_length=%u key_data_length=%u \r\n", WPA_GET_BE16(key->key_length), WPA_GET_BE16(key->key_data_length));  //key_length:16  key_data_length:22
	//wpa_hexdump(MSG_DEBUG, "[DUMP] replay_counter", key->replay_counter, WPA_REPLAY_COUNTER_LEN);  //replay_counter: 00 00 00 00 00 00 00 00
    //wpa_hexdump(MSG_DEBUG, "[DUMP] key_nonce", key->key_nonce, WPA_NONCE_LEN); //key_nonce: 59 b5 5e d7 b5 ad 96 8d 6a 4f 6d 67 47 eb 5c 59 a1 41 ae f2 80 0e 0f ca ad 47 b0 99 da 80 ef 1a
    //wpa_hexdump(MSG_DEBUG, "[DUMP] key_iv", key->key_iv, 16);                  //key_iv: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    //wpa_hexdump(MSG_DEBUG, "[DUMP] key_rsc", key->key_rsc, 8);                 //key_rsc: 00 00 00 00 00 00 00 00
    //wpa_hexdump(MSG_DEBUG, "[DUMP] key_id (reserved)", key->key_id, 8);        //key_id: 00 00 00 00 00 00 00 00
    //wpa_hexdump(MSG_DEBUG, "[DUMP] key_mic", key->key_mic, 16);                //key_mic: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    //wpa_printf_dbg(MSG_DEBUG, "\r\n");
#endif /* CONFIG_NO_STDOUT_DEBUG */
}

int wpa_sm_set_assoc_wpa_ie_patch(struct wpa_sm *sm, const u8 *ie, size_t len)
{
    //wpa_printf(MSG_DEBUG, "[IE]WPA: Set WPA IE \r\n");

	if (sm == NULL)
		return -1;

	os_free(sm->assoc_wpa_ie);
	if (ie == NULL || len == 0) {
		//wpa_printf(MSG_DEBUG, "[IE]WPA: clearing own WPA/RSN IE \r\n");
		sm->assoc_wpa_ie = NULL;
		sm->assoc_wpa_ie_len = 0;
	} else {
		//wpa_hexdump(MSG_DEBUG, "[IE]WPA: set own WPA/RSN IE \r\n", ie, len);
		sm->assoc_wpa_ie = os_malloc(len);
		if (sm->assoc_wpa_ie == NULL)
			return -1;
		os_memcpy(sm->assoc_wpa_ie, ie, len);
		sm->assoc_wpa_ie_len = len;
	}

	return 0;
}

int wpa_sm_set_ap_rsn_ie_patch(struct wpa_sm *sm, const u8 *ie, size_t len)
{
    //wpa_printf(MSG_DEBUG, "[IE]WPA: Set AP RSN IE \r\n");

	if (sm == NULL)
		return -1;

	os_free(sm->ap_rsn_ie);
	if (ie == NULL || len == 0) {
		//wpa_printf(MSG_DEBUG, "[IE]WPA: clearing AP RSN IE \r\n");
		sm->ap_rsn_ie = NULL;
		sm->ap_rsn_ie_len = 0;
	} else {
		//wpa_hexdump(MSG_DEBUG, "[IE]WPA: set AP RSN IE \r\n", ie, len);
		sm->ap_rsn_ie = os_malloc(len);
		if (sm->ap_rsn_ie == NULL)
			return -1;

		os_memcpy(sm->ap_rsn_ie, ie, len);
		sm->ap_rsn_ie_len = len;
	}

	return 0;
}

int wpa_sm_set_ap_wpa_ie_patch(struct wpa_sm *sm, const u8 *ie, size_t len)
{
    wpa_printf_dbg(MSG_DEBUG, "[IE]WPA: set AP WPA IE \r\n");

	if (sm == NULL)
		return -1;

	os_free(sm->ap_wpa_ie);
	if (ie == NULL || len == 0) {
		wpa_printf_dbg(MSG_DEBUG, "[IE]WPA: clearing AP WPA IE \r\n");
		sm->ap_wpa_ie = NULL;
		sm->ap_wpa_ie_len = 0;
	} else {
		wpa_hexdump(MSG_DEBUG, "[IE]WPA: set AP WPA IE \r\n", ie, len);
		sm->ap_wpa_ie = os_malloc(len);
		if (sm->ap_wpa_ie == NULL)
			return -1;

		os_memcpy(sm->ap_wpa_ie, ie, len);
		sm->ap_wpa_ie_len = len;
	}

	return 0;
}

int wpa_supplicant_verify_eapol_key_mic_patch(struct wpa_sm *sm,
					       struct wpa_eapol_key *key,
					       u16 ver,
					       const u8 *buf, size_t len)
{  //message 3 => enter
	u8 mic[16];
	int ok = 0;
    //int i;

#if 0
    wpa_printf(MSG_DEBUG, "[KEY][MIC]WPA: ver:%d len:%ld sm->tptk_set:%d \r\n", ver, len, sm->tptk_set);
    wpa_printf(MSG_DEBUG, "[KEY][MIC]WPA: key->key_mic:");
    for(i=0; i<16; i++) wpa_printf(MSG_DEBUG, "%02x ", key->key_mic[i]);
    wpa_printf(MSG_DEBUG, "\r\n");
#endif

	os_memcpy(mic, key->key_mic, 16);

	if (sm->tptk_set) {
		os_memset(key->key_mic, 0, 16);

#if 0
        wpa_printf(MSG_DEBUG, "[KEY][MIC]WPA: sm->tptk.kck:");
        for(i=0;i<16;i++) wpa_printf(MSG_DEBUG, "%02x ", sm->tptk.kck[i]);
        wpa_printf(MSG_DEBUG, "\r\n");

        wpa_printf(MSG_DEBUG, "[KEY][MIC]WPA: buf:");
        for(i=0;i<len;i++) wpa_printf(MSG_DEBUG, "%02x ", *(buf + i));
        wpa_printf(MSG_DEBUG, "\r\n");
#endif
		wpa_eapol_key_mic(sm->tptk.kck, ver, buf, len, key->key_mic);

		if (os_memcmp(mic, key->key_mic, 16) != 0) {
            wpa_printf_dbg(MSG_DEBUG, "[KEY][MIC]WPA: MIC compare fail \r\n");
			//wpa_printf(MSG_WARNING, "[KEY][MIC]WPA: Invalid EAPOL-Key MIC when using TPTK - ignoring TPTK \r\n");
		} else {
		    //wpa_printf(MSG_DEBUG, "[KEY][MIC]WPA: MIC compare ok \r\n");
			ok = 1;
			sm->tptk_set = 0;
			sm->ptk_set = 1;
			os_memcpy(&sm->ptk, &sm->tptk, sizeof(sm->ptk));
		}
	}

    //wpa_printf(MSG_DEBUG, "[KEY][MIC]WPA: ok:%d sm->ptk_set:%d \r\n", ok, sm->ptk_set);

	if (!ok && sm->ptk_set) {
		os_memset(key->key_mic, 0, 16);

#if 0
        wpa_printf(MSG_DEBUG, "[KEY][MIC]WPA: sm->ptk.kck:");
        for(i=0;i<16;i++) wpa_printf(MSG_DEBUG, "%02x ", sm->ptk.kck[i]);
        wpa_printf(MSG_DEBUG, "\r\n");
#endif

		wpa_eapol_key_mic(sm->ptk.kck, ver, buf, len, key->key_mic);

		if (os_memcmp(mic, key->key_mic, 16) != 0) {
			wpa_printf_dbg(MSG_WARNING, "[KEY][MIC]WPA: Invalid EAPOL-Key MIC - dropping packet \r\n");
			return -1;
		}
		ok = 1;
	}

	if (!ok) {
		wpa_printf_dbg(MSG_WARNING, "[KEY][MIC]WPA: Could not verify EAPOL-Key MIC - dropping packet \r\n");
		return -1;
	}

	os_memcpy(sm->rx_replay_counter, key->replay_counter, WPA_REPLAY_COUNTER_LEN);
	sm->rx_replay_counter_set = 1;

	return 0;
}

int wpa_sm_rx_eapol_patch(struct wpa_sm *sm, const u8 *src_addr,
		    const u8 *buf, size_t len)
{
	size_t plen, data_len, extra_len;
	struct ieee802_1x_hdr *hdr;
	struct wpa_eapol_key *key;
	u16 key_info, ver;
	u8 *tmp;
	int ret = -1;
	struct wpa_peerkey *peerkey = NULL;

    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: len:%d \r\n", len);

	if (len < sizeof(*hdr) + sizeof(*key)) { //no enter
		wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: EAPOL frame too short to be a WPA EAPOL-Key (len %lu, expecting at least %lu) \r\n", (unsigned long) len, (unsigned long) sizeof(*hdr) + sizeof(*key));
		return 0;
	}

	tmp = os_malloc(len);
	if (tmp == NULL) //no enter
		return -1;
	os_memcpy(tmp, buf, len);

	hdr = (struct ieee802_1x_hdr *) tmp;
	key = (struct wpa_eapol_key *) (hdr + 1);
	plen = be_to_host16(hdr->length);
	data_len = plen + sizeof(*hdr);
	//wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: IEEE 802.1X RX: version=%d type=%d length=%lu \r\n", hdr->version, hdr->type, (unsigned long) plen); //hdr->version:1 hdr->type:3 length:117

    os_memset(&msg_1_4_hdr, 0, sizeof(msg_1_4_hdr));
    msg_1_4_hdr.version = hdr->version;
    msg_1_4_hdr.type = hdr->type;
    msg_1_4_hdr.length = hdr->length;
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: put ieee802_1x_hdr to msg_1_4_hdr \r\n");
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: msg_1_4_hdr.version:%d \r\n", msg_1_4_hdr.version);
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: msg_1_4_hdr.type:%d \r\n", msg_1_4_hdr.type);
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: msg_1_4_hdr.length:%d \r\n", msg_1_4_hdr.length);

	if (hdr->version < EAPOL_VERSION) { //enter this, but do nothing
		/* TODO: backwards compatibility */
	}

	if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY) { //no enter
		wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: EAPOL frame (type %u) discarded, not a Key frame \r\n", hdr->type);
		ret = 0;
		goto out;
	}

    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: plen:%ld len:%d sizeof(*hdr):%ld sizeof(*key):%ld \r\n", plen, len, sizeof(*hdr), sizeof(*key));

#if 0
	if (plen > len - sizeof(*hdr) || plen < sizeof(*key)) { //no enter
		wpa_printf(MSG_DEBUG, "\r\n wpa_sm_rx_eapol, WPA: EAPOL frame payload size %lu "
			   "invalid (frame size %lu)",
			   (unsigned long) plen, (unsigned long) len);
		ret = 0;
		goto out;
	}
#endif

    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: EAPOL-Key type (%d) ", key->type); //WPA2 PSK =>key->type:2

	if (key->type != EAPOL_KEY_TYPE_WPA && key->type != EAPOL_KEY_TYPE_RSN) //no enter
	{
		wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: EAPOL-Key type (%d) unknown, discarded", key->type);
		ret = 0;
		goto out;
	}

	wpa_eapol_key_dump(key);

	//eapol_sm_notify_lower_layer_success(sm->eapol, 0);
	wpa_hexdump(MSG_MSGDUMP, "[KEY]WPA: RX EAPOL-Key", tmp, len);

    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: len:%d data_len:%d ", len, data_len); //len:125   data_len:121
	if (data_len < len) { //enter
		wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ignoring %lu bytes after the IEEE 802.1X data ", (unsigned long) len - data_len); //4
	}
	key_info = WPA_GET_BE16(key->key_info); //message 1 => key_info: 0x8a==138
	                                        //message 2 => key_info:0x13ca

    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: key_info:%x g_key_info:%x \r\n", key_info, g_key_info);
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: frame_len:%x g_frame_len:%x \r\n", len, g_frame_len);

#if 0
    //Set key_info to global variable g_key_info for next EAPOL-Key Frame checking, it the same with it, it means it's the duplicate frame, can ignore it
    if(key_info == g_key_info ||
       key_info == g_key_info_1_4 ||
       key_info == g_key_info_3_4 )
    { // Skip this frame
        wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: key_info == g_key_info, Skip this EAPOL-Key Frame \r\n");
        ret = 0;
        goto out;
    }

    //Set len to global variable g_frame_len for next EAPOL-Key Frame checking, it the same with it, it means it's the duplicate frame, can ignore it
    if(len == g_frame_len)
    { // Skip this frame
        wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: len == g_frame_len, Skip this EAPOL-Key Frame \r\n");
        ret = 0;
        goto out;
    }
#endif

    g_key_info = key_info;
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: set g_key_info=%x \r\n", g_key_info);

    g_frame_len = len;
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: set g_frame_len=%x \r\n", g_frame_len);


	ver = key_info & WPA_KEY_INFO_TYPE_MASK;
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ver:%d \r\n", ver);   // 2
	if (ver != WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 &&
	    ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {  //no enter
		wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: Unsupported EAPOL-Key descriptor version %d. \r\n", ver);
		goto out;
	}

    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: pairwise_cipher:%d group_cipher:%d \r\n", sm->pairwise_cipher, sm->group_cipher);

	if (sm->pairwise_cipher == WPA_CIPHER_CCMP &&
	    ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) { //no enter
		//wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: CCMP is used, but EAPOL-Key descriptor version (%d) is not 2. \r\n", ver);
		if (sm->group_cipher != WPA_CIPHER_CCMP &&
		    !(key_info & WPA_KEY_INFO_KEY_TYPE)) {
			/* Earlier versions of IEEE 802.11i did not explicitly
			 * require version 2 descriptor for all EAPOL-Key
			 * packets, so allow group keys to use version 1 if
			 * CCMP is not used for them. */
			wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: Backwards compatibility: allow invalid version for non-CCMP group keys \r\n");
		} else
			goto out;
	}

#if 0
#ifdef CONFIG_PEERKEY
	for (peerkey = sm->peerkey; peerkey; peerkey = peerkey->next) {
		if (os_memcmp(peerkey->addr, src_addr, ETH_ALEN) == 0)
			break;
	}

	if (!(key_info & WPA_KEY_INFO_SMK_MESSAGE) && peerkey) {
		if (!peerkey->initiator && peerkey->replay_counter_set &&
		    os_memcmp(key->replay_counter, peerkey->replay_counter,
			      WPA_REPLAY_COUNTER_LEN) <= 0) {
			wpa_printf(MSG_WARNING, "RSN: EAPOL-Key Replay "
				   "Counter did not increase (STK) - dropping "
				   "packet");
			goto out;
		} else if (peerkey->initiator) {
			u8 _tmp[WPA_REPLAY_COUNTER_LEN];
			os_memcpy(_tmp, key->replay_counter,
				  WPA_REPLAY_COUNTER_LEN);
			inc_byte_array(_tmp, WPA_REPLAY_COUNTER_LEN);
			if (os_memcmp(_tmp, peerkey->replay_counter,
				      WPA_REPLAY_COUNTER_LEN) != 0) {
				wpa_printf(MSG_DEBUG, "RSN: EAPOL-Key Replay "
					   "Counter did not match (STK) - "
					   "dropping packet");
				goto out;
			}
		}
	}

	if (peerkey && peerkey->initiator && (key_info & WPA_KEY_INFO_ACK)) {
		wpa_printf(MSG_DEBUG, "RSN: Ack bit in key_info from STK peer");
		goto out;
	}
#endif /* CONFIG_PEERKEY */
#endif

#if 0
    wpa_printf_dbg(MSG_WARNING, "[KEY]WPA: sm->rx_replay_counter_set:%d \r\n", sm->rx_replay_counter_set); //sm->rx_replay_counter_set:0
    wpa_printf_dbg(MSG_WARNING, "[KEY]WPA: key->replay_counter[0~7]: %x %x %x %x %x %x %x %x \r\n", //key->replay_counter[0~7]: 0 0 0 0 0 0 0 1
                            key->replay_counter[0],
                            key->replay_counter[1],
                            key->replay_counter[2],
                            key->replay_counter[3],
                            key->replay_counter[4],
                            key->replay_counter[5],
                            key->replay_counter[6],
                            key->replay_counter[7]);
    wpa_printf_dbg(MSG_WARNING, "[KEY]WPA: sm->rx_replay_counter[0~7]: %x %x %x %x %x %x %x %x \r\n", //sm->rx_replay_counter[0~7]: 0 0 0 0 0 0 0 0
                            sm->rx_replay_counter[0],
                            sm->rx_replay_counter[1],
                            sm->rx_replay_counter[2],
                            sm->rx_replay_counter[3],
                            sm->rx_replay_counter[4],
                            sm->rx_replay_counter[5],
                            sm->rx_replay_counter[6],
                            sm->rx_replay_counter[7]);
#endif


#if 0
	if (!peerkey && sm->rx_replay_counter_set &&
	    os_memcmp(key->replay_counter, sm->rx_replay_counter,
		      WPA_REPLAY_COUNTER_LEN) <= 0) { //no enter
		wpa_printf(MSG_WARNING, "\r\n wpa_sm_rx_eapol, WPA: EAPOL-Key Replay Counter did not"
			   " increase - dropping packet ");
		goto out;
	}
#else
//need to check here in the future
#if 0
	if (os_memcmp(key->replay_counter, sm->rx_replay_counter, WPA_REPLAY_COUNTER_LEN) <= 0)
    {
		wpa_printf(MSG_WARNING, "\r\n wpa_sm_rx_eapol, WPA: EAPOL-Key Replay Counter did not"
			   " increase - dropping packet");
		goto out;
	}
#endif
#endif


#if 0
	if (!(key_info & (WPA_KEY_INFO_ACK | WPA_KEY_INFO_SMK_MESSAGE))
#if 0
#ifdef CONFIG_PEERKEY
	    && (peerkey == NULL || !peerkey->initiator)
#endif /* CONFIG_PEERKEY */
#endif
	) { //no enter
		wpa_printf(MSG_DEBUG, "\r\n wpa_sm_rx_eapol, WPA: No Ack bit in key_info ");
		goto out;
	}
#endif

	if (key_info & WPA_KEY_INFO_REQUEST) { //no enter
		wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: EAPOL-Key with Request bit - dropped \r\n");
		goto out;
	}

#if 0
	if ((key_info & WPA_KEY_INFO_MIC) && !peerkey &&
	    wpa_supplicant_verify_eapol_key_mic(sm, key, ver, tmp, data_len)) //no enter
		goto out;
#else
    if ((key_info & WPA_KEY_INFO_MIC) && wpa_supplicant_verify_eapol_key_mic(sm, key, ver, tmp, data_len)) //message 1 => no enter
    {                                                                                                      //message 2 => enter, fail
        wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: key mic verify fail \r\n");
		goto out;
    }
#endif

#if 0
#ifdef CONFIG_PEERKEY
	if ((key_info & WPA_KEY_INFO_MIC) && peerkey &&
	    peerkey_verify_eapol_key_mic(sm, peerkey, key, ver, tmp, data_len))
		goto out;
#endif /* CONFIG_PEERKEY */
#endif

	extra_len = data_len - sizeof(*hdr) - sizeof(*key);

#if 0
    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: data_len:%d sizeof(*hdr):%d sizeof(*key):%d extra_len:%d \r\n",
                         data_len,
                         sizeof(*hdr),
                         sizeof(*key),
                         extra_len); //data_len:121 sizeof(*hdr):4 sizeof(*key):95 extra_len:22
#endif

	if (WPA_GET_BE16(key->key_data_length) > extra_len) { //no enter
		wpa_printf_dbg( MSG_DEBUG, "[KEY]WPA: Invalid EAPOL-Key frame - key_data overflow (%d > %lu) \r\n", WPA_GET_BE16(key->key_data_length), (unsigned long) extra_len);
		goto out;
	}
	extra_len = WPA_GET_BE16(key->key_data_length);

	if (sm->proto == WPA_PROTO_RSN &&
	    (key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) { //no enter
	    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: wpa_s->proto == WPA_PROTO_RSN \r\n");
		if (wpa_supplicant_decrypt_key_data(sm, key, ver))
			goto out;
		extra_len = WPA_GET_BE16(key->key_data_length);
	}

    wpa_printf_dbg(MSG_WARNING, "[KEY]WPA: key_info:%x \r\n", key_info)

	if (key_info & WPA_KEY_INFO_KEY_TYPE) { //enter
		if (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) { //no enter
			//wpa_printf_dbg(MSG_WARNING, "[KEY]WPA: Ignored EAPOL-Key (Pairwise) with non-zero key index \r\n");
			goto out;
		}
		if (peerkey) { //no enter
            //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: Do peerkey_rx_eapol_4way \r\n");
			/* PeerKey 4-Way Handshake */
			peerkey_rx_eapol_4way(sm, peerkey, key, key_info, ver);
		} else if (key_info & WPA_KEY_INFO_MIC) { //no enter
		    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: do wpa_supplicant_process_3_of_4 \r\n");
            msg_print(LOG_HIGH_LEVEL, "[WIFI]Receive EAPOL frame : size=%d \r\n", len);
            
            //Save key_info of message 3/4
            g_key_info_3_4 = key_info;

			/* 3/4 4-Way Handshake */
			wpa_supplicant_process_3_of_4(sm, key, ver);
		} else { //enter
    		//wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: do wpa_supplicant_process_1_of_4 \r\n");
            msg_print(LOG_HIGH_LEVEL, "[WIFI]Receive EAPOL frame : size=%d \r\n", len);
            //Save key_info of message 1/4
            g_key_info_1_4 = key_info;

			/* 1/4 4-Way Handshake */
			wpa_supplicant_process_1_of_4(sm, src_addr, key, ver);
		}
	} else if (key_info & WPA_KEY_INFO_SMK_MESSAGE) {
		/* PeerKey SMK Handshake */
		peerkey_rx_eapol_smk(sm, src_addr, key, extra_len, key_info, ver);
	} else {
		if (key_info & WPA_KEY_INFO_MIC) {
            //wpa_printf_dbg(MSG_WARNING, "[KEY]WPA: call wpa_supplicant_process_1_of_2 \r\n");
            msg_print(LOG_HIGH_LEVEL, "[WIFI]Receive EAPOL frame : size=%d \r\n", len);
			/* 1/2 Group Key Handshake */
			wpa_supplicant_process_1_of_2(sm, src_addr, key, extra_len, ver);
		} else {
			//wpa_printf_dbg(MSG_WARNING, "[KEY]WPA: EAPOL-Key (Group) without Mic bit - dropped \r\n");
		}
	}

	ret = 1;
out:
	os_free(tmp);
	return ret;
}

u8 * wpa_alloc_eapol_patch(u8 ver, u8 type, void *data, u16 data_len, size_t *msg_len, void **data_pos)
{
	struct ieee802_1x_hdr *hdr;

	*msg_len = sizeof(*hdr) + data_len;

    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: malloc eapol key, ver:%d type:%d data_len:%d msg_len:%d \r\n", ver, type, data_len, *msg_len);

	hdr = os_malloc(*msg_len);
	if (hdr == NULL)
		return NULL;

	hdr->version = ver;
	hdr->type = type;
	hdr->length = host_to_be16(data_len);

	if (data)
		os_memcpy(hdr + 1, data, data_len);
	else
		os_memset(hdr + 1, 0, data_len);

	if (data_pos)
		*data_pos = hdr + 1;

	return (u8 *) hdr;
}

void wpa_eapol_key_send_patch(struct wpa_sm *sm, const u8 *kck,
			int ver, const u8 *dest, u16 proto,
			u8 *msg, size_t msg_len, u8 *key_mic)
{   //msg: 121 bytes EAPOL-Key Frame
    //msg_len:121

//    int i;
//    int ret;

    //wpa_printf(MSG_DEBUG, "\r\n WPA: wpa_eapol_key_send, ver:%d ", ver); //
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: kck:");
    //for(i=0; i<16; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", kck[i]);
    //wpa_printf_dbg(MSG_DEBUG, "\r\n");

#if 0
	if (is_zero_ether_addr(dest) && is_zero_ether_addr(sm->bssid)) {
		/*
		 * Association event was not yet received; try to fetch
		 * BSSID from the driver.
		 */
		if (wpa_sm_get_bssid(sm, sm->bssid) < 0) {
			wpa_printf(MSG_DEBUG, "\r\n wpa_eapol_key_send, WPA: Failed to read BSSID for "
				   "EAPOL-Key destination address \r\n");
		} else {
			dest = sm->bssid;
			wpa_printf(MSG_DEBUG, "\r\n wpa_eapol_key_send, WPA: Use BSSID (" MACSTR
				   ") as the destination for EAPOL-Key \r\n",
				   MAC2STR(dest));
		}
	}
	if (key_mic &&
	    wpa_eapol_key_mic(kck, ver, msg, msg_len, key_mic)) {
		wpa_printf(MSG_ERROR, "\r\n wpa_eapol_key_send, WPA: Failed to generate EAPOL-Key "
			   "version %d MIC \r\n", ver);
		goto out;
	}

    wpa_printf(MSG_DEBUG, "\r\n wpa_eapol_key_send, call wpa_eapol_key_mic \r\n");

    ret = wpa_eapol_key_mic(kck, ver, msg, msg_len, key_mic);

    wpa_printf(MSG_DEBUG, "\r\n wpa_eapol_key_send, after wpa_eapol_key_mic \r\n");

    wpa_printf(MSG_DEBUG, "\r\n wpa_eapol_key_send, key_mic[16]: "); //key_mic[16]: 8c 08 c7 da 06 31 ae 55 ac 41 fe f8 e8 82 ea 9e
    for(i=0;i<16;i++) wpa_printf(MSG_ERROR, "%02x ", key_mic[i]);
#endif


    //wpa_printf(MSG_DEBUG, "\r\n wpa_eapol_key_send, param1 dest: ");
    //for(i=0; i<6; i++) wpa_printf(MSG_DEBUG, "%02x ", *(dest + i));
    //wpa_printf(MSG_DEBUG, "\r\n wpa_eapol_key_send, param2 proto: %lx \r\n", proto);
	//wpa_hexdump(MSG_MSGDUMP, "\r\n wpa_eapol_key_send, WPA: TX EAPOL-Key ", msg, msg_len);
    //wpa_printf(MSG_DEBUG, "\r\n wpa_eapol_key_send, call wpa_ether_send \r\n");

    wpa_ether_send(dest, proto, msg, msg_len); //ok

	//eapol_sm_notify_tx_eapol_key(sm->eapol);
//out:
	os_free(msg);
}

int wpa_supplicant_validate_ie_patch(struct wpa_sm *sm,
				      unsigned char *src_addr,
				      struct wpa_eapol_ie_parse *ie)
{
    //int i;

    if (src_addr != NULL) {
#if 0
        wpa_printf_dbg(MSG_DEBUG, "[IE]WPA: src_addr:");
        for(i=0;i<6;i++) {
            wpa_printf_dbg(MSG_DEBUG, "%02x", *(src_addr + i));
        }
        wpa_printf_dbg(MSG_DEBUG, "\r\n");
#endif
    }

    if(ie != NULL){
#if 0
        wpa_printf_dbg(MSG_DEBUG, "[IE]WPA: ie->rsn_ie_len:%ld \r\n", ie->rsn_ie_len);
        if(ie->rsn_ie != NULL) {
            wpa_printf_dbg(MSG_DEBUG, "[IE]WPA: rsn_ie:");
            for(i=0; i<ie->rsn_ie_len; i++) {
                wpa_printf_dbg(MSG_DEBUG, "%02x", *(ie->rsn_ie + i) );
            }
            wpa_printf_dbg(MSG_DEBUG, "\r\n");
        }
#endif
    }

	if (sm->ap_wpa_ie == NULL && sm->ap_rsn_ie == NULL) {
#if 0
		wpa_printf_dbg(MSG_DEBUG, "[IE]WPA: No WPA/RSN IE for this AP known. Trying to get from scan results \r\n");
		if (wpa_sm_get_beacon_ie(sm) < 0) {
			wpa_printf_dbg(MSG_WARNING, "[IE]WPA: Could not find AP from the scan results \r\n");
		} else {
			wpa_printf_dbg(MSG_DEBUG, "[IE]WPA: Found the current AP from updated scan results \r\n");
		}
#endif
	}

	return 0;
}

int wpa_supplicant_install_ptk_patch(struct wpa_sm *sm,
				      const struct wpa_eapol_key *key)
{
    //int i;
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: pairwise_cipher:%d proto:%d \r\n", sm->pairwise_cipher, sm->proto);
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: Installing PTK to the driver \r\n");

    os_memcpy(g_ptk.kck, sm->ptk.kck, sizeof(g_ptk.kck));
    os_memcpy(g_ptk.kek, sm->ptk.kek, sizeof(g_ptk.kek));
    os_memcpy(g_ptk.tk1, sm->ptk.tk1, sizeof(g_ptk.tk1));
    os_memcpy(g_ptk.u.auth.tx_mic_key, sm->ptk.u.auth.tx_mic_key, sizeof(g_ptk.u.auth.tx_mic_key));
    os_memcpy(g_ptk.u.auth.rx_mic_key, sm->ptk.u.auth.rx_mic_key, sizeof(g_ptk.u.auth.rx_mic_key));

#if 0
    wpa_printf_dbg(MSG_DEBUG, "\r\n\r\n#################### Installed Keys ###############################\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: g_ptk.kck: ");
    for(i=0; i<sizeof(g_ptk.kck); i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", g_ptk.kck[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: g_ptk.kek: ");
    for(i=0; i<sizeof(g_ptk.kek); i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", g_ptk.kek[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: g_ptk.tk1: ");
    for(i=0; i<sizeof(g_ptk.tk1); i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", g_ptk.tk1[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: g_ptk.u.auth.tx_mic_key: ");
    for(i=0; i<sizeof(g_ptk.u.auth.tx_mic_key); i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", g_ptk.u.auth.tx_mic_key[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: g_ptk.u.auth.rx_mic_key: ");
    for(i=0; i<sizeof(g_ptk.u.auth.rx_mic_key); i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", g_ptk.u.auth.rx_mic_key[i]);
    wpa_printf_dbg(MSG_DEBUG, "\r\n");

    wpa_printf_dbg(MSG_DEBUG, "################################################################### \r\n\r\n");
#endif

	return 0;
}

int wpa_supplicant_install_gtk_patch(struct wpa_sm *sm,
				      const struct wpa_gtk_data *gd,
				      const u8 *key_rsc)
{
	const u8 *_gtk = gd->gtk;
	u8 gtk_buf[32];
    //int i;

#if 0
    wpa_printf(MSG_DEBUG, "[GTK]WPA: sm->group_cipher:%d sm->pairwise_cipher:%d \r\n", sm->group_cipher, sm->pairwise_cipher);

    wpa_printf(MSG_DEBUG, "[GTK]WPA: gd->gtk_len:%d gd->gtk:", gd->gtk_len);
    for(i=0; i<gd->gtk_len; i++) wpa_printf(MSG_DEBUG, "%02x ", gd->gtk[i]);
    wpa_printf(MSG_DEBUG, "\r\n");

	wpa_hexdump_key(MSG_DEBUG, "[GTK]WPA: Group Key", gd->gtk, gd->gtk_len);
	wpa_printf(MSG_DEBUG, "[GTK]WPA: Installing GTK to the driver (keyidx=%d tx=%d len=%d). \r\n", gd->keyidx, gd->tx, gd->gtk_len);
	wpa_hexdump(MSG_DEBUG, "[GTK]WPA: RSC", key_rsc, gd->key_rsc_len);
#endif

	if (sm->group_cipher == WPA_CIPHER_TKIP) {
        //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_install_gtk, group_cipher == WPA_CIPHER_TKIP \r\n");

		/* Swap Tx/Rx keys for Michael MIC */
		os_memcpy(gtk_buf, gd->gtk, 16);
		os_memcpy(gtk_buf + 16, gd->gtk + 24, 8);
		os_memcpy(gtk_buf + 24, gd->gtk + 16, 8);
		_gtk = gtk_buf;
	}

#if 0
	if (sm->pairwise_cipher == WPA_CIPHER_NONE) {
		if (wpa_sm_set_key(sm, gd->alg,
				   (u8 *) "\xff\xff\xff\xff\xff\xff",
				   gd->keyidx, 1, key_rsc, gd->key_rsc_len,
				   _gtk, gd->gtk_len) < 0) {
			wpa_printf(MSG_WARNING, "WPA: Failed to set "
				   "GTK to the driver (Group only).");
			return -1;
		}
	} else if (wpa_sm_set_key(sm, gd->alg,
				  (u8 *) "\xff\xff\xff\xff\xff\xff",
				  gd->keyidx, gd->tx, key_rsc, gd->key_rsc_len,
				  _gtk, gd->gtk_len) < 0) {
		wpa_printf(MSG_WARNING, "WPA: Failed to set GTK to "
			   "the driver (alg=%d keylen=%d keyidx=%d)",
			   gd->alg, gd->gtk_len, gd->keyidx);
		return -1;
	}
    return 0;
#else
    if (sm->pairwise_cipher == WPA_CIPHER_NONE) {
        //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_install_gtk, pairwise_cipher == WPA_CIPHER_NONE \r\n");
    } else {
        //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_install_gtk, pairwise_cipher != WPA_CIPHER_NONE \r\n");
        g_gtk.gtk_len = gd->gtk_len;
        memcpy(g_gtk.gtk, _gtk, gd->gtk_len);
        wpa_printf_dbg(MSG_DEBUG, "[GTK]WPA: install gtk \r\n");
    }
    //wpa_printf(MSG_DEBUG, "\r\n wpa_supplicant_install_gtk, g_gtk.gtk_len:%d g_gtk.gtk: ", g_gtk.gtk_len);
    //for (i=0; i<gd->gtk_len; i++) wpa_printf(MSG_DEBUG, "%02x ", g_gtk.gtk[i]);

    return 0;
#endif
}

/*
   Interface Initialization: WPA
 */
void wpa_func_init_patch(void)
{
    wpa_supplicant_process_1_of_4 = wpa_supplicant_process_1_of_4_patch;
    wpa_supplicant_send_2_of_4 = wpa_supplicant_send_2_of_4_patch;
    wpa_supplicant_process_3_of_4 = wpa_supplicant_process_3_of_4_patch;
    wpa_supplicant_send_4_of_4 = wpa_supplicant_send_4_of_4_patch;
    wpa_eapol_key_dump = wpa_eapol_key_dump_patch;
    wpa_sm_set_assoc_wpa_ie = wpa_sm_set_assoc_wpa_ie_patch;
    wpa_sm_set_ap_rsn_ie = wpa_sm_set_ap_rsn_ie_patch;
    wpa_sm_set_ap_wpa_ie = wpa_sm_set_ap_wpa_ie_patch;
    wpa_supplicant_verify_eapol_key_mic = wpa_supplicant_verify_eapol_key_mic_patch;
    wpa_sm_rx_eapol = wpa_sm_rx_eapol_patch;
    wpa_eapol_key_send = wpa_eapol_key_send_patch;
    wpa_supplicant_validate_ie = wpa_supplicant_validate_ie_patch;
    wpa_supplicant_install_ptk = wpa_supplicant_install_ptk_patch;
    wpa_supplicant_install_gtk = wpa_supplicant_install_gtk_patch;
    wpa_alloc_eapol = wpa_alloc_eapol_patch;
    wpa_ether_send = wpa_ether_send_patch;
    wpa_derive_ptk = wpa_derive_ptk_patch;

    return;
}

