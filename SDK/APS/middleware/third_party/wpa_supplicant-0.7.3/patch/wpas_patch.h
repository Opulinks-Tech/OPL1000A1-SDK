/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlnik Communication Corp. (C) 2017
******************************************************************************/

#ifndef __WPAS_PATCH_H__
#define __WPAS_PATCH_H__
#include "wpa_cli.h"
#include "driver_netlink.h"
#include "defs.h"
#include "wpa.h"

//External Reference
extern struct wpa_scan_results res;
extern struct wpa_config conf;
extern struct wpa_config rec_conf;
extern struct wpa_ssid ssid_data;
extern struct wpa_ssid rec_ssid_data;
extern struct wpa_supplicant *wpa_s;
extern u8 g_bssid[6];
extern char g_passphrase[MAX_LEN_OF_PASSWD];
extern const char * wpa_supplicant_state_txt(enum wpa_states state);
extern struct wpa_ptk g_ptk;

//Function Delaration
void wpas_patch_init(void);
#endif

