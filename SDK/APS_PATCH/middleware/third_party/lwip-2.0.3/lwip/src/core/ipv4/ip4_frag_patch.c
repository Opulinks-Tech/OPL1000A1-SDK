/**
 * @file
 * This is the IPv4 packet segmentation and reassembly implementation.
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
 * Author: Jani Monoses <jani@iv.ro>
 *         Simon Goldschmidt
 * original reassembly code by Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#if LWIP_IPV4

#include "lwip/ip4_frag.h"
#include "lwip/def.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/icmp.h"

#include <string.h>

#if defined(LWIP_ROMBUILD)
#include "ipv4/ip4_frag_if.h"
#endif

#if IP_REASSEMBLY
/**
 * The IP reassembly code currently has the following limitations:
 * - IP header options are not supported
 * - fragments must not overlap (e.g. due to different routes),
 *   currently, overlapping or duplicate fragments are thrown away
 *   if IP_REASS_CHECK_OVERLAP=1 (the default)!
 *
 * @todo: work with IP header options
 */

/** Setting this to 0, you can turn off checking the fragments for overlapping
 * regions. The code gets a little smaller. Only use this if you know that
 * overlapping won't occur on your network! */
#ifndef IP_REASS_CHECK_OVERLAP
#define IP_REASS_CHECK_OVERLAP 1
#endif /* IP_REASS_CHECK_OVERLAP */

/** Set to 0 to prevent freeing the oldest datagram when the reassembly buffer is
 * full (IP_REASS_MAX_PBUFS pbufs are enqueued). The code gets a little smaller.
 * Datagrams will be freed by timeout only. Especially useful when MEMP_NUM_REASSDATA
 * is set to 1, so one datagram can be reassembled at a time, only. */
#ifndef IP_REASS_FREE_OLDEST
#define IP_REASS_FREE_OLDEST 1
#endif /* IP_REASS_FREE_OLDEST */

#define IP_REASS_FLAG_LASTFRAG 0x01

#define IP_REASS_VALIDATE_TELEGRAM_FINISHED  1
#define IP_REASS_VALIDATE_PBUF_QUEUED        0
#define IP_REASS_VALIDATE_PBUF_DROPPED       -1

