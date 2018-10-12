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
#include "wifi_mac_tx_data_patch.h"

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
extern u8 g_fastconn;   
extern u8 g_wifi_reconnection_counter;

extern int g_DbgMode;

extern wpa_alloc_eapol_fp_t wpa_alloc_eapol;
extern wpa_derive_ptk_fp_t wpa_derive_ptk;

extern u8 g_wpa_psk[32];

void wpa_supplicant_process_3_of_4_patch(struct wpa_sm *sm,
					  const struct wpa_eapol_key *key,
					  u16 ver)
{
	u16 key_info, keylen, len;
	const u8 *pos;
	struct wpa_eapol_ie_parse ie;
    int i;
    hap_control_t *hap_temp;
    hap_temp=get_hap_control_struct();

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
    if (hap_temp->hap_en)
    {
        hiddenap_complete();
    }
    g_wifi_reconnection_counter = MAX_WIFI_RECONNECTION;
#endif

    wpa_clr_key_info();

    //wpa_printf_dbg(MSG_INFO, "\r\nsecured connected\r\n\r\n");
    //msg_print(LOG_HIGH_LEVEL, "\r\nsecured connected\r\n\r\n");
    g_fastconn = 0;
    wifi_sta_join_complete(1); // 1 means success
    send_port_security_done_event();
    wifi_mac_set_encrypt_eapol_frame(true);
    
#ifdef __WIFI_AUTO_CONNECT__
    /* Set successfully connect info to Auto Connect list */
    switch(get_auto_connect_mode()) {
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
            
            wifi_mac_set_encrypt_eapol_frame(false);
            
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

/*
   Interface Initialization: WPA
 */
void wpa_func_init_patch(void)
{
    wpa_supplicant_process_3_of_4 = wpa_supplicant_process_3_of_4_patch;
    wpa_sm_rx_eapol               = wpa_sm_rx_eapol_patch;
    return;
}
