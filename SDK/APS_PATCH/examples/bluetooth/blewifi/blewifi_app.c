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

/**
 * @file blewifi_app.c
 * @author Vincent Chen
 * @date 12 Feb 2018
 * @brief File creates the wifible app task architecture.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "event_loop.h"
#include "wifi_api.h"
#include "wifi_event.h"
#include "wifi_event_handler.h"
#include "lwip_helper.h"
#include "blewifi_app.h"
#include "blewifi_server_app.h"
#include "blewifi_ctrl.h"
#include "blewifi_user_app.h"
#include "blewifi_data.h"
#include "blewifi_api.h"

extern unsigned char g_BleWifiDataBuf[BLEWIFI_CTRL_MSG_MAX_LEN];

int wifi_do_scan(int mode);
int wifi_do_connection(void);

int wifi_indication(int msgType, void *data, int len);
int wifi_ind_scan_res(void);
int wifi_ind_scan_done(void);
int wifi_ind_connection(void *data, int len);
int wifi_ind_disconnection(void *data, int len);

#define WIFI_READY_TIME 2000

void wifi_wait_ready(void)
{
    /* wait a while for system ready */
    osDelay(WIFI_READY_TIME);
}

int wifi_do_scan(int mode)
{
    wifi_scan_config_t scan_config;

    memset(&scan_config, 0, sizeof(scan_config));
    scan_config.scan_type = (wifi_scan_type_t)mode;
    wifi_scan_start(&scan_config, NULL);
    return 0;
}

int wifi_do_connection(void)
{
    wifi_config_t wifi_config = {0};
    wifi_scan_list_t scan_list;
    int i;
    int isMatched = 0;

    memset(&scan_list, 0, sizeof(scan_list));

    /* Read Confguration */
    wifi_get_config(WIFI_MODE_STA, &wifi_config);

    /* Get APs list */
    wifi_scan_get_ap_list(&scan_list);

    /* Search if AP matched */
    for (i=0; i< scan_list.num; i++) {
        if (memcmp(scan_list.ap_record[i].bssid, wifi_config.sta_config.bssid, WIFI_MAC_ADDRESS_LENGTH) == 0)
        {
            isMatched = 1;
            break;
        }
    }

    if(isMatched == 1) {
        /* Wi-Fi Connection */
        wifi_connection_connect(&wifi_config);
    } else {
        /* Scan Again */
        wifi_do_scan(WIFI_SCAN_TYPE_ACTIVE);
    }

    return 0;
}

int wifi_scan_send_report(void)
{
    wifi_scan_info_t *ap_list = NULL;
    blewifi_scan_info_t *blewifi_ap_list = NULL;
    uint16_t apCount = 0;
    int i = 0;

    wifi_scan_get_ap_num(&apCount);

    if (apCount == 0) {
        printf("No AP found\r\n");
        return 0;
    }
    printf("ap num = %d\n", apCount);
    ap_list = (wifi_scan_info_t *)malloc(sizeof(wifi_scan_info_t) * apCount);

    if (!ap_list) {
        printf("malloc fail, ap_list is NULL\r\n");
        return -1;
    }

    wifi_scan_get_ap_records(&apCount, ap_list);

    blewifi_ap_list = (blewifi_scan_info_t *)malloc(sizeof(blewifi_scan_info_t) *apCount);
    if (!blewifi_ap_list) {
        if (ap_list) {
            free(ap_list);
        }
        printf("malloc fail, blewifi_ap_list is NULL\r\n");
        return -1;
    }

    /* build blewifi ap list */
    for (i = 0; i < apCount; ++i)
    {
        memcpy(blewifi_ap_list[i].ssid, ap_list[i].ssid, sizeof(ap_list[i].ssid));
        memcpy(blewifi_ap_list[i].bssid, ap_list[i].bssid, WIFI_MAC_ADDRESS_LENGTH);
        blewifi_ap_list[i].rssi = ap_list[i].rssi;
        blewifi_ap_list[i].auth_mode = ap_list[i].auth_mode;
        blewifi_ap_list[i].ssid_length = strlen((const char *)ap_list[i].ssid);
    }

    /* Send Data to BLE */
    /* Send AP inforamtion individually */
    for (i = 0; i < apCount; ++i)
    {
        blewifi_send_scan_report(1, &blewifi_ap_list[i]);
        osDelay(100);
    }

    free(ap_list);
    free(blewifi_ap_list);
    return 0;
}

int wifi_scan_report_complete(void)
{
    blewifi_send_response(BLEWIFI_RSP_SCAN_END, 0);
    return 0;
}

int wifi_connect_complete(uint8_t *data, int len)
{
    blewifi_send_response(BLEWIFI_RSP_CONNECT, data[0]);
    return 0;
}

