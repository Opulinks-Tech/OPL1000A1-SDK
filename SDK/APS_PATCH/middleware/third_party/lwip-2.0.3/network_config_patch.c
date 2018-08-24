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

int32_t tcpip_config_init_patch(lwip_tcpip_config_t *tcpip_config)
{
    /* Static IP assignment */
    ip4addr_aton(STA_IPADDR_DEF, &(tcpip_config->sta_ip));
    ip4addr_aton(STA_NETMASK_DEF, &tcpip_config->sta_mask);
    ip4addr_aton(STA_GATEWAY_DEF, &tcpip_config->sta_gw);

    return 0;
}

/*-------------------------------------------------------------------------------------
 * Interface assignment
 *------------------------------------------------------------------------------------*/
void lwip_load_interface_network_config_patch(void)
{
    tcpip_config_init = tcpip_config_init_patch;
}
