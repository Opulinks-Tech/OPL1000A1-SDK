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

#ifndef __EVENTS_NETLINK_PATCH_H__
#define __EVENTS_NETLINK_PATCH_H__
#include "wpa_supplicant_i.h"
#include "wpabuf.h"
#include "common.h"
#include "driver.h"

void wpa_supplicant_event_assoc_patch(struct wpa_supplicant *wpa_s, union wpa_event_data *data);
void events_netlink_func_init_patch(void);

#endif

