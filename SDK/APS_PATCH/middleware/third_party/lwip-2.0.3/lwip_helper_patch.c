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

extern sys_sem_t wifi_connected;
extern sys_sem_t ip_ready;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
/* NETIF data */
extern struct netif netif;

LWIP_RETDATA bool tcpip_inited;

/*****************************************************************************
 * Private functions declarations
 ****************************************************************************/
//static void tcpip_init_done( void* arg );
//static void ip_ready_callback(struct netif *netif);

//static int32_t wifi_station_connected_event_handler(void);
//static int32_t wifi_station_disconnected_event_handler(void);

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void ip_ready_callback_patch(struct netif *netif)
{
    event_msg_t msg = {0};
    if (!ip4_addr_isany(netif_ip4_addr(netif))) {
        char ipaddrstr[16] = {0};
        ipaddr_ntoa_r(&netif->ip_addr, ipaddrstr, sizeof(ipaddrstr));
        printf("DHCP got IP:%s\r\n", ipaddrstr);
        sys_sem_signal(&ip_ready);

        msg.event = WIFI_EVENT_STA_GOT_IP;
        msg.length = 0;
        msg.param = NULL;
        wifi_event_loop_send(&msg);
    }
    else
    {
        printf("DHCP got Failed\r\n");
    }
}


void lwip_net_ready_patch(void)
{
    sys_arch_sem_wait(&wifi_connected, 0);
    if(dhcp_config_init() == STA_IP_MODE_DHCP) {
        sys_arch_sem_wait(&ip_ready, 0);
    }
}

void lwip_network_init_patch(uint8_t opmode)
{
    LWIP_UNUSED_ARG(opmode);

    if (tcpip_inited == false) {
        tcpip_inited = true;

        /* Initialize the LwIP system.  */
        LWIP_DEBUGF(LWIP_DBG_ON, ("Initialising LwIP " "2.0.0" "\n"));

        sys_sem_new(&wifi_connected, 0);

        if(dhcp_config_init() == STA_IP_MODE_DHCP) {
            sys_sem_new(&ip_ready, 0);
        }

        lwip_tcpip_init();
    }
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/*-------------------------------------------------------------------------------------
 * Interface assignment
 *------------------------------------------------------------------------------------*/
void lwip_load_interface_lwip_helper_patch(void)
{
    tcpip_inited = false;
    lwip_network_init   = lwip_network_init_patch;
    ip_ready_callback   = ip_ready_callback_patch;
    lwip_net_ready      = lwip_net_ready_patch;
}

