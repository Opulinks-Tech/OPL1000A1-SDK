#ifndef __LWIP_HELPER_H__
#define __LWIP_HELPER_H__

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


typedef enum {
    STA_IP_MODE_STATIC   = 0,
    STA_IP_MODE_DHCP     = 1
} sta_ip_mode_t;

typedef enum {
    NETIF_TYPE_LOOPBACK = 0,
    NETIF_TYPE_AP       = 1,
    NETIF_TYPE_STA      = 2
} netif_type_t;

typedef struct {
    ip4_addr_t sta_ip;
    ip4_addr_t sta_mask;
    ip4_addr_t sta_gw;
} lwip_tcpip_config_t;


#define WIFI_MODE_STA_ONLY      (1)
#define WIFI_MODE_AP_ONLY       (2)


/**
  * @brief  network init function. initial wifi and lwip config
  * @param None
  * @retval None
  */

void lwip_network_init(uint8_t opmode);


void lwip_net_start(uint8_t opmode);


void lwip_net_stop(uint8_t opmode);


/**
  * @brief  when wifi and ip ready will return.
  * @param None
  * @retval None
  */
void lwip_net_ready(void);

/**
* @note This api is only for internal use
*/
uint8_t wifi_set_opmode(uint8_t target_mode);

int lwip_get_ip_info(char *ifname);


#ifdef __cplusplus
}
#endif

#endif /* __LWIP_HELPER_H__ */
