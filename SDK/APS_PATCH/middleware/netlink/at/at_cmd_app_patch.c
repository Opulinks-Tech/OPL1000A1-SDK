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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "event_loop.h"
#include "wifi_api.h"
#include "wifi_event.h"
#include "wifi_event_handler.h"
#include "lwip_helper.h"
#include "at_cmd_app.h"
#include "at_cmd_common.h"
#include "at_cmd_tcpip.h"
#include "controller_wifi_com.h"
#include "at_cmd_msg_ext.h"

#include "at_cmd_app_patch.h"
#include "at_cmd_msg_ext_patch.h"

typedef int  (*at_wifi_event_handler_cb_fp_t)(wifi_event_id_t event_id, void *data, uint16_t length);

extern volatile bool at_ip_mode;
extern int at_wifi_status;
extern int at_wifi_reason;
extern at_state_type_t mdState;
extern at_wifi_event_handler_cb_fp_t at_wifi_event_handler_cb_adpt;

int at_wifi_event_handler_cb_patch(wifi_event_id_t event_id, void *data, uint16_t length)
{
    int reason = *((int*)data);

    switch(event_id) {
    case WIFI_EVENT_STA_START:
        wifi_auto_connect_start();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        lwip_net_start(WIFI_MODE_STA);
        at_wifi_status = WIFI_EVENT_STA_CONNECTED;
        _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_DONE);
        printf("\r\nWiFi Connect, reason %d\r\n", reason);
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("\r\nWiFi Disconnect, reason %d\r\n", reason);
        if (at_wifi_status == WIFI_EVENT_STA_CONNECTED ||
            at_wifi_status == WIFI_EVENT_STA_GOT_IP) {
            at_msg_ext_wifi_dispatch_connect_reason(true, reason);
        }
        else {
            at_msg_ext_wifi_dispatch_connect_reason(false, reason);
        }
        at_wifi_status = WIFI_EVENT_STA_DISCONNECTED;
        at_wifi_reason = STATION_CONNECT_FAIL;
        mdState = AT_STA_DISCONNECT;
        break;
    case WIFI_EVENT_SCAN_COMPLETE:
        _at_msg_ext_wifi_show_ap_by_filter();
        at_uart1_printf("\r\nOK\r\n");
        break;
    case WIFI_EVENT_STA_GOT_IP:
        mdState = AT_STA_GOT_IP;
        at_wifi_status = WIFI_EVENT_STA_GOT_IP;
        printf("\r\nWiFi Obtained IP!\r\n");
        if (at_ip_mode != 1) {
            at_uart1_printf("WIFI GOT IP\r\n");
            at_uart1_printf("\r\nOK\r\n");
        }
        break;
    case WIFI_EVENT_STA_CONNECTION_FAILED:
        printf("\r\nWiFi Connected failed\r\n");
        at_wifi_reason = STATION_CONNECT_FAIL;
        at_msg_ext_wifi_dispatch_connect_reason(false, reason);
        break;
    default:
        printf("\r\n Unknown Event %d \r\n", event_id);
        break;
    }
    return 0;
}

void at_cmd_app_func_preinit_patch(void)
{
    at_wifi_event_handler_cb_adpt         = at_wifi_event_handler_cb_patch;
}
