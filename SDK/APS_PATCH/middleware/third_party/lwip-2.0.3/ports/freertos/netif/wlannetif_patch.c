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

#if defined(LWIP_ROMBUILD)
#include "port/wlannetif_if.h"
#endif

#include "wlannetif_patch.h"
#include "wifi_nvm.h"
#include "sys_common_ctrl.h"

#define TX_TASK_STACKSIZE           (512)
#ifdef LWIP_DEBUG
#define RX_TASK_STACKSIZE           (512*2)
#else
#define RX_TASK_STACKSIZE           (512)
#endif


struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

extern struct netif netif;
extern sys_sem_t TxReadySem;

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
    u8 type = BASE_NVM_MAC_SRC_TYPE_ID_OTP;
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
    base_mac_addr_src_get_cfg(BASE_NVM_MAC_SRC_IFACE_ID_STA, &type);
    if (type == BASE_NVM_MAC_SRC_TYPE_ID_OTP) {
        //TODO
        //Wait OTP function.
        memcpy(netif->hwaddr, s_StaInfo.au8Dot11MACAddress, MAC_ADDR_LEN);
    }
    else {
        wifi_nvm_sta_info_read(WIFI_NVM_STA_INFO_ID_MAC_ADDR, MAC_ADDR_LEN, netif->hwaddr);
    }
    
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

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output_patch(struct netif *netif, struct pbuf *p)
{
    struct ethernetif *ethernetif = netif->state;
    struct pbuf *q;
    static int full_count = 0;
    static int write_count = 0;
    u16_t len = 0;

    LWIP_UNUSED_ARG(ethernetif);

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif


    for(q = p; q != NULL; q = q->next) {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        #ifdef TX_PKT_DUMP
        dump_buffer(q->payload, q->len, 1);
        #endif

        /* Wait for transmit cleanup task to wakeup */
        if ( TX_QUEUE_FULL == wifi_mac_tx_start(q->payload, q->len) )
        {
            full_count++;
            printf("__packet_tx_task: Tx WriteCount: %d  FullCount:%d\r\n", write_count, full_count);
            sys_arch_sem_wait(&TxReadySem, 1);
            printf("__packet_tx_task: recevie Tx ready event to wakeup\r\n");
            //return ERR_MEM;
        }
        else
        {
            full_count = 0;
            write_count++;
            //printf("__packet_tx_task: Tx WriteCount: %d  FullCount:%d\r\n", write_count, full_count);
        }
        len = len + q->len;
    }

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

void lwip_load_interface_wlannetif_patch(void)
{
    low_level_init_adpt  = low_level_init_patch;
    low_level_output_adpt = low_level_output_patch;
    return;
}

