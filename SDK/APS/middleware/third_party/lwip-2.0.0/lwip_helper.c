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

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/dhcp.h"

#include "wlannetif.h"
#include "lwip_helper.h"
#include "network_config.h"

sys_sem_t wifi_connected;
sys_sem_t ip_ready;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
/* NETIF data */
struct netif netif;

/*****************************************************************************
 * Private functions declarations
 ****************************************************************************/
static void tcpip_init_done( void* arg );
static void ip_ready_callback(struct netif *netif);

static int32_t wifi_station_connected_event_handler(void);
static int32_t wifi_station_disconnected_event_handler(void);

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void tcpip_init_done( void* arg )
{
	/* Tell main thread TCP/IP init is done */
	*(s32_t *)arg = 1;
}


/**
  * @brief  wifi connected will call this callback function. set lwip status in this function
  * @retval None
  */
static int32_t wifi_station_connected_event_handler(void)
{
    netif_set_link_up(&netif);
    sys_sem_signal(&wifi_connected);
    printf("wifi connected\r\n");
    return 0;
}

/**
  * @brief  wifi disconnected will call this callback function. set lwip status in this function
  * @retval None
  */
static int32_t wifi_station_disconnected_event_handler(void)
{
    netif_set_link_down(&netif);
    if(dhcp_config_init() == STA_IP_MODE_DHCP) {
        netif_set_addr(&netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    }    
    printf("wifi disconnected\r\n");
    return 1;
}

static void ip_ready_callback(struct netif *netif)
{
    if (!ip4_addr_isany(netif_ip4_addr(netif))) {
        char ipaddrstr[16] = {0};
        ipaddr_ntoa_r(&netif->ip_addr, ipaddrstr, sizeof(ipaddrstr));
        printf("DHCP got IP:%s\r\n", ipaddrstr);
    }
    else
    {
        printf("DHCP got Failed\r\n");
    }
    sys_sem_signal(&ip_ready);
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
void lwip_tcpip_init(void)
{
	ip4_addr_t ipaddr, netmask, gw;
	volatile s32_t tcpipdone = 0;
    lwip_tcpip_config_t tcpip_config = {{0}, {0}, {0}};

    tcpip_init(tcpip_init_done, (void *) &tcpipdone);
	while (!tcpipdone) {
		sys_msleep(1);
	}

	LWIP_DEBUGF(LWIP_DBG_ON, ("LwIP TCPIP thread is initialized.. \n"));

    if (0 != tcpip_config_init(&tcpip_config)) {
        LWIP_DEBUGF(LWIP_DBG_ON, ("tcpip config init fail \n"));
        return;
    }

    ipaddr = tcpip_config.sta_ip;
    netmask = tcpip_config.sta_mask;
    gw = tcpip_config.sta_gw;

	/* Add netif interface for NL1000, wlan interface */
	if (!netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input)) {
		LWIP_ASSERT("Net interface failed to initialize\r\n", 0);
	}
	netif_set_default(&netif);
	netif_set_up(&netif);

    //netif_set_link_callback(&netif, lwip_netif_link_irq);
    //netif_set_status_callback(&netif, lwip_netif_status_irq);

#if USE_DHCP
	//dhcp_start(&netif);
#endif
}

void lwip_network_init(uint8_t opmode)
{
    LWIP_UNUSED_ARG(opmode);

    /* Initialize the LwIP system.  */
    LWIP_DEBUGF(LWIP_DBG_ON, ("Initialising LwIP " "2.0.0" "\n"));

    sys_sem_new(&wifi_connected, 0);

    if(dhcp_config_init() == STA_IP_MODE_DHCP) {
        sys_sem_new(&ip_ready, 0);
    }

    lwip_tcpip_init();
}

void lwip_net_start(uint8_t opmode)
{
    //struct netif *sta_if;
    //struct netif *ap_if;
    LWIP_UNUSED_ARG(opmode);

    switch(opmode) {
        case WIFI_MODE_STA_ONLY:
            //ToDO: register wifi connection event
            //wifi_connection_register_event_handler(WIFI_EVENT_IOT_PORT_SECURE, wifi_station_connected_event_handler);
            //wifi_connection_register_event_handler(WIFI_EVENT_IOT_DISCONNECTED, wifi_station_disconnected_event_handler);
            if(dhcp_config_init() == STA_IP_MODE_DHCP) {
                //sta_if = netif_find_by_type(NETIF_TYPE_STA);
                netif_set_default(&netif);
                netif_set_link_up(&netif);
                netif_set_status_callback(&netif, ip_ready_callback);
                dhcp_start(&netif);
            }
            else
            {
                netif_set_link_up(&netif);
            }
            break;
        case WIFI_MODE_AP_ONLY: {
            /*
            dhcpd_settings_t dhcpd_settings = {{0},{0},{0},{0},{0},{0},{0}};
            strcpy((char *)dhcpd_settings.dhcpd_server_address, AP_IPADDR);
            strcpy((char *)dhcpd_settings.dhcpd_netmask, AP_NETMASK);
            strcpy((char *)dhcpd_settings.dhcpd_gateway, AP_GATEWAY);
            strcpy((char *)dhcpd_settings.dhcpd_primary_dns, PRIMARY_DNS);
            strcpy((char *)dhcpd_settings.dhcpd_secondary_dns, SECONDARY_DNS);
            strcpy((char *)dhcpd_settings.dhcpd_ip_pool_start, IP_POOL_START);
            strcpy((char *)dhcpd_settings.dhcpd_ip_pool_end, IP_POOL_END);
            ap_if = netif_find_by_type(NETIF_TYPE_AP);
            netif_set_default(ap_if);
            netif_set_link_up(ap_if);
            dhcpd_start(&dhcpd_settings);
            */
            break;
        }
    }


}

void lwip_net_stop(uint8_t opmode)
{
    //struct netif *sta_if;
    //struct netif *ap_if;

    //sta_if = netif_find_by_type(NETIF_TYPE_STA);
    //ap_if = netif_find_by_type(NETIF_TYPE_AP);
    switch (opmode) {
        case WIFI_MODE_AP_ONLY:
            /*
            dhcpd_stop();
            netif_set_link_down(ap_if);
            break;*/
        case WIFI_MODE_STA_ONLY:
            netif_set_status_callback(&netif, NULL);
            if(dhcp_config_init() == STA_IP_MODE_DHCP) {
                dhcp_release(&netif);
                dhcp_stop(&netif);
            }
            netif_set_link_down(&netif);
            break;
    }
}

void lwip_net_ready()
{
    //sys_arch_sem_wait(&wifi_connected, 0);
    if(dhcp_config_init() == STA_IP_MODE_DHCP) {
        sys_arch_sem_wait(&ip_ready, 0);
    }
}

int lwip_get_ip_info(char *ifname)
{
    struct netif *iface = netif_find(ifname);

    if (!iface && !netif_is_up(iface)) {
        return -1;
    }

    printf("\ninterface: %s\n", ifname);

#if LWIP_DHCP
    if (dhcp_supplied_address(iface)) {
        struct dhcp *d = ((struct dhcp*)(iface)->client_data[LWIP_NETIF_CLIENT_DATA_INDEX_DHCP]);
        printf("ip mode : DHCP\n");
        printf("  ip      %s\n", ip4addr_ntoa(&d->offered_ip_addr));
        printf("  netmask %s\n", ip4addr_ntoa(&d->offered_sn_mask));
        printf("  gateway %s\n", ip4addr_ntoa(&d->offered_gw_addr));
    }
    else
#endif
    {
        printf("ip mode : STATIC\n");
        printf("ip addr : %s\n", ipaddr_ntoa(&iface->ip_addr));
        printf("netmask : %s\n", ipaddr_ntoa(&iface->netmask));
        printf("gateway : %s\n", ipaddr_ntoa(&iface->gw));
    }

    return 0;
}

