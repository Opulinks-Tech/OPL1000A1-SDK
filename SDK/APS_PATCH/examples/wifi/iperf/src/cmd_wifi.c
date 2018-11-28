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
#include <stdio.h>
#include <string.h>
#include "cmd_wifi.h"
#include "console.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "opulinks_log.h"
#include "wifi_api.h"
#include "iperf_cli.h"

extern EventGroupHandle_t wifi_event_group;
extern const int CONNECTED_BIT0;

static const char *TAG="cmd_wifi";

static int wifi_cmd_sta(int argc, char** argv)
{
    LOGI_DRCT(TAG, "connecting to %s", argv[1]);
    
    wifi_config_t wifi_config = {0};
    wifi_get_config(WIFI_MODE_STA, &wifi_config);
    
    memcpy(&wifi_config.sta_config.ssid, argv[1], strlen(argv[1]));
    wifi_config.sta_config.ssid_length = strlen(argv[1]);
    
    memcpy(&wifi_config.sta_config.password, argv[2], strlen(argv[2]));
    wifi_config.sta_config.password_length = strlen(argv[2]);
    
    int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT0, 0, 1, 0);
    if (bits & CONNECTED_BIT0) {
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT0);
    }
    
    wifi_connection_connect(&wifi_config);
    return 0;
}

static int wifi_cmd_scan(int argc, char** argv)
{
    LOGI_DRCT(TAG, "sta scanning");
    
    wifi_scan_config_t scan_config;
    memset(&scan_config, 0, sizeof(scan_config));
    scan_config.scan_type = WIFI_SCAN_TYPE_MIX;
    wifi_scan_start(&scan_config, NULL);
    return 0;
}

static int wifi_cmd_query(int argc, char **argv)
{
    wifi_ap_record_t ap_info = {0};
    
    LOGI_DRCT(TAG, "query wifi states");
    int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT0, 0, 1, 0);
    if (bits & CONNECTED_BIT0) {
        wifi_sta_get_ap_info(&ap_info);
        LOGI_DRCT(TAG, "Wifi Connected, AP SSID=%s", ap_info.ssid);
    }
    else {
        LOGI_DRCT(TAG, "Disconnected");
    }
    
    return 0;
}

static int wifi_cmd_data_rate(int argc, char **argv)
{
    wifi_mac_data_rate_t data_rate;
    LOGI_DRCT(TAG, "set wifi Tx fix data rate");
    
    data_rate = (wifi_mac_data_rate_t)atoi(argv[1]);
    wifi_config_set_mac_tx_data_rate(data_rate);
    
    LOGI_DRCT(TAG, "current setting : %d", data_rate);
    LOGI_DRCT(TAG, "  0:DTO, 1:1M, 2:2M, 3:5.5M, 4:11M");
    
    return 0;
}

static int commands_help(int argc, char** argv)
{
    LOGI_DRCT(TAG, "Wifi Part:");
    LOGI_DRCT(TAG, "  Scan AP    : scan");
    LOGI_DRCT(TAG, "  Connect AP : connect SSID Passphrase");
    LOGI_DRCT(TAG, "  Query  States : query");
    LOGI_DRCT(TAG, "  Set Tx Data rate : wifi_data_rate TYPE");
    LOGI_DRCT(TAG, "Iperf Part:");
    LOGI_DRCT(TAG, "  help  : iperf -h");
    
    return 0;
}

static int cmd_buf_to_arg(int *argc, char **argv, char *pbuf)
{
    int count = 0;
    char *p = strtok(pbuf, " ");
    
    argv[count] = p;
    count++;
    
    while ((p = strtok(NULL, " ")) != NULL) {
        argv[count] = p;
        count++;
    }
    
    *argc = count;
    return 0;
}

int wifi_cmd_func(char *pbuf, int len)
{
    char *argv[10] = {0};
    int argc = 0;
    
    cmd_buf_to_arg(&argc, &argv[0], pbuf);
    
    tracer_drct_printf("\r\n");
    if (!strcmp(argv[0], "connect")) {
        wifi_cmd_sta(argc, argv);
        return CMD_FINISHED;
    }
    else if (!strcmp(argv[0], "scan")) {
        wifi_cmd_scan(argc, argv);
        return CMD_FINISHED;
    }
    else if (!strcmp(argv[0], "iperf")) {
        cmd_iperf(argc, argv);
        return CMD_FINISHED;
    }
    else if (!strcmp(argv[0], "help")) {
        commands_help(argc, argv);
        return CMD_FINISHED;
    }
    else if (!strcmp(argv[0], "query")) {
        wifi_cmd_query(argc, argv);
        return CMD_FINISHED;
    }
    else if (!strcmp(argv[0], "wifi_data_rate")) {
        wifi_cmd_data_rate(argc, argv);
        return CMD_FINISHED;
    }
    
    return CMD_CONTINUE;
}
