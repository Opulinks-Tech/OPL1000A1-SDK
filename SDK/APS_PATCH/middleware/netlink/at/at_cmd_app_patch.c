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
#include "at_cmd_nvm.h"

#include "at_cmd_app_patch.h"
#include "at_cmd_msg_ext_patch.h"
#include "at_cmd_tcpip_patch.h"
#include "sys_os_config_patch.h"
#include "at_cmd_task_patch.h"

typedef void (*at_cmd_wifi_hook_fp_t)(void);
typedef int  (*at_wifi_event_handler_cb_fp_t)(wifi_event_id_t event_id, void *data, uint16_t length);

RET_DATA at_blewifi_init_fp_t at_blewifi_init_adpt;

extern osThreadId at_app_task_id;

extern volatile bool at_ip_mode;
extern int at_wifi_status;
extern int at_wifi_reason;
extern at_state_type_t mdState;
extern at_wifi_event_handler_cb_fp_t at_wifi_event_handler_cb_adpt;

extern RET_DATA at_cmd_wifi_hook_fp_t at_cmd_wifi_hook_adpt;

extern int at_wifi_event_handler_cb(wifi_event_id_t event_id, void *data, uint16_t length);
extern at_nvm_trans_config_t gTransCfg;

int at_blewifi_init(void)
{
    at_cmd_crlf_term_set(1); // Enable CR-LF termination for AT+CIPSEND
    return at_blewifi_init_adpt();
}

int at_blewifi_init_impl(void)
{
    // Dummy, for patch
    // Success should be return 0
    return -1;
}

int at_wifi_trans_do_scan(void)
{
    wifi_scan_config_t scan_config;
    uint8_t auto_conn_num = 0;
    
    wifi_auto_connect_get_saved_ap_num(&auto_conn_num);
    if (auto_conn_num == 0) {
        return 0;
    }
    
    memset(&scan_config, 0, sizeof(scan_config));
    scan_config.scan_type = WIFI_SCAN_TYPE_MIX;
    
    wifi_scan_start(&scan_config, NULL);
    return 0;
}

void at_wifi_trans_reconnect(void)
{
    uint8_t wifi_auto = false;
    uint8_t auto_conn_num = 0;
    int i, j;
    int is_matched = 0;
    wifi_scan_list_t *p_scan_list = NULL;
    wifi_auto_connect_info_t ac_info = {0};
    
    if (at_ip_mode == false) {
        goto exit;
    }
    
    wifi_auto_connect_get_mode(&wifi_auto);
    if (wifi_auto == false) {
        wifi_auto_connect_set_mode(true);
    }
    
    wifi_auto_connect_get_saved_ap_num(&auto_conn_num);
    if (auto_conn_num == 0) {
        goto exit;
    }
    
    p_scan_list = (wifi_scan_list_t *)malloc(sizeof(wifi_scan_list_t));
    if(p_scan_list == NULL)
    {
        goto exit;
    }
    
    wifi_scan_get_ap_list(p_scan_list);
    
    for (i=0; i<auto_conn_num; i++) {
        wifi_auto_connect_get_ap_info(i, &ac_info);
        for (j=0; j<p_scan_list->num; j++) {
            if (memcmp(p_scan_list->ap_record[j].bssid, ac_info.bssid, WIFI_MAC_ADDRESS_LENGTH) == 0) {
                if (wifi_auto_connect_update_ch(i, p_scan_list->ap_record[j].channel)) {
                    goto exit;
                }
                is_matched = 1;
                break;
            }
        }
    }
    
    if (is_matched) {
        wifi_auto_connect_start();
    } else {
        at_wifi_trans_do_scan();
    }
    
exit:
    if (p_scan_list) {
        free(p_scan_list);
        p_scan_list = NULL;
    }
}

