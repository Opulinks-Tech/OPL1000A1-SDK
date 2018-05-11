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

#ifndef __WIFI_MAC_PATCH__
#define __WIFI_MAC_PATCH__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ieee80211_crypto_ccmp.h"

u8* ccmp_decrypt_impl_v1(const u8 *tk, const struct ieee80211_hdr *hdr, u8 *data, size_t data_len, size_t *decrypted_len);
u8* ccmp_encrypt_impl_v1(const u8 *tk, u8 *frame, size_t len, size_t hdrlen, u8 *qos, u8 *pn, int keyid, size_t *encrypted_len);

void wifi_mac_crypto_func_init_patch(void);
void wifi_mac_task_func_init_patch(void);
void wifi_mac_rx_data_func_init_patch(void);

#endif //#ifndef __WIFI_MAC_PATCH__

