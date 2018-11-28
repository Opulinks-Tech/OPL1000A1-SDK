/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Opulinks Technology Ltd. (C) 2018
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "cmd_wifi.h"
#include "console.h"
#include "event_loop.h"
#include "event_groups.h"
#include "lwip_helper.h"
#include "iperf_example_main.h"
#include "sys_common_api.h"
#include "wifi_api.h"
#include "wifi_types.h"
#include "wifi_event.h"
#include "wifi_event_handler.h"
#include "opulinks_log.h"

/********************************************
Declaration of Global Variables & Functions
********************************************/
EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT0    = 0x01;
const int DISCONNECTED_BIT1 = 0x02;

/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
osThreadId app_task_id;
static const char *TAG="iperf";


static int wifi_scan_result_handler(void)
{
    uint16_t sta_number = 0;
    wifi_scan_info_t *scan_list = NULL;
    int i;
    
    wifi_scan_get_ap_num(&sta_number);
    if (sta_number == 0) {
        return 0;
    }
    
    scan_list = malloc(sta_number * sizeof(wifi_scan_list_t));
    if (scan_list == NULL) {
        LOGE(TAG, "Failed to malloc buffer scan results");
        return -1;
    }
    
    if (wifi_scan_get_ap_records(&sta_number, scan_list) == 0) {
        for (i=0; i<sta_number; i++) {
            LOGI(TAG, "SSID=%22.22s, Channel=%2d, RSSI=%2d", scan_list[i].ssid, scan_list[i].channel, scan_list[i].rssi);
        }
    }
    
    free(scan_list);
    scan_list = NULL;
    
    return 0;
}

static int wifi_event_handler_cb(wifi_event_id_t event_id, void *data, uint16_t length)
{
    switch(event_id) {
    case WIFI_EVENT_STA_CONNECTED:
        lwip_net_start(WIFI_MODE_STA);
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT0);
        xEventGroupSetBits(wifi_event_group, DISCONNECTED_BIT1);
        break;
    case WIFI_EVENT_SCAN_COMPLETE:
        printf("Wi-Fi Scan Done \r\n");
        wifi_scan_result_handler();
        break;
    case WIFI_EVENT_STA_GOT_IP:
        printf("Wi-Fi Got IP \r\n");
        xEventGroupClearBits(wifi_event_group, DISCONNECTED_BIT1);
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT0);
        lwip_get_ip_info("st1");
        break;
    case WIFI_EVENT_STA_CONNECTION_FAILED:
        printf("Wi-Fi Connected failed\r\n");
        break;
    default:
        printf("Unknown Event %d \r\n", event_id);
        break;
    }
    return 0;
}

static int console_init(void)
{
    console_cmd_register(&wifi_cmd_func);
    return 0;
}

void initial_wifi_config(void)
{
    wifi_event_group = xEventGroupCreate();
    wifi_auto_connect_set_mode(0); //Disable Auto Connect
}

void initial_network(void *args)
{
    /* Tcpip stack and net interface initialization,  dhcp client process initialization. */
    lwip_network_init(WIFI_MODE_STA);

    /* Waiting for connection & got IP from DHCP server */
    lwip_net_ready();

    osDelay(500);
    
    vTaskDelete(NULL);
}

void creat_task_to_init_network(void)
{
    osThreadDef_t task_def;
    
    /* Create task */
    task_def.name = "user_app";
    task_def.stacksize = OS_TASK_STACK_SIZE_APP;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = initial_network;
    app_task_id = osThreadCreate(&task_def, (void*)NULL);

    if(app_task_id == NULL)
    {
        LOGE(TAG, "user_app Task create fail \r\n");
    }
    else
    {
        printf("user_app Task create successful \r\n");
    }
}

void AppInit(void)
{
    wifi_init_config_t int_cfg = {.event_handler = (wifi_event_notify_cb_t)&wifi_event_loop_send, .magic = 0x1F2F3F4F};
    
    /* Event Loop Initialization */
    wifi_event_loop_init((wifi_event_cb_t)wifi_event_handler_cb);

    /* Initialize wifi stack and register wifi init complete event handler */
    wifi_init(&int_cfg, NULL);
    
    /* Initial Wifi Setting for this example */
    initial_wifi_config();
    
    /* Initial Network process */
    creat_task_to_init_network();
    
    /* Initial/Register commands */
    console_init();
    
    printf("\n ===========================================\n");
    printf(" |     Start to test WiFi throughput       |\n");
    printf(" |  1. Print 'help' to get command list    |\n");
    printf(" |  2. Setup WiFi connection               |\n");
    printf(" |  3. Run iperf to test throughput        |\n");
    printf(" ===========================================\n");
}
