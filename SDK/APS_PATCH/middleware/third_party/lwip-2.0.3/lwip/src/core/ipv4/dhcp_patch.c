/**
 * @file
 * Dynamic Host Configuration Protocol client
 *
 * @defgroup dhcp4 DHCPv4
 * @ingroup ip4
 * DHCP (IPv4) related functions
 * This is a DHCP client for the lwIP TCP/IP stack. It aims to conform
 * with RFC 2131 and RFC 2132.
 *
 * @todo:
 * - Support for interfaces other than Ethernet (SLIP, PPP, ...)
 *
 * Options:
 * @ref DHCP_COARSE_TIMER_SECS (recommended 60 which is a minute)
 * @ref DHCP_FINE_TIMER_MSECS (recommended 500 which equals TCP coarse timer)
 *
 * dhcp_start() starts a DHCP client instance which
 * configures the interface by obtaining an IP address lease and maintaining it.
 *
 * Use dhcp_release() to end the lease and use dhcp_stop()
 * to remove the DHCP client.
 *
 * @see netifapi_dhcp4
 */

/*
 * Copyright (c) 2001-2004 Leon Woestenberg <leon.woestenberg@gmx.net>
 * Copyright (c) 2001-2004 Axon Digital Design B.V., The Netherlands.
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
 * The Swedish Institute of Computer Science and Adam Dunkels
 * are specifically granted permission to redistribute this
 * source code.
 *
 * Author: Leon Woestenberg <leon.woestenberg@gmx.net>
 *
 */

#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_DHCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/stats.h"
#include "lwip/mem.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/def.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "lwip/etharp.h"
#include "lwip/prot/dhcp.h"
#include "dhcp_patch.h"

#include <string.h>
#include "msg.h"

#if defined(LWIP_ROMBUILD)
#include "ipv4/dhcp_if.h"
#endif

/** DHCP_OPTION_MAX_MSG_SIZE is set to the MTU
 * MTU is checked to be big enough in dhcp_start */
#define DHCP_MAX_MSG_LEN(netif)        (netif->mtu)
#define DHCP_MAX_MSG_LEN_MIN_REQUIRED  576

extern dhcp_set_state_fp_t dhcp_set_state_adpt;
extern dhcp_option_long_fp_t dhcp_option_long_adpt;
extern struct udp_pcb *dhcp_pcb;

static u8_t dhcp_discover_request_options_patch[] = {
    DHCP_OPTION_SUBNET_MASK,
    DHCP_OPTION_ROUTER,
    DHCP_OPTION_BROADCAST,
    DHCP_OPTION_DNS_SERVER
  
/**add options for support more router**/
   ,DHCP_OPTION_DOMAIN_NAME, 
    DHCP_OPTION_NB_TINS, 
    DHCP_OPTION_NB_TINT, 
    DHCP_OPTION_NB_TIS, 
    DHCP_OPTION_PRD, 
    DHCP_OPTION_STATIC_ROUTER, 
    DHCP_OPTION_CLASSLESS_STATIC_ROUTER, 
    DHCP_OPTION_VSN 


#if LWIP_DHCP_GET_NTP_SRV
  , DHCP_OPTION_NTP
#endif /* LWIP_DHCP_GET_NTP_SRV */
  };
  
/**
 * Start the DHCP process, discover a DHCP server.
 *
 * @param netif the netif under DHCP control
 */
