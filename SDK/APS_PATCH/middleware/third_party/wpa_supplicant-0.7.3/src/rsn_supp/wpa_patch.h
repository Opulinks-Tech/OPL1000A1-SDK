/*
 * wpa_supplicant - WPA definitions
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
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

#ifndef __WPA_PATCH_H__
#define __WPA_PATCH_H__

#include "defs.h"
#include "eapol_common.h"
#include "wpa_common.h"

//void wpa_printf_dbg(int level, char *fmt, ...);

int wpa_supplicant_send_2_of_4_patch(struct wpa_sm *sm, const unsigned char *dst,
			       const struct wpa_eapol_key *key,
			       int ver, const u8 *nonce,
			       const u8 *wpa_ie, size_t wpa_ie_len,
			       struct wpa_ptk *ptk);

void wpa_supplicant_process_3_of_4_patch(struct wpa_sm *sm,
					  const struct wpa_eapol_key *key,
					  u16 ver);

void wpa_func_init_patch(void);

#endif

