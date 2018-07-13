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

#ifndef __WIFI_MAC_TASK_PATCH_H__
#define __WIFI_MAC_TASK_PATCH_H__

#define WIFI_MAC_RX_REASM_TIMEOUT       0x0003

void wifi_mac_rx_reasm_timeout_handle(void *pParam);

#endif //#ifndef __WIFI_MAC_TASK_PATCH_H__