static err_t
dhcp_discover_patch(struct netif *netif)
{
  struct dhcp *dhcp = netif_dhcp_data(netif);
  err_t result = ERR_OK;
  u16_t msecs;
  u8_t i;
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover()\n"));
  ip4_addr_set_any(&dhcp->offered_ip_addr);
  dhcp_set_state_adpt(dhcp, DHCP_STATE_SELECTING);
  /* create and initialize the DHCP message header */
  result = dhcp_create_msg(netif, dhcp, DHCP_DISCOVER);
  if (result == ERR_OK) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover: making request\n"));

    dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
    dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));

    dhcp_option(dhcp, DHCP_OPTION_PARAMETER_REQUEST_LIST, LWIP_ARRAYSIZE(dhcp_discover_request_options_patch));
    for (i = 0; i < LWIP_ARRAYSIZE(dhcp_discover_request_options_patch); i++) {
      dhcp_option_byte(dhcp, dhcp_discover_request_options_patch[i]);
    }
    dhcp_option_trailer(dhcp);

    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover: realloc()ing\n"));
    pbuf_realloc(dhcp->p_out, sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN + dhcp->options_out_len);

    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover: sendto(DISCOVER, IP_ADDR_BROADCAST, DHCP_SERVER_PORT)\n"));
    udp_sendto_if_src(dhcp_pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT, netif, IP4_ADDR_ANY);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover: deleting()ing\n"));
    dhcp_delete_msg(dhcp);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_discover: SELECTING\n"));
  } else {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp_discover: could not allocate DHCP request\n"));
  }
  if (dhcp->tries < 255) {
    dhcp->tries++;
  }
#if LWIP_DHCP_AUTOIP_COOP
  if (dhcp->tries >= LWIP_DHCP_AUTOIP_COOP_TRIES && dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_OFF) {
    dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_ON;
    autoip_start(netif);
  }
#endif /* LWIP_DHCP_AUTOIP_COOP */
  msecs = (dhcp->tries < 6 ? 1 << dhcp->tries : 60) * 1000;
  dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_discover(): set request timeout %"U16_F" msecs\n", msecs));
  return result;
}

/**
 * Select a DHCP server offer out of all offers.
 *
 * Simply select the first offer received.
 *
 * @param netif the netif under DHCP control
 * @return lwIP specific error (see error.h)
 */
static err_t
dhcp_select_patch(struct netif *netif)
{
  struct dhcp *dhcp = netif_dhcp_data(netif);
  err_t result;
  u16_t msecs;
  u8_t i;
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_select(netif=%p) %c%c%"U16_F"\n", (void*)netif, netif->name[0], netif->name[1], (u16_t)netif->num));
  dhcp_set_state_adpt(dhcp, DHCP_STATE_REQUESTING);

  /* create and initialize the DHCP message header */
  result = dhcp_create_msg(netif, dhcp, DHCP_REQUEST);
  if (result == ERR_OK) {
    dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
    dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));

    /* MUST request the offered IP address */
    dhcp_option(dhcp, DHCP_OPTION_REQUESTED_IP, 4);
    dhcp_option_long_adpt(dhcp, lwip_ntohl(ip4_addr_get_u32(&dhcp->offered_ip_addr)));

    dhcp_option(dhcp, DHCP_OPTION_SERVER_ID, 4);
    dhcp_option_long_adpt(dhcp, lwip_ntohl(ip4_addr_get_u32(ip_2_ip4(&dhcp->server_ip_addr))));

    dhcp_option(dhcp, DHCP_OPTION_PARAMETER_REQUEST_LIST, LWIP_ARRAYSIZE(dhcp_discover_request_options_patch));
    for (i = 0; i < LWIP_ARRAYSIZE(dhcp_discover_request_options_patch); i++) {
      dhcp_option_byte(dhcp, dhcp_discover_request_options_patch[i]);
    }

#if LWIP_NETIF_HOSTNAME
    dhcp_option_hostname(dhcp, netif);
#endif /* LWIP_NETIF_HOSTNAME */

    dhcp_option_trailer(dhcp);
    /* shrink the pbuf to the actual content length */
    pbuf_realloc(dhcp->p_out, sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN + dhcp->options_out_len);

    /* send broadcast to any DHCP server */
    udp_sendto_if_src(dhcp_pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT, netif, IP4_ADDR_ANY);
    dhcp_delete_msg(dhcp);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_select: REQUESTING\n"));
  } else {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("dhcp_select: could not allocate DHCP request\n"));
  }
  if (dhcp->tries < 255) {
    dhcp->tries++;
  }
  msecs = (dhcp->tries < 6 ? 1 << dhcp->tries : 60) * 1000;
  dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_STATE, ("dhcp_select(): set request timeout %"U16_F" msecs\n", msecs));
  return result;
}


void lwip_load_interface_dhcp_patch(void)
{
	dhcp_discover_adpt           = dhcp_discover_patch;
	dhcp_select_adpt           = dhcp_select_patch;	
}
#endif
