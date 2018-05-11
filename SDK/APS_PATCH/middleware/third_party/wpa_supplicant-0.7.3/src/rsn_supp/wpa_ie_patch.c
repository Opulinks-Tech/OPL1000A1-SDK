/*
 * wpa_supplicant - WPA/RSN IE and KDE processing
 * Copyright (c) 2003-2008, Jouni Malinen <j@w1.fi>
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
#include "wpa.h"
#include "pmksa_cache.h"
#include "ieee802_11_defs.h"
#include "wpa_i.h"
#include "wpa_ie.h"
#include "wpa_debug_patch.h"

int wpa_parse_generic_patch(const u8 *pos, const u8 *end,
			     struct wpa_eapol_ie_parse *ie)
{
    int i;

    //wpa_printf(MSG_DEBUG, "[KEY]WPA: pos[1]:%d \r\n", pos[1]); //WPA PSK, wpa_parse_generic, pos[1]:22
    //wpa_printf(MSG_DEBUG, "[KEY]WPA: pos[2]:%x pos[3]:%x pos[4]:%x pos[5]:%x \r\n", pos[2], pos[3], pos[4], pos[5]); //
    //wpa_printf(MSG_DEBUG, "[KEY]WPA: pos[2 + WPA_SELECTOR_LEN]:%d \r\n", pos[2 + WPA_SELECTOR_LEN]); //
    //wpa_printf(MSG_DEBUG, "[KEY]WPA: pos[2 + WPA_SELECTOR_LEN + 1]:%d \r\n", pos[2 + WPA_SELECTOR_LEN + 1]); //

	if (pos[1] == 0)
		return 1;

	if (pos[1] >= 6 &&

	    //RSN_SELECTOR_GET(pos + 2) == WPA_OUI_TYPE &&
	    pos[2] == 0x0 &&
	    pos[3] == 0x50 &&
	    pos[4] == 0xf2 &&
	    pos[5] == 0x1 &&

	    pos[2 + WPA_SELECTOR_LEN] == 1 &&
	    pos[2 + WPA_SELECTOR_LEN + 1] == 0) {
		ie->wpa_ie = pos;
		ie->wpa_ie_len = pos[1] + 2;
		wpa_hexdump(MSG_DEBUG, "[KEY]WPA: WPA IE in EAPOL-Key", ie->wpa_ie, ie->wpa_ie_len);
		return 0;
	}

	if (pos + 1 + RSN_SELECTOR_LEN < end &&
	    pos[1] >= RSN_SELECTOR_LEN + PMKID_LEN &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_PMKID) { //enter
		ie->pmkid = pos + 2 + RSN_SELECTOR_LEN;
		wpa_hexdump(MSG_DEBUG, "[KEY]WPA: PMKID in EAPOL-Key", pos, pos[1] + 2);  //hexdump(len=22): dd 14 00 0f ac 04 ba 37 7b 1a e7 dd 3e e9 5b 06 ab dc 4a ad 81 54
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_GROUPKEY) {
		ie->gtk = pos + 2 + RSN_SELECTOR_LEN;
		ie->gtk_len = pos[1] - RSN_SELECTOR_LEN;

        wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ie->gtk_len:%d \r\n", ie->gtk_len);
        wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ie->gtk:");
        for (i=0; i<ie->gtk_len; i++) wpa_printf_dbg(MSG_DEBUG, "%02x ", *(ie->gtk + i));
        wpa_printf_dbg(MSG_DEBUG, "\r\n");
		wpa_hexdump_key(MSG_DEBUG, "[KEY]WPA: GTK in EAPOL-Key", pos, pos[1] + 2);
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_MAC_ADDR) {
		ie->mac_addr = pos + 2 + RSN_SELECTOR_LEN;
		ie->mac_addr_len = pos[1] - RSN_SELECTOR_LEN;
		wpa_hexdump(MSG_DEBUG, "[KEY]WPA: MAC Address in EAPOL-Key", pos, pos[1] + 2);
		return 0;
	}

#ifdef CONFIG_PEERKEY
	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_SMK) {
		ie->smk = pos + 2 + RSN_SELECTOR_LEN;
		ie->smk_len = pos[1] - RSN_SELECTOR_LEN;
		wpa_hexdump_key(MSG_DEBUG, "[KEY]WPA: SMK in EAPOL-Key", pos, pos[1] + 2);
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_NONCE) {
		ie->nonce = pos + 2 + RSN_SELECTOR_LEN;
		ie->nonce_len = pos[1] - RSN_SELECTOR_LEN;
		wpa_hexdump(MSG_DEBUG, "[KEY]WPA: Nonce in EAPOL-Key", pos, pos[1] + 2);
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_LIFETIME) {
		ie->lifetime = pos + 2 + RSN_SELECTOR_LEN;
		ie->lifetime_len = pos[1] - RSN_SELECTOR_LEN;
		wpa_hexdump(MSG_DEBUG, "[KEY]WPA: Lifetime in EAPOL-Key", pos, pos[1] + 2);
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_ERROR) {
		ie->error = pos + 2 + RSN_SELECTOR_LEN;
		ie->error_len = pos[1] - RSN_SELECTOR_LEN;
		wpa_hexdump(MSG_DEBUG, "[KEY]WPA: Error in EAPOL-Key", pos, pos[1] + 2);
		return 0;
	}
#endif /* CONFIG_PEERKEY */

	return 0;
}

/*
   Interface Initialization: WPA IE
 */
void wpa_ie_func_init_patch(void)
{
    wpa_parse_generic = wpa_parse_generic_patch;
    return;
}