int at_wifi_event_handler_cb_patch(wifi_event_id_t event_id, void *data, uint16_t length)
{
    int reason = *((int*)data);

    switch(event_id) {
    case WIFI_EVENT_STA_START:
        lwip_network_init(WIFI_MODE_STA);
        wifi_auto_connect_start();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        lwip_net_start(WIFI_MODE_STA);
        at_wifi_status = WIFI_EVENT_STA_CONNECTED;
        if (at_ip_mode != true) {
            _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_DONE);
        }
        printf("\r\nWiFi Connect, reason %d\r\n", reason);
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("\r\nWiFi Disconnect, reason %d\r\n", reason);
        lwip_net_stop(WIFI_MODE_STA);
        if (at_ip_mode != true) {
            at_msg_ext_wifi_dispatch_connect_reason(false, reason);
        }
        at_wifi_status = WIFI_EVENT_STA_DISCONNECTED;
        at_wifi_reason = STATION_CONNECT_FAIL;
        mdState = AT_STA_DISCONNECT;
        
        if (at_ip_mode == true) {
            at_wifi_trans_do_scan();
        }
        break;
    case WIFI_EVENT_SCAN_COMPLETE:
        if (at_ip_mode != true) {
            _at_msg_ext_wifi_show_ap_by_filter();
            at_uart1_printf("\r\nOK\r\n");
        } else {
            at_wifi_trans_reconnect();
        }
        break;
    case WIFI_EVENT_STA_GOT_IP:
        mdState = AT_STA_GOT_IP;
        if (at_wifi_status != WIFI_EVENT_STA_GOT_IP) {
            at_wifi_status = WIFI_EVENT_STA_GOT_IP;
            if (at_ip_mode != true) {
                at_uart1_printf("WIFI GOT IP\r\n");
                at_uart1_printf("\r\nOK\r\n");
            }
        }
        printf("\r\nWiFi Obtained IP!\r\n");
        break;
    case WIFI_EVENT_STA_CONNECTION_FAILED:
        printf("\r\nWiFi Connected failed\r\n");
        if (mdState == AT_STA_GOT_IP) {
            lwip_net_stop(WIFI_MODE_STA);
        }
        
        if (at_ip_mode != true) {
            if (at_wifi_status == WIFI_EVENT_STA_CONNECTED ||
                at_wifi_status == WIFI_EVENT_STA_GOT_IP) {
                at_msg_ext_wifi_dispatch_connect_reason(true, reason);
            }
            else {
                at_msg_ext_wifi_dispatch_connect_reason(false, reason);
            }
        }
        at_wifi_status = WIFI_EVENT_STA_DISCONNECTED;
        at_wifi_reason = STATION_CONNECT_FAIL;
        
        if (at_ip_mode == true) {
            at_wifi_trans_do_scan();
        }
        break;
    default:
        printf("\r\n Unknown Event %d \r\n", event_id);
        break;
    }
    return 0;
}

int at_wifi_event_update_status(wifi_event_id_t event_id, void *data, uint16_t length)
{
    switch(event_id) {
    case WIFI_EVENT_STA_START:
        break;
    case WIFI_EVENT_STA_CONNECTED:
        at_wifi_status = WIFI_EVENT_STA_CONNECTED;
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        at_wifi_status = WIFI_EVENT_STA_DISCONNECTED;
        at_wifi_reason = STATION_CONNECT_FAIL;
        mdState = AT_STA_DISCONNECT;
        break;
    case WIFI_EVENT_SCAN_COMPLETE:
        break;
    case WIFI_EVENT_STA_GOT_IP:
        mdState = AT_STA_GOT_IP;
        at_wifi_status = WIFI_EVENT_STA_GOT_IP;
        break;
    case WIFI_EVENT_STA_CONNECTION_FAILED:
        at_wifi_reason = STATION_CONNECT_FAIL;
        break;
    default:
        break;
    }
    return 0;
}

void at_trans_save_link_task(void *args)
{
    if (gTransCfg.enable == true) {
        at_cmd_trans_lock();
        
        //debug use
        printf("Transparent saved config \n");
        printf("enable=%d\n", gTransCfg.enable);
        printf("link_id=%d\n", gTransCfg.link_id);
        printf("link_type=%d\n", gTransCfg.link_type);
        printf("change_mode=%d\n", gTransCfg.change_mode);
        printf("remote_ip=%s\n", gTransCfg.remote_ip);
        printf("remote_port=%d\n", gTransCfg.remote_port);
        printf("local_port=%d\n", gTransCfg.local_port);
        printf("keep_alive=%d\n", gTransCfg.keep_alive);
    }
    
    /* Waiting for connection & got IP from DHCP server */
    lwip_net_ready();
    
    if (gTransCfg.enable == true) {
        while (at_trans_client_init(&gTransCfg) != 0) {
            osDelay(300);
        }
    }
    
    vTaskDelete(NULL);
}

void at_trans_save_link_task_create(void)
{
    osThreadDef_t task_def;
    
    /* Create task */
    task_def.name = OS_TASK_NAME_AT_WIFI_APP;
    task_def.stacksize = OS_TASK_STACK_SIZE_AT_WIFI_APP_PATCH;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = at_trans_save_link_task;
    
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

void at_cmd_wifi_hook_patch(void)
{
    wifi_init_config_t int_cfg = {.event_handler = (wifi_event_notify_cb_t)&at_wifi_event_handler_cb, .magic = 0x1F2F3F4F};

    /* Event Loop Initialization */
    wifi_event_loop_init((wifi_event_cb_t)at_wifi_event_handler_cb);

    /* Initialize wifi stack and register wifi init complete event handler */
    wifi_init(&int_cfg, NULL);

    /* Wi-Fi operation start */
    wifi_start();
    
    at_trans_save_link_task_create();
}

void at_cmd_app_func_preinit_patch(void)
{
    at_wifi_event_handler_cb_adpt         = at_wifi_event_handler_cb_patch;
    at_cmd_wifi_hook_adpt                 = at_cmd_wifi_hook_patch;
    at_blewifi_init_adpt                  = at_blewifi_init_impl;
}
#endif /* #if defined(__AT_CMD_SUPPORT__) */