int wifi_disconnect_complete(uint8_t *data, int len)
{
    blewifi_send_response(BLEWIFI_RSP_DISCONNECT, data[0]);
    return 0;
}

int wifi_indication(int msgType, void *data, int len)
{
    blewifi_ctrl_msg_send(msgType, data, len);
    return 0;
}

int wifi_ind_scan_res(void)
{
    wifi_indication(BLEWIFI_CTRL_MSG_WIFI_SCAN_RESULTS_SEND, NULL, 0);
    return 0;
}

int wifi_ind_scan_done(void)
{
    wifi_indication(BLEWIFI_CTRL_MSG_WIFI_SCAN_DONE_IND, NULL, 0);
    return 0;
}

int wifi_ind_connection(void *data, int len)
{
    wifi_indication(BLEWIFI_CTRL_MSG_WIFI_CONNECTION_IND, data, len);
    return 0;
}

int wifi_ind_disconnection(void *data, int len)
{
    wifi_indication(BLEWIFI_CTRL_MSG_WIFI_DISCONNECTION_IND, data, len);
    return 0;
}

void wifi_event_loop_ind(uint32_t type)
{
    event_msg_t Msg = {0};

    Msg.event = type;
    Msg.length = 0;
    Msg.param = NULL;
    wifi_event_loop_send(&Msg);
}

int wifi_event_handler_cb(wifi_event_id_t event_id, void *data, uint16_t length)
{
    uint8_t reason = *((uint8_t*)data);

    switch(event_id) {
    case WIFI_EVENT_STA_START:
        printf("\r\nWi-Fi Start \r\n");
        //wifi_wait_ready();

        //blewifi_build_scan_req();
        //wifi_event_loop_ind(WIFI_EVENT_SCAN_COMPLETE);
        //blewifi_build_connect_req();
        //wifi_scan_send_report();
        //wifi_event_loop_ind(WIFI_EVENT_STA_CONNECTED);
        //blewifi_build_disconnect_req();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        lwip_net_start(WIFI_MODE_STA);
        printf("\r\nWi-Fi Connected \r\n");
        wifi_ind_connection(data, length);
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("\r\nWi-Fi Disconnected , reason %d\r\n", reason);
        wifi_ind_disconnection(data, length);
        break;
    case WIFI_EVENT_SCAN_COMPLETE:
        printf("\r\nWi-Fi Scan Done \r\n");
        wifi_ind_scan_done();
        break;
    case WIFI_EVENT_STA_GOT_IP:
        printf("\r\nWi-Fi Got IP \r\n");
        break;
    case WIFI_EVENT_STA_CONNECTION_FAILED:
        printf("\r\nWi-Fi Connected failed, reason %d\r\n", reason);
        //open     - sucess:0, failed:1
        //security - sucess:2, failed:3
        wifi_ind_connection(data, length);
        break;
    default:
        printf("\r\n Unknown Event %d \r\n", event_id);
        break;
    }
    return 0;
}

void blewifi_wifi_init(void)
{
    wifi_init_config_t int_cfg = {.event_handler = (wifi_event_notify_cb_t)&wifi_event_loop_send, .magic = 0x1F2F3F4F};
    //wifi_config_t wifi_config = {0};

    //unsigned char bssid[WIFI_MAC_ADDRESS_LENGTH] = {0x74, 0xda, 0xda, 0xe7, 0x08, 0xf1};

    memset(g_BleWifiDataBuf, 0, sizeof(g_BleWifiDataBuf));

    /* Event Loop Initialization */
    wifi_event_loop_init((wifi_event_cb_t)wifi_event_handler_cb);

    /* Initialize wifi stack and register wifi init complete event handler */
    wifi_init(&int_cfg, NULL);

#if 0
    /* Set user's configuration */
    strcpy((char *)wifi_config.sta_config.ssid, WIFI_SSID);
    strcpy((char *)wifi_config.sta_config.password, WIFI_PASSWORD);
    wifi_config.sta_config.ssid_length = strlen(WIFI_SSID);
    wifi_config.sta_config.password_length = strlen(WIFI_PASSWORD);
    memcpy(wifi_config.sta_config.bssid, bssid, WIFI_MAC_ADDRESS_LENGTH);

    wifi_set_config(WIFI_MODE_STA, &wifi_config);
#endif

    /* Wi-Fi operation start */
    wifi_start();
}

void BleWifiAppInit(void)
{
    /* Wi-Fi Initialization */
    blewifi_wifi_init();

    /* BLE Stack Initialization */
    blewifi_ble_init();

    /* Create blewifi "control" task */
    blewifi_ctrl_task_create();

    /* Create blewifi "user app" task */
    blewifi_user_app_task_create();

    /* Create blewifi "data" task */
    //blewifi_data_task_create();
}

