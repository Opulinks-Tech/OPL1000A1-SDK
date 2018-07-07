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

#ifndef _LWIP_HELPER_PATCH_H_
#define _LWIP_HELPER_PATCH_H_

#include <stdio.h>
#include <stdint.h>

#include "lwip/ip4_addr.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Private static api function */
typedef int32_t (*lwip_get_netif_fp_t)(struct netif *iface);

/* Export interface funtion pointer */
extern lwip_get_netif_fp_t      lwip_get_netif;

#ifdef __cplusplus
}
#endif

#endif /* _LWIP_HELPER_PATCH_H_ */
