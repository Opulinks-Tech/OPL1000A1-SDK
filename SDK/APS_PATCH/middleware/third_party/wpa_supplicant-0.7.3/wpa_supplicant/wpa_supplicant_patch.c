/*
 * WPA Supplicant
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
 *
 * This file implements functions for registering and unregistering
 * %wpa_supplicant interfaces. In addition, this file contains number of
 * functions for managing network connections.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "includes.h"
#include "common.h"
#include "eapol_supp_sm.h"
#include "eap.h"
#include "eap_methods.h"
#include "wpa.h"
#include "eloop.h"
#include "config.h"
#include "l2_packet.h"
#include "wpa_supplicant_i.h"
#include "driver_i.h"
#include "ctrl_iface.h"
#include "pcsc_funcs.h"
#include "version.h"
#include "preauth.h"
#include "pmksa_cache.h"
#include "wpa_ctrl.h"
#include "mlme.h"
#include "ieee802_11_defs.h"
#include "wpas_glue.h"
#include "wps_supplicant.h"
#include "sme.h"
#include "ap.h"
#include "notify.h"
#include "bss.h"
#include "scan.h"
#include "msg.h"
#include "wpa_auth.h"
#include "wpa_ie.h"
#include "events_netlink.h"
#include "at_cmd_common.h"
#include "wpa_debug_patch.h"

extern struct wpa_supplicant *wpa_s;

void wpa_supplicant_rx_eapol_netlink_patch( const u8 *src_addr,
			     const u8 *buf, size_t len)
{
#if 0
	wpa_printf(MSG_DEBUG, "[KEY]WPA: RX EAPOL from %02x:%02x:%02x:%02x:%02x:%02x \r\n", *(src_addr+0),
                                                                                                               *(src_addr+1),
                                                                                                               *(src_addr+2),
                                                                                                               *(src_addr+3),
                                                                                                               *(src_addr+4),
                                                                                                               *(src_addr+5));
	wpa_hexdump(MSG_DEBUG, "[KEY]WPA: RX EAPOL", buf, len);
    wpa_printf(MSG_DEBUG, "[KEY]WPA: len:%d key_mgmt:%d \r\n", len, wpa_s->key_mgmt);
#endif

    //Check key_mgmt
	if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE) {
		wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: Ignored received EAPOL frame since no key management is configured \r\n");
		return;
	}

#if 0
    wpa_s->wpa->key_mgmt = wpa_s->key_mgmt;
    wpa_s->wpa->pairwise_cipher =  wpa_s->pairwise_cipher;
    wpa_s->wpa->group_cipher =  wpa_s->group_cipher;
    wpa_s->wpa->mgmt_group_cipher = wpa_s->mgmt_group_cipher;
#endif //#if 0

	wpa_sm_rx_eapol(wpa_s->wpa, src_addr, buf, len);
}

int wpa_supplicant_parse_ies_patch(const u8 *buf, size_t len,
			     struct wpa_eapol_ie_parse *ie)
{
	const u8 *pos, *end;
	int ret = 0;

    //wpa_printf(MSG_DEBUG, "[IE]WPA: Parse IE, len:%d \r\n", len);

	os_memset(ie, 0, sizeof(*ie));
	for (pos = buf, end = pos + len; pos + 1 < end; pos += 2 + pos[1]) {
		if (pos[0] == 0xdd &&
		    ((pos == buf + len - 1) || pos[1] == 0)) {
			/* Ignore padding */
			break;
		}
		if (pos + 2 + pos[1] > end) { //no enter
		    /*
			wpa_printf(MSG_DEBUG, "[IE]WPA: WPA: EAPOL-Key Key Data "
				   "underflow (ie=%d len=%d pos=%d) \r\n",
				   pos[0], pos[1], (int) (pos - buf));*/
			wpa_hexdump_key(MSG_DEBUG, "WPA: Key Data", buf, len);
			ret = -1;
			break;
		}
		if (*pos == WLAN_EID_RSN) { //enter
			ie->rsn_ie = pos;
			ie->rsn_ie_len = pos[1] + 2;
			wpa_hexdump(MSG_DEBUG, "[IE]WPA: RSN IE in EAPOL-Key", ie->rsn_ie, ie->rsn_ie_len);
		} else if (*pos == WLAN_EID_VENDOR_SPECIFIC) { //enter
    		//wpa_printf(MSG_DEBUG, "[IE]WPA: call wpa_parse_generic \r\n");

			ret = wpa_parse_generic(pos, end, ie);
			if (ret < 0)
				break;
			if (ret > 0) {
				ret = 0;
				break;
			}
		} else {
			wpa_hexdump(MSG_DEBUG, "[IE]WPA: Unrecognized EAPOL-Key Key Data IE", pos, 2 + pos[1]);
		}
	}

	return ret;
}

/*
   Interface Initialization: WPA_SUPPLICANT
 */
void wpa_supplicant_func_init_patch(void)
{
    wpa_supplicant_rx_eapol_netlink = wpa_supplicant_rx_eapol_netlink_patch;
    wpa_supplicant_parse_ies = wpa_supplicant_parse_ies_patch;
    return;
}

