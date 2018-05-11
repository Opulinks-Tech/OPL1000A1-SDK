/*
 * WPA definitions shared between hostapd and wpa_supplicant
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

#ifndef __WPA_COMMON_PATCH_H__
#define __WPA_COMMON_PATCH_H__

#include "defs.h"

/*
  Scan Mode
    #0: Active Scan
    #1: Passive Scan
    #2: Mix Scan (combine Active and Passive)
*/
enum wifi_scan_mode_ext
{
  SCAN_MODE_ACTIVE_EXT = 0,
  SCAN_MODE_PASSIVE_EXT,
  SCAN_MODE_MIX_EXT,
  SCAN_MODE_NUM_EXT,
};

void wpa_common_func_init_patch(void);

void wpa_pmk_to_ptk_patch(const u8 *pmk, size_t pmk_len, const char *label,
		    const u8 *addr1, const u8 *addr2,
		    const u8 *nonce1, const u8 *nonce2,
		    u8 *ptk, size_t ptk_len, int use_sha256);

#endif

