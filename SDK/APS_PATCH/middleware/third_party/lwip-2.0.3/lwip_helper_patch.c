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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/dhcp.h"

#include "wlannetif.h"
#include "lwip_helper.h"
#include "network_config.h"
#include "event_loop.h"
#include "wifi_api.h"
#include "wifi_mac_task.h"

#include "ps_patch.h"
#include "lwip/tcpip_patch.h"

extern LWIP_RETDATA sys_sem_t wifi_connected;
extern LWIP_RETDATA sys_sem_t ip_ready;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
/* NETIF data */
extern LWIP_RETDATA struct netif netif;
extern LWIP_RETDATA bool tcpip_inited;

/*****************************************************************************
 * Private functions declarations
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void lwip_check_timeouts(PS_WAKEUP_TYPE wake_type)
{
    tcpip_check_timeouts();
    return;
}
/*****************************************************************************
 * Public functions
 ****************************************************************************/
void lwip_network_init_patch(uint8_t opmode)
{
    LWIP_UNUSED_ARG(opmode);

    if (tcpip_inited == false) {
        tcpip_inited = true;

        sys_sem_new(&wifi_connected, 0);

        if(dhcp_config_init() == STA_IP_MODE_DHCP) {
            sys_sem_new(&ip_ready, 0);
        }

        lwip_tcpip_init();

        /* register check timeouts callback for Smart Sleep */
        ps_set_wakeup_callback_internal(lwip_check_timeouts);
    }
}

/*-------------------------------------------------------------------------------------
 * Interface assignment
 *------------------------------------------------------------------------------------*/
void lwip_load_interface_lwip_helper_patch(void)
{
    /* Cold boot initialization for "zero_init" retention data */

    lwip_network_init   = lwip_network_init_patch;
}

