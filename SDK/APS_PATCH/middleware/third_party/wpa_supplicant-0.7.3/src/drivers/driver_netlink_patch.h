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

#ifndef __DRIVER_NETLINK_PATCH_H__
#define __DRIVER_NETLINK_PATCH_H__

typedef void (*wpa_driver_netlink_show_scan_results_fp_t)(void);
typedef Boolean (*wpa_driver_netlink_fast_connect_fp_t)(u8 mode, u8 index);
typedef int (*wpa_driver_netlink_sta_cfg_fp_t)(u8 mode, u8 cmd_idx, u8 *value);
typedef Boolean (*wpa_driver_netlink_get_sta_cfg_fp_t)(u8 cfg_idx, void *ptr);

extern wpa_driver_netlink_show_scan_results_fp_t wpa_driver_netlink_show_scan_results;
extern wpa_driver_netlink_scan_results_free_fp_t wpa_driver_netlink_scan_results_clear;
extern wpa_driver_netlink_fast_connect_fp_t wpa_driver_netlink_fast_connect;
extern wpa_driver_netlink_sta_cfg_fp_t wpa_driver_netlink_sta_cfg;
extern wpa_driver_netlink_get_sta_cfg_fp_t wpa_driver_netlink_get_sta_cfg;

void wpa_driver_func_init_patch(void);
int wpa_driver_netlink_get_state(void);

#endif

