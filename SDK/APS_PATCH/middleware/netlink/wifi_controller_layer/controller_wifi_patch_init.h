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

#ifndef __CONTROLLER_WIFI_PATCH_INIT_H__
#define __CONTROLLER_WIFI_PATCH_INIT_H__

/**
 * @brief This defines Wi-Fi event handler. Call #wifi_connection_register_event_handler() 
 * to register a handler, then the Wi-Fi driver generates an event and sends it to the handler.
 */
void wifi_ctrl_patch_init(void);

#endif  //__CONTROLLER_WIFI_PATCH_INIT_H__
