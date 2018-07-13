/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "arch/sys_arch.h"
#include "wlannetif.h"
//#include "netif/ppp_oe.h"
#include <string.h>

#include "wifi_mac_task.h"
#include "msg.h"

/* Define those to better describe your network interface. */
#define IFNAME0 's'
#define IFNAME1 't'

#define TRANSPORT_TASK
//#define IF_LOOPBACK
//#define TX_PKT_DUMP
//#define RX_PKT_DUMP

#if defined(LWIP_ROMBUILD)
#include "port/wlannetif_if.h"
#endif

#include "wlannetif_patch.h"
#include "wifi_nvm_patch.h"

#define TX_TASK_STACKSIZE           (512)
#ifdef LWIP_DEBUG
#define RX_TASK_STACKSIZE           (512*2)
#else
#define RX_TASK_STACKSIZE           (512)
#endif

#define RX_PRIORITY                 (tskIDLE_PRIORITY + 2)//(osPriorityBelowNormal)
#define TX_PRIORITY                 (tskIDLE_PRIORITY + 2)//(osPriorityBelowNormal)


struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};


extern sys_sem_t TxReadySem;
extern sys_sem_t RxReadySem; /**< RX packet ready semaphore */
extern sys_sem_t TxCleanSem;
extern sys_thread_t rx_thread_handle;
extern struct netif netif;

char g_sLwipHostName[WLAN_HOST_NAME_LEN + 1] = {WLAN_DEF_HOST_NAME};


int wlan_input(void *buffer, uint16_t len)
{
    ethernetif_input(&netif, buffer, len);
    return 0;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
struct pbuf *
low_level_input_patch(struct netif *netif, void *buf, u16_t len)
{
    struct ethernetif *ethernetif = netif->state;
    struct pbuf *p = NULL, *q = NULL;
    //u16_t l = 0;

    LWIP_UNUSED_ARG(ethernetif);

    /* Drop oversized packet */
    if (len > 1514) {
        goto done;
        //return NULL;
    }

    if (!netif || !buf || len <= 0)
        goto done;
        //return NULL;

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    /* We allocate a continous pbuf */
    //p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);

    if (p != NULL) {

#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* pbufs allocated from the RAM pool should be non-chained. */
        LWIP_ASSERT("lpc_rx_queue: pbuf is not contiguous (chained)",
        pbuf_clen(p) <= 1);

        /* Copy the data to intermediate buffer. This is required because
           the driver copies all the data to one continuous packet data buffer. */

        memcpy(p->payload, buf, len);

        /* We iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for(q = p; q != NULL; q = q->next) {
          /* Read enough bytes to fill this pbuf in the chain. The
           * available data in the pbuf is given by the q->len
           * variable.
           * This does not necessarily have to be a memcpy, you can also preallocate
           * pbufs for a DMA-enabled MAC and after receiving truncate it to the
           * actually received size. In this case, ensure the tot_len member of the
           * pbuf is the sum of the chained pbuf len members.
           */

            //memcpy((u8_t*)q->payload, (u8_t*)&buf[l], q->len);
            //l = l + q->len;
        }

        // Acknowledge that packet has been read;
        //wifi_mac_rx_queue_first_entry_free();

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        /* drop packet(); */
        //wifi_mac_rx_queue_first_entry_free();
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

done:
    wifi_mac_rx_queue_first_entry_free();
    return p;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init_patch(struct netif *netif)
{
    err_t err;
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    ethernetif = mem_malloc(sizeof(struct ethernetif));
    if (ethernetif == NULL) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    //netif->hostname = "netlink";
    /*
    snprintf(g_sLwipHostName, sizeof(g_sLwipHostName), "%s%02X%02X%02X", 
             WLAN_DEF_HOST_NAME_PREFIX, 
             s_StaInfo.au8Dot11MACAddress[3], s_StaInfo.au8Dot11MACAddress[4], s_StaInfo.au8Dot11MACAddress[5]);
    */
    netif->hostname = g_sLwipHostName;
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
#if LWIP_ARP
    netif->output = etharp_output;
#else /* LWIP_ARP */
    netif->output = NULL; /* not used for PPPoE */
#endif /* LWIP_ARP */
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init(netif);

#ifdef TRANSPORT_TASK
    /* Packet receive task */
    #if 0
    err = sys_sem_new(&RxReadySem, 0);
    LWIP_ASSERT("RxReadySem creation error", (err == ERR_OK));
    if((rx_thread_handle = sys_thread_new("receive_thread", __packet_rx_task, netif, RX_TASK_STACKSIZE, RX_PRIORITY)) == NULL)
    {
        printf("__packet_rx_task create failed\r\n");
    }
    #endif
	/* Transmit cleanup task */
	err = sys_sem_new(&TxReadySem, 0);
	LWIP_ASSERT("TxReadySem  creation error", (err == ERR_OK));

#ifdef IF_LOOPBACK
    err = sys_sem_new(&TxCleanSem, 0);
	LWIP_ASSERT("TxCleanSem creation error", (err == ERR_OK));
    if( NULL == sys_thread_new("txclean_thread", __packet_tx_task, netif, TX_TASK_STACKSIZE, TX_PRIORITY))
    {
        printf("__packet_tx_task create failed\r\n");
    }
#endif
    wifi_mac_rx_notify_tcp_callback_registration((wifi_mac_rx_notify_tcp_callback_t)wifi_rx_callback);
    wifi_mac_tx_notify_tcp_callback_registration((wifi_mac_tx_notify_tcp_callback_t)wifi_tx_callback);
#endif /* TRANSPORT_TASK */

    wifi_mac_register_rxcb(NULL, wlan_input);

    return ERR_OK;
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init_patch(struct netif *netif)
{
    struct ethernetif *ethernetif = netif->state;

    LWIP_UNUSED_ARG(ethernetif);

    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    /*netif->hwaddr[0] = 0x00;
    netif->hwaddr[1] = 0x11;
    netif->hwaddr[2] = 0x22;
    netif->hwaddr[3] = 0x33;
    netif->hwaddr[4] = 0x44;
    netif->hwaddr[5] = 0x55;
    
    netif->hwaddr[0] = 0x22;
    netif->hwaddr[1] = 0x33;
    netif->hwaddr[2] = 0x44;
    netif->hwaddr[3] = 0x55;
    netif->hwaddr[4] = 0x66;
    netif->hwaddr[5] = 0x76;
    */
    //memcpy(netif->hwaddr, s_StaInfo.au8Dot11MACAddress, MAC_ADDR_LEN);
    
    //ToDo: we need to get mac address through wifi drvier api, before we call lwip_init()
    //      should be set mac address after wifi ready
    wifi_nvm_sta_info_read(WIFI_NVM_STA_INFO_ID_MAC_ADDR, MAC_ADDR_LEN, netif->hwaddr);
    
    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    #if LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
    #endif

    /* Do whatever else is needed to initialize interface. */
}

void lwip_load_interface_wlannetif_patch(void)
{
    low_level_input_adpt = low_level_input_patch;
    ethernetif_init_adpt = ethernetif_init_patch;
    low_level_init_adpt  = low_level_init_patch;
    return;
}

