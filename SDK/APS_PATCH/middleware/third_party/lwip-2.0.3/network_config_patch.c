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

#include "network_config.h"
#include "network_config_patch.h"
#include "sys_common_ctrl.h"

extern int dhcp_does_arp_check_flag;

int32_t dhcp_config_init_patch(void)
{
    //tcpip_config_dhcp_arp_check_init();
    return (USE_DHCP == 0) ? STA_IP_MODE_STATIC : STA_IP_MODE_DHCP;
}

int32_t tcpip_config_init_patch(lwip_tcpip_config_t *tcpip_config)
{
    /* Static IP assignment */
    ip4addr_aton(STA_IPADDR_DEF, &(tcpip_config->sta_ip));
    ip4addr_aton(STA_NETMASK_DEF, &tcpip_config->sta_mask);
    ip4addr_aton(STA_GATEWAY_DEF, &tcpip_config->sta_gw);

    return 0;
}

void tcpip_config_dhcp_arp_check_init(void)
{
    int dhcp_arp;
    dhcp_arp = get_dhcp_arp_check_from_fim();
    if (dhcp_arp == -1) { //failed
        dhcp_does_arp_check_flag = 1;
    }
    dhcp_does_arp_check_flag = dhcp_arp;
}

/*-------------------------------------------------------------------------------------
 * Interface assignment
 *------------------------------------------------------------------------------------*/
void lwip_load_interface_network_config_patch(void)
{
    dhcp_config_init     =     dhcp_config_init_patch;
    tcpip_config_init    =     tcpip_config_init_patch;
}
