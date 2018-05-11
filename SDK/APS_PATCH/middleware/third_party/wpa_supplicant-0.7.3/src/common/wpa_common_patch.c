/*
 * WPA/RSN - Shared functions for supplicant and authenticator
 * Copyright (c) 2002-2008, Jouni Malinen <j@w1.fi>
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
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "aes_wrap.h"
#include "crypto.h"
#include "ieee802_11_defs.h"
#include "defs.h"
#include "wpa_common.h"
#include "basic_defs.h"
#include "scrt.h"
#include "wpa_debug_patch.h"

void wpa_pmk_to_ptk_patch(const u8 *pmk, size_t pmk_len, const char *label,
		    const u8 *addr1, const u8 *addr2,
		    const u8 *nonce1, const u8 *nonce2,
		    u8 *ptk, size_t ptk_len, int use_sha256)
{
	u8 data[2 * ETH_ALEN + 2 * WPA_NONCE_LEN];
    //int ret;

	if (os_memcmp(addr1, addr2, ETH_ALEN) < 0) {
		os_memcpy(data, addr1, ETH_ALEN);
		os_memcpy(data + ETH_ALEN, addr2, ETH_ALEN);
	} else {
		os_memcpy(data, addr2, ETH_ALEN);
		os_memcpy(data + ETH_ALEN, addr1, ETH_ALEN);
	}

	if (os_memcmp(nonce1, nonce2, WPA_NONCE_LEN) < 0) {
		os_memcpy(data + 2 * ETH_ALEN, nonce1, WPA_NONCE_LEN);
		os_memcpy(data + 2 * ETH_ALEN + WPA_NONCE_LEN, nonce2,
			  WPA_NONCE_LEN);
	} else {
		os_memcpy(data + 2 * ETH_ALEN, nonce2, WPA_NONCE_LEN);
		os_memcpy(data + 2 * ETH_ALEN + WPA_NONCE_LEN, nonce1,
			  WPA_NONCE_LEN);
	}

    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: call sha1_prf ");
	//ret = sha1_prf(pmk, pmk_len, label, data, sizeof(data), ptk, ptk_len);
	sha1_prf(pmk, pmk_len, label, data, sizeof(data), ptk, ptk_len);
    //wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: ret of sha1_prf: %d ", ret); //0

	//wpa_printf(MSG_DEBUG, "\r\n wpa_pmk_to_ptk, WPA: PTK derivation - A1=" MACSTR " A2=" MACSTR,
	//	   MAC2STR(addr1), MAC2STR(addr2));

#if 0
    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: PTK derivation - A1=%02x:%02x:%02x:%02x:%02x:%02x ",
                         *(addr1+0),
                         *(addr1+1),
                         *(addr1+2),
                         *(addr1+3),
                         *(addr1+4),
                         *(addr1+5)); //A1=22:33:44:55:66:76
    wpa_printf_dbg(MSG_DEBUG, "[KEY]WPA: PTK derivation - A2=%02x:%02x:%02x:%02x:%02x:%02x ",
                         *(addr2+0),
                         *(addr2+1),
                         *(addr2+2),
                         *(addr2+3),
                         *(addr2+4),
                         *(addr2+5)); //A2=80:26:89:58:64:d3
#endif
	//wpa_hexdump_key(MSG_DEBUG, "[KEY]WPA: PMK", pmk, pmk_len); //PMK - hexdump(len=32): e8 d1 d6 69 13 0c c6 d7 aa 5a 4e 0b 07 9f 49 a8 c5 b6 0b d3 30 23 4e f0 f0 30 ec a6 fc 04 29 fd
	//wpa_hexdump_key(MSG_DEBUG, "[KEY]WPA: PTK", ptk, ptk_len); //PTK - hexdump(len=48): cb 63 d8 48 4f 7c 55 c4 c8 de 67 72 56 af 62 08 43 44 5b 42 73 d8 79 95 ff a1 aa e1 c0 f1 df 2d 9f c3 3f 01 ea 8a 16 3d f5 e8 3e d3 75 5a bb 63
}

int wpa_eapol_key_mic_patch(const u8 *key, int ver, const u8 *buf, size_t len,
		      u8 *mic)
{
	u8 hash[SHA1_MAC_LEN];
    bool ret;

    //wpa_printf_dbg(MSG_INFO, "[KEY]WPA: MIC Caculation, ver:%d len:%d \r\n", ver, len);

	switch (ver) {
	case WPA_KEY_INFO_TYPE_HMAC_MD5_RC4:
        //wpa_printf(MSG_INFO, "\r\n wpa_eapol_key_mic, case WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 \r\n");
		return hmac_md5(key, 16, buf, len, mic);
	case WPA_KEY_INFO_TYPE_HMAC_SHA1_AES:
        //wpa_printf(MSG_INFO, "\r\n wpa_eapol_key_mic, case WPA_KEY_INFO_TYPE_HMAC_SHA1_AES \r\n");
#if 0
		if (hmac_sha1(key, 16, buf, len, hash))
			return -1;
#endif
        wpa_printf_dbg(MSG_INFO, "[KEY]WPA: call hw api nl_hmac_sha_1 \r\n");

        //ret = nl_hmac_sha_1(key, 16, buf, len, hash);
        ret = nl_hmac_sha_1((uint8_t *) key, 16, (uint8_t *) buf, (int) len, (uint8_t *) hash);

        if(ret == false) return -1;

		os_memcpy(mic, hash, MD5_MAC_LEN);
		break;
	default:
		return -1;
	}

	return 0;
}

/*
   Interface Initialization: WPA_COMMON
 */
void wpa_common_func_init_patch(void)
{
    wpa_pmk_to_ptk = wpa_pmk_to_ptk_patch;
    wpa_eapol_key_mic = wpa_eapol_key_mic_patch;
    return;
}