/** This is a helper struct which holds the starting
 * offset and the ending offset of this fragment to
 * easily chain the fragments.
 * It has the same packing requirements as the IP header, since it replaces
 * the IP header in memory in incoming fragments (after copying it) to keep
 * track of the various fragments. (-> If the IP header doesn't need packing,
 * this struct doesn't need packing, too.)
 */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct ip_reass_helper {
  PACK_STRUCT_FIELD(struct pbuf *next_pbuf);
  PACK_STRUCT_FIELD(u16_t start);
  PACK_STRUCT_FIELD(u16_t end);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define IP_ADDRESSES_AND_ID_MATCH(iphdrA, iphdrB)  \
  (ip4_addr_cmp(&(iphdrA)->src, &(iphdrB)->src) && \
   ip4_addr_cmp(&(iphdrA)->dest, &(iphdrB)->dest) && \
   IPH_ID(iphdrA) == IPH_ID(iphdrB)) ? 1 : 0

/* global variables */
extern LWIP_RETDATA struct ip_reassdata *reassdatagrams;
extern LWIP_RETDATA u16_t ip_reass_pbufcount;

/**
 * Chain a new pbuf into the pbuf list that composes the datagram.  The pbuf list
 * will grow over time as  new pbufs are rx.
 * Also checks that the datagram passes basic continuity checks (if the last
 * fragment was received at least once).
 * @param ipr points to the reassembly state
 * @param new_p points to the pbuf for the current fragment
 * @param is_last is 1 if this pbuf has MF==0 (ipr->flags not updated yet)
 * @return see IP_REASS_VALIDATE_* defines
 */
static int
ip_reass_chain_frag_into_datagram_and_validate_patch(struct ip_reassdata *ipr, struct pbuf *new_p, int is_last)
{
  struct ip_reass_helper *iprh, *iprh_tmp, *iprh_prev=NULL;
  struct pbuf *q;
  u16_t offset, len;
  struct ip_hdr *fraghdr;
  int valid = 1;

  /* Extract length and fragment offset from current fragment */
  fraghdr = (struct ip_hdr*)new_p->payload;
  len = lwip_ntohs(IPH_LEN(fraghdr)) - IPH_HL(fraghdr) * 4;
  offset = (lwip_ntohs(IPH_OFFSET(fraghdr)) & IP_OFFMASK) * 8;

  /* overwrite the fragment's ip header from the pbuf with our helper struct,
   * and setup the embedded helper structure. */
  /* make sure the struct ip_reass_helper fits into the IP header */
  LWIP_ASSERT("sizeof(struct ip_reass_helper) <= IP_HLEN",
              sizeof(struct ip_reass_helper) <= IP_HLEN);
  iprh = (struct ip_reass_helper*)new_p->payload;
  iprh->next_pbuf = NULL;
  iprh->start = offset;
  iprh->end = offset + len;

  /* Iterate through until we either get to the end of the list (append),
   * or we find one with a larger offset (insert). */
  for (q = ipr->p; q != NULL;) {
    iprh_tmp = (struct ip_reass_helper*)q->payload;
    if (iprh->start < iprh_tmp->start) {
      /* the new pbuf should be inserted before this */
      iprh->next_pbuf = q;
      if (iprh_prev != NULL) {
        /* not the fragment with the lowest offset */
#if IP_REASS_CHECK_OVERLAP
        if ((iprh->start < iprh_prev->end) || (iprh->end > iprh_tmp->start)) {
          /* fragment overlaps with previous or following, throw away */
          goto freepbuf;
        }
#endif /* IP_REASS_CHECK_OVERLAP */
        iprh_prev->next_pbuf = new_p;
        if (iprh_prev->end != iprh->start) {
          /* There is a fragment missing between the current
           * and the previous fragment */
          valid = 0;
        }
      } else {
#if IP_REASS_CHECK_OVERLAP
        if (iprh->end > iprh_tmp->start) {
          /* fragment overlaps with following, throw away */
          goto freepbuf;
        }
#endif /* IP_REASS_CHECK_OVERLAP */
        /* fragment with the lowest offset */
        ipr->p = new_p;
      }
      break;
    } else if (iprh->start == iprh_tmp->start) {
      /* received the same datagram twice: no need to keep the datagram */
      goto freepbuf;
#if IP_REASS_CHECK_OVERLAP
    } else if (iprh->start < iprh_tmp->end) {
      /* overlap: no need to keep the new datagram */
      goto freepbuf;
#endif /* IP_REASS_CHECK_OVERLAP */
    } else {
      /* Check if the fragments received so far have no holes. */
      if (iprh_prev != NULL) {
        if (iprh_prev->end != iprh_tmp->start) {
          /* There is a fragment missing between the current
           * and the previous fragment */
          valid = 0;
        }
      }
    }
    q = iprh_tmp->next_pbuf;
    iprh_prev = iprh_tmp;
  }

  /* If q is NULL, then we made it to the end of the list. Determine what to do now */
  if (q == NULL) {
    if (iprh_prev != NULL) {
      /* this is (for now), the fragment with the highest offset:
       * chain it to the last fragment */
#if IP_REASS_CHECK_OVERLAP
      LWIP_ASSERT("check fragments don't overlap", iprh_prev->end <= iprh->start);
#endif /* IP_REASS_CHECK_OVERLAP */
      iprh_prev->next_pbuf = new_p;
      if (iprh_prev->end != iprh->start) {
        valid = 0;
      }
    } else {
#if IP_REASS_CHECK_OVERLAP
      LWIP_ASSERT("no previous fragment, this must be the first fragment!",
        ipr->p == NULL);
#endif /* IP_REASS_CHECK_OVERLAP */
      /* this is the first fragment we ever received for this ip datagram */
      ipr->p = new_p;
    }
  }

  /* At this point, the validation part begins: */
  /* If we already received the last fragment */
  if (is_last || ((ipr->flags & IP_REASS_FLAG_LASTFRAG) != 0)) {
    /* and had no holes so far */
    if (valid) {
      /* then check if the rest of the fragments is here */
      /* Check if the queue starts with the first datagram */
      if ((ipr->p == NULL) || (((struct ip_reass_helper*)ipr->p->payload)->start != 0)) {
        valid = 0;
      } else {
        /* and check that there are no holes after this datagram */
        iprh_prev = iprh;
        q = iprh->next_pbuf;
        while (q != NULL) {
          iprh = (struct ip_reass_helper*)q->payload;
          if (iprh_prev->end != iprh->start) {
            valid = 0;
            break;
          }
          iprh_prev = iprh;
          q = iprh->next_pbuf;
        }
        /* if still valid, all fragments are received
         * (because to the MF==0 already arrived */
        if (valid) {
          LWIP_ASSERT("sanity check", ipr->p != NULL);
          LWIP_ASSERT("sanity check",
            ((struct ip_reass_helper*)ipr->p->payload) != iprh);
          LWIP_ASSERT("validate_datagram:next_pbuf!=NULL",
            iprh->next_pbuf == NULL);
        }
      }
    }
    /* If valid is 0 here, there are some fragments missing in the middle
     * (since MF == 0 has already arrived). Such datagrams simply time out if
     * no more fragments are received... */
    return valid ? IP_REASS_VALIDATE_TELEGRAM_FINISHED : IP_REASS_VALIDATE_PBUF_QUEUED;
  }
  /* If we come here, not all fragments were received, yet! */
  return IP_REASS_VALIDATE_PBUF_QUEUED; /* not yet valid! */
#if IP_REASS_CHECK_OVERLAP
freepbuf:
  #if 0 // new_p will be freed in ip4_reass
  ip_reass_pbufcount -= pbuf_clen(new_p);
  pbuf_free(new_p);
  #endif
  return IP_REASS_VALIDATE_PBUF_DROPPED;
#endif /* IP_REASS_CHECK_OVERLAP */
}

void lwip_load_interface_ip4_frag_patch(void)
{
    ip_reass_chain_frag_into_datagram_and_validate_adpt = ip_reass_chain_frag_into_datagram_and_validate_patch;
    return;
}
#endif /* IP_REASSEMBLY */

#endif /* LWIP_IPV4 */
