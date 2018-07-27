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

/* user api related */
extern void lwip_load_interface_socket_patch(void);

/* ipv4 related */
extern void lwip_load_interface_ip4_frag_patch(void);

/* ipv6 related */


/* common */


/* network interface */


/* porting layer */
extern void lwip_load_interface_wlannetif_patch(void);

/* application && cli */
extern void lwip_load_interface_lwip_helper_patch(void);


void lwip_module_interface_init_patch(void)
{
    lwip_load_interface_wlannetif_patch();
    lwip_load_interface_ip4_frag_patch();
    lwip_load_interface_socket_patch();
    lwip_load_interface_lwip_helper_patch();
    return;
}
