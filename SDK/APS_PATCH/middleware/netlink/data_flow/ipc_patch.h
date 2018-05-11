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

#ifndef __IPC_PATCH_H__
#define __IPC_PATCH_H__

#include "ipc.h"
#include "wifi_mac_common_patch.h"
#include "ps.h"


// For Wi-Fi BSS Info Extension
#define IPC_WIFI_BSS_INFO_EXT_START     IPC_ADDR_ALIGN(IPC_WIFI_STA_INFO_END, 8)
#define IPC_WIFI_BSS_INFO_EXT_LEN       sizeof(bss_info_ext_t)
#define IPC_WIFI_BSS_INFO_EXT_END       (IPC_WIFI_BSS_INFO_EXT_START +IPC_WIFI_BSS_INFO_EXT_LEN)

// For PS module ps_conf
#define IPC_PS_CONF_START               IPC_ADDR_ALIGN(IPC_WIFI_BSS_INFO_EXT_END, 4)
#define IPC_PS_CONF_LEN                 sizeof(t_ps_conf)
#define IPC_PS_CONF_END                 (IPC_PS_CONF_START + IPC_PS_CONF_LEN)


#define IPC_SHM_AVAIL_ADDR              IPC_PS_CONF_END


void ipc_patch_init(void);
void *ipc_rb_write_patch(void *pRb);
void ipc_proc_patch(void);
int ipc_wifi_aps_rx_handle_patch(uint32_t dwType, uint32_t dwIdx, void *pBuf, uint32_t dwBufSize);

#endif //#ifndef __IPC_PATCH_H__

