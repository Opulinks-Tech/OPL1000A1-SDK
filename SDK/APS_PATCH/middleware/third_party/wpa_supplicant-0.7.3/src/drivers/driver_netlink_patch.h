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

#ifndef _DRIVER_NETLINK_PATCH_h_
#define _DRIVER_NETLINK_PATCH_h_

#include "defs.h"

void wpa_driver_func_init_patch(void);
int wpa_driver_netlink_get_state(void);
Boolean wpa_driver_netlink_scan_by_cfg(void *cfg);

#endif /* _DRIVER_NETLINK_PATCH_h_ */
