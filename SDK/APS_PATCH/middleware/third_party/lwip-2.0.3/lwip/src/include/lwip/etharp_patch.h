#ifndef LWIP_HDR_NETIF_ETHARP_PATCH_H
#define LWIP_HDR_NETIF_ETHARP_PATCH_H


#include "lwip/etharp.h"


#ifdef __cplusplus
extern "C" {
#endif


void lwip_load_interface_etharp_patch(void);

int lwip_auto_arp_enable(uint8_t enable, uint32_t timeout);
int lwip_one_shot_arp_enable(void);


#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_NETIF_ETHARP_PATCH_H */

