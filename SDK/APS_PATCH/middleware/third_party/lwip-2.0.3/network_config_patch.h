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

#ifndef _NETWORK_CONFIG_PATCH_H_
#define _NETWORK_CONFIG_PATCH_H_

#define STA_IPADDR_DEF  ("0.0.0.0")
#define STA_NETMASK_DEF ("0.0.0.0")
#define STA_GATEWAY_DEF ("0.0.0.0")

void lwip_load_interface_network_config_patch(void);

#endif /* _NETWORK_CONFIG_PATCH_H_ */
