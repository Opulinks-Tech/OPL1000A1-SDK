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
#if defined(__AT_CMD_SUPPORT__)

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
#include "sys_os_config_patch.h"

typedef void (*at_cmd_wifi_hook_fp_t)(void);
typedef int  (*at_wifi_event_handler_cb_fp_t)(wifi_event_id_t event_id, void *data, uint16_t length);

extern osThreadId at_app_task_id;

extern volatile bool at_ip_mode;
extern int at_wifi_status;
extern int at_wifi_reason;
extern at_state_type_t mdState;
extern at_wifi_event_handler_cb_fp_t at_wifi_event_handler_cb_adpt;

extern RET_DATA at_cmd_wifi_hook_fp_t at_cmd_wifi_hook_adpt;

extern int at_wifi_event_handler_cb(wifi_event_id_t event_id, void *data, uint16_t length);

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
        at_msg_ext_wifi_dispatch_connect_reason(false, reason);
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
        if (at_wifi_status == WIFI_EVENT_STA_CONNECTED ||
            at_wifi_status == WIFI_EVENT_STA_GOT_IP) {
            at_msg_ext_wifi_dispatch_connect_reason(true, reason);
        }
        else {
            at_msg_ext_wifi_dispatch_connect_reason(false, reason);
        }
        at_wifi_reason = STATION_CONNECT_FAIL;
        break;
    default:
        printf("\r\n Unknown Event %d \r\n", event_id);
        break;
    }
    return 0;
}

void at_cmd_wifi_hook_patch(void)
{
    osThreadDef_t task_def;
    wifi_init_config_t int_cfg = {.event_handler = (wifi_event_notify_cb_t)&at_wifi_event_handler_cb, .magic = 0x1F2F3F4F};

    /* Event Loop Initialization */
    wifi_event_loop_init((wifi_event_cb_t)at_wifi_event_handler_cb);

    /* Initialize wifi stack and register wifi init complete event handler */
    wifi_init(&int_cfg, NULL);

    /* Wi-Fi operation start */
    wifi_start();

    /* Create task */
    task_def.name = OS_TASK_NAME_AT_WIFI_APP;
    task_def.stacksize = OS_TASK_STACK_SIZE_AT_WIFI_APP_PATCH;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = at_user_wifi_app_entry;
    
    at_app_task_id = osThreadCreate(&task_def, (void*)NULL);
    if(at_app_task_id == NULL)
    {
        printf("at_wifi_app Task create fail \r\n");
    }
    else
    {
        printf("at_wifi_app Task create successful \r\n");
    }
}

void at_cmd_app_func_preinit_patch(void)
{
    at_wifi_event_handler_cb_adpt         = at_wifi_event_handler_cb_patch;
    at_cmd_wifi_hook_adpt                 = at_cmd_wifi_hook_patch;
}
#endif
