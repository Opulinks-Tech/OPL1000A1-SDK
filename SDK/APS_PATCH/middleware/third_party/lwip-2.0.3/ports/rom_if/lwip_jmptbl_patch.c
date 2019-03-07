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

#include "lwip/opt.h"
#include "sys_os_config_patch.h"
#include "lwip/etharp_patch.h"

/* user api related */
extern void lwip_load_interface_socket_patch(void);

/* ipv4 related */
extern void lwip_load_interface_dhcp_patch(void);
/* ipv6 related */


/* common */
extern void lwip_load_interface_tcpip_patch(void);
extern void lwip_load_interface_timeouts_patch(void);
/* network interface */

/* network config */
extern void lwip_load_interface_network_config_patch(void);

/* porting layer */
extern void lwip_load_interface_wlannetif_patch(void);
extern void lwip_load_interface_sys_arch_freertos_patch(void);

/* application && cli */
extern void lwip_load_interface_lwip_helper_patch(void);
extern void lwip_load_interface_cli_patch(void);

/* tcpip if */
extern void lwip_load_interface_tcpip_if_patch(void);

/* tcp */
extern void lwip_load_interface_tcp_patch(void);

void lwip_module_interface_init_patch(void)
{
    lwip_load_interface_timeouts_patch();
    lwip_load_interface_tcpip_patch();
    lwip_load_interface_socket_patch();
    lwip_load_interface_wlannetif_patch();
    lwip_load_interface_network_config_patch();
    lwip_load_interface_lwip_helper_patch();
    lwip_load_interface_cli_patch();
    lwip_load_interface_tcpip_if_patch();
    lwip_load_interface_dhcp_patch();
    lwip_load_interface_sys_arch_freertos_patch();
    lwip_load_interface_etharp_patch();
    lwip_load_interface_tcp_patch();
    
    return;
}
