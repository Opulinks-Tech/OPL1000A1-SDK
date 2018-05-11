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
#include "wpa2_station_app.h"


osThreadId app_task_id;
#define WIFI_READY_TIME 2000

void uart_printf(E_UartIdx_t port,char data[])
{
    uint8_t index = 0;

    while(data[index] != 0)
    {
        Hal_Uart_DataSend(port,data[index]);
        index++;
    }
}

int wifi_event_sta_connected_handler(wifi_event_t event, uint8_t *data, uint32_t length)
{
    return wifi_station_connected_event_handler(data);
}

int wifi_event_sta_disconnected_handler(wifi_event_t event, uint8_t *data, uint32_t length)
{
    return wifi_station_disconnected_event_handler(data);
}

void wifi_wait_ready(void)
{
    /* wait a while for system ready */
    osDelay(WIFI_READY_TIME);
}

int wifi_do_scan(int mode)
{
    wifi_scan_config_t scan_config;
    memset(&scan_config, 0, sizeof(scan_config));
    wifi_scan_start(&scan_config, NULL);
    return 0;
}

int wifi_connection(void)
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

        if (memcmp(scan_list.ap_record[i].ssid, wifi_config.sta_config.ssid, wifi_config.sta_config.ssid_length) == 0)
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

int wifi_event_handler_cb(wifi_event_id_t event_id, void *data, uint16_t length)
{
    switch(event_id) {
    case WIFI_EVENT_STA_START:
        printf("\r\nWi-Fi Start \r\n");
        wifi_wait_ready();
        wifi_do_scan(WIFI_SCAN_TYPE_ACTIVE);
        break;
    case WIFI_EVENT_STA_CONNECTED:
        lwip_net_start(WIFI_MODE_STA);
        printf("\r\nWi-Fi Connected \r\n");

        uart_printf(UART_IDX_0,"UART0：OPL1000 connected! \r\n");
        Hal_Vic_GpioOutput(GPIO_IDX_04,1);
    
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("\r\nWi-Fi Disconnected \r\n");
        wifi_do_scan(WIFI_SCAN_TYPE_ACTIVE);

        uart_printf(UART_IDX_0,"UART0：OPL1000 disconnected! \r\n");
        Hal_Vic_GpioOutput(GPIO_IDX_04,0);
    
        break;
    case WIFI_EVENT_SCAN_COMPLETE:
        printf("\r\nWi-Fi Scan Done \r\n");
        wifi_connection();
        break;
    case WIFI_EVENT_STA_GOT_IP:
        printf("\r\nWi-Fi Got IP \r\n");
        lwip_get_ip_info("st1");
        break;
    case WIFI_EVENT_STA_CONNECTION_FAILED:
        printf("\r\nWi-Fi Connected failed\r\n");
        wifi_do_scan(WIFI_SCAN_TYPE_ACTIVE);
        break;
    default:
        printf("\r\n Unknown Event %d \r\n", event_id);
        break;
    }
    return 0;
}

static void user_wifi_app_entry(void *args)
{
    /* Tcpip stack and net interface initialization,  dhcp client process initialization. */
    lwip_network_init(WIFI_MODE_STA);

    /* Waiting for connection & got IP from DHCP server */
    lwip_net_ready();

    while (1) {
        //lwip_get_ip_info("st1");
        osDelay(2000);
    }
}

void WifiAppInit(void)
{
    osThreadDef_t task_def;
    wifi_init_config_t int_cfg = {.event_handler = (wifi_event_notify_cb_t)&wifi_event_loop_send, .magic = 0x1F2F3F4F};
    wifi_config_t wifi_config = {0};

    unsigned char bssid[WIFI_MAC_ADDRESS_LENGTH] = {0x74, 0xda, 0xda, 0xe7, 0x08, 0xf1};
    
    printf("wifi example \r\n");
    
    /* Register internal wifi event default handler */
    wifi_register_event_handler(WIFI_EVENT_STA_CONNECTED, (wifi_event_handler_t)wifi_event_sta_connected_handler);
    wifi_register_event_handler(WIFI_EVENT_STA_DISCONNECTED, (wifi_event_handler_t)wifi_event_sta_disconnected_handler);

    /* Event Loop Initialization */
    wifi_event_loop_init((wifi_event_cb_t)wifi_event_handler_cb);

    /* Initialize wifi stack and register wifi init complete event handler */
    wifi_init(&int_cfg, NULL);

    /* Set user's configuration */
    strcpy((char *)wifi_config.sta_config.ssid, WIFI_SSID);
    strcpy((char *)wifi_config.sta_config.password, WIFI_PASSWORD);
    wifi_config.sta_config.ssid_length = strlen(WIFI_SSID);
    wifi_config.sta_config.password_length = strlen(WIFI_PASSWORD);
    memcpy(wifi_config.sta_config.bssid, bssid, WIFI_MAC_ADDRESS_LENGTH);

    wifi_set_config(WIFI_MODE_STA, &wifi_config);

    /* Wi-Fi operation start */
    wifi_start();

    /* Create task */
    task_def.name = "user_app";
    task_def.stacksize = OS_TASK_STACK_SIZE_APP;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = user_wifi_app_entry;
    app_task_id = osThreadCreate(&task_def, (void*)NULL);
    if(app_task_id == NULL)
    {
        printf("user_app Task create fail \r\n");
    }
    else
    {
        printf("user_app Task create successful \r\n");
    }
    
}


