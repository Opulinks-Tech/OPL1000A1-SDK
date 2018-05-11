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
 * @file at_cmd_msg_ext_patch.c
 * @author Michael Liao
 * @date 20 Mar 2018
 * @brief File supports the different customerized AT Command's output messages.
 *
 */

/*
 * @brief Support all customers's AT CMD's output message
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include "nl1000.h"
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_common.h"
#include "at_cmd_msg_ext.h"
#include "controller_wifi_com.h"
#include "at_cmd_wifi.h"
#include "at_cmd_msg_ext_patch.h"
#include "wpa_at_if.h"
#include "wifi_api.h"
#include "wifi_types.h"
#include "at_cmd_wifi_patch.h"
#include "common.h"
#include "driver_netlink.h"
#include "cmsis_os.h"
#include "wpa_cli.h"

extern Boolean isMAC(char *s);
extern char *g_wifi_argv[AT_MAX_CMD_ARGS];
extern int g_wifi_argc;

int at_compare_rssi(const void *a, const void *b)
{
    return ((wifi_scan_info_t *)a)->rssi > ((wifi_scan_info_t *)b)->rssi ? 1 : -1;
}

void at_msg_ext_wifi_sorting_patch(wifi_scan_list_t *data)
{
    qsort(data->ap_record, data->num, sizeof(data->ap_record[0]), at_compare_rssi);
}

void at_msg_ext_wifi_scan_patch(int cusType, scan_report_t *result)
{
    int i;
    int sorting;
    static wifi_scan_list_t scan_list;
    static wifi_scan_list_t scan_list_sorting;
    int mask;

    memset(&scan_list, 0, sizeof(scan_list));
    memset(&scan_list_sorting, 0, sizeof(scan_list_sorting));

    wifi_scan_get_ap_list(&scan_list);
    if (scan_list.num <= 0) return;

    mask = get_sorting_mask();

    switch (cusType)
    {
    	case AT_MSG_EXT_ESPRESSIF:
            sorting = is_sorting();
            if(sorting)
            {
                memcpy(&scan_list_sorting, &scan_list, sizeof(scan_list_sorting));
                at_msg_ext_wifi_sorting_patch(&scan_list_sorting);

                msg_print_uart1("\r\n");
                for (i = 0; i < scan_list_sorting.num; i++)
                {
#if 0
                    msg_print_uart1("+CWLAP:%d,%s,%d,%02x:%02x:%02x:%02x:%02x:%02x,%d\r\n",
                                                             scan_list_sorting.ap_record[i].auth_mode,
                                                             scan_list_sorting.ap_record[i].ssid,
                                                             scan_list_sorting.ap_record[i].rssi,
                                                             scan_list_sorting.ap_record[i].bssid[0],
                                                             scan_list_sorting.ap_record[i].bssid[1],
                                                             scan_list_sorting.ap_record[i].bssid[2],
                                                             scan_list_sorting.ap_record[i].bssid[3],
                                                             scan_list_sorting.ap_record[i].bssid[4],
                                                             scan_list_sorting.ap_record[i].bssid[5],
                                                             scan_list_sorting.ap_record[i].channel);
#else
                    msg_print_uart1("+CWLAP:");
                    if (mask & AT_WIFI_SHOW_ECN_BIT)
                    {
                        msg_print_uart1("%d,", scan_list_sorting.ap_record[i].auth_mode);
                    }
                    else
                    {
                        msg_print_uart1(",");
                    }
                    if (mask & AT_WIFI_SHOW_SSID_BIT)
                    {
                        msg_print_uart1("%s,", scan_list_sorting.ap_record[i].ssid);
                    }
                    else
                    {
                        msg_print_uart1(",");
                    }
                    if (mask & AT_WIFI_SHOW_RSSI_BIT)
                    {
                        msg_print_uart1("%d,", scan_list_sorting.ap_record[i].rssi);
                    }
                    else
                    {
                        msg_print_uart1(",");
                    }
                    if (mask & AT_WIFI_SHOW_MAC_BIT)
                    {
                        msg_print_uart1("%02x:%02x:%02x:%02x:%02x:%02x,",   scan_list_sorting.ap_record[i].bssid[0],
                                                                            scan_list_sorting.ap_record[i].bssid[1],
                                                                            scan_list_sorting.ap_record[i].bssid[2],
                                                                            scan_list_sorting.ap_record[i].bssid[3],
                                                                            scan_list_sorting.ap_record[i].bssid[4],
                                                                            scan_list_sorting.ap_record[i].bssid[5]);
                    }
                    else
                    {
                        msg_print_uart1(",");
                    }
                    if (mask & AT_WIFI_SHOW_CHANNEL_BIT)
                    {
                        msg_print_uart1("%d", scan_list_sorting.ap_record[i].channel);
                    }
                    msg_print_uart1("\r\n");
#endif
                }
            }
            else
            {
                msg_print_uart1("\r\n");
                for (i = 0; i < scan_list.num; i++)
                {
                    msg_print_uart1("+CWLAP:%d,%s,%d,%02x:%02x:%02x:%02x:%02x:%02x,%d\r\n",
                                                             scan_list.ap_record[i].auth_mode,
                                                             scan_list.ap_record[i].ssid,
                                                             scan_list.ap_record[i].rssi,
                                                             scan_list.ap_record[i].bssid[0],
                                                             scan_list.ap_record[i].bssid[1],
                                                             scan_list.ap_record[i].bssid[2],
                                                             scan_list.ap_record[i].bssid[3],
                                                             scan_list.ap_record[i].bssid[4],
                                                             scan_list.ap_record[i].bssid[5],
                                                             scan_list.ap_record[i].channel);
                }
            }
    		break;

        case AT_MSG_EXT_AMPEC:
    		break;

    	default:
    		break;
	}
}

/*
 * @brief Display the extend message of "Wi-Fi Connect" for different customer's AT CMD
 *
 */
void at_msg_ext_wifi_connect_patch(int cusType)
{
    switch (cusType)
    {
        case AT_MSG_EXT_ESPRESSIF:
    		break;
    	case AT_MSG_EXT_AMPEC:
            /**
                      Msg Format-
                        <CR><LF>Response message<NUL><CR><LF>
                        Response message- Joining : 001D737474EC
                                                   Successfully joined : 001D737474EC
                                                   Obtaining IP address via DHCP
                                                   Network ready IP: 192.168.22.151
                    */
    		break;
    	default:
    		break;
	}
}

/*
 * @brief Display the extend message of "Wi-Fi Disconnect" for different customer's AT CMD
 *
 */
void at_msg_ext_wifi_disconnect_patch(int cusType)
{
    switch (cusType)
    {
    	case AT_MSG_EXT_ESPRESSIF:
    		break;
        case AT_MSG_EXT_AMPEC:
             /**
                      Msg Format-
                        <CR><LF>OK<CR><LF>
                    */
            msg_print_uart1("\r\nOK\r\n");
    		break;
    	default:
    		break;
	}
}

/*
 * @brief Display the extend message of "Wi-Fi Get RSSI" for different customer's AT CMD
 *
 */
void at_msg_ext_wifi_get_rssi_patch(int cusType, int rssi)
{
    switch (cusType)
    {
        case AT_MSG_EXT_ESPRESSIF:
    		break;
    	case AT_MSG_EXT_AMPEC:
            /**
                      Msg Format-
                        <CR><LF>Response message<NUL><CR><LF>
                        Response message- RSSI is -40
                    */
            msg_print_uart1("\r\nRSSI is %d\r\n", rssi);
    		break;
    	default:
    		break;
	}
}

void at_msg_ext_wifi_err_patch(int cusType, char *cmd_str, int error_code)
{
    switch (cusType)
    {
        case AT_MSG_EXT_ESPRESSIF: //cmd_str => "+CWJAP"
            msg_print_uart1("\r\n%s:%d\r\n", cmd_str, error_code);
            msg_print_uart1("\r\nERROR\r\n");
    		break;
    	case AT_MSG_EXT_AMPEC:
    		break;
    	default:
    		break;
	}
}

void _at_msg_ext_wifi_connect_patch(int cusType, int msg_code)
{
    switch (cusType)
    {
        case AT_MSG_EXT_ESPRESSIF:
            switch(msg_code)
            {
                case MSG_WIFI_CONNECTED_OPEN:
                    msg_print_uart1("\r\nOK\r\n");
                    msg_print_uart1("\r\nWIFI CONNECTED\r\n");
                    break;
                case MSG_WIFI_CONNECTED_SECURITY:
                    msg_print_uart1("\r\nOK\r\n");
                    msg_print_uart1("\r\nWIFI CONNECTED\r\n");
                    break;
                case MSG_WIFI_DISCONNECTED:
                    msg_print_uart1("\r\nWIFI DISCONNECT\r\n");
                    break;
                default:
                    break;
            }
            break;
    	case AT_MSG_EXT_AMPEC:
            /**
                      Msg Format-
                        <CR><LF>Response message<NUL><CR><LF>
                        Response message- Joining : 001D737474EC
                                                   Successfully joined : 001D737474EC
                                                   Obtaining IP address via DHCP
                                                   Network ready IP: 192.168.22.151
                    */
    		break;
    	default:
    		break;
	}
}

void _at_msg_ext_wifi_show_one_ap_patch(int argc, char *argv[])
{
    scan_report_t *scan_repo = NULL;
    u8 bssid[6] = {0};
    u8 ssid[MAX_LEN_OF_SSID + 1] = {0};
    u16 auth_mode;
    int ch;
    int len_ssid = 0;
    int i;
    
    if (argc <= 1) return;

    memset(bssid, 0, 6);
    memset(ssid, 0, MAX_LEN_OF_SSID + 1);

    scan_repo = wifi_get_scan_result();

    if (scan_repo->uScanApNum == 0) {
        return;
    }
    
    if (argc >= 2)
    {
        len_ssid = strlen(argv[1]);
        
        if(len_ssid > MAX_LEN_OF_SSID)
        {
            len_ssid = MAX_LEN_OF_SSID;
        }
        memcpy(ssid, argv[1], len_ssid);
    }

    if (argc >= 3)
    {
        if(isMAC(argv[2]))
        {
            hwaddr_aton2(argv[2], bssid);
        }
    }

    if (argc >= 4)
    {
        ch = atoi(argv[3]);
    }

    for(i=0; i<scan_repo->uScanApNum; i++) {
        if (argc >= 2) { //ssid
            if (os_memcmp(ssid, scan_repo->pScanInfo[i].ssid, len_ssid) != 0) {
                continue;
            }
        }

        if (argc >= 3) { //mac address
            if (os_memcmp(bssid, scan_repo->pScanInfo[i].bssid, WIFI_MAC_ADDRESS_LENGTH) != 0 ) {
                continue;
            }
        }

        if (argc >= 4) { //channel
            if (scan_repo->pScanInfo[i].ap_channel != ch) {
                continue;
            }
        }
        
        if (scan_repo->pScanInfo[i].capabilities & 0x000000010) {
            switch (scan_repo->pScanInfo[i].wpa_data.proto)
            {
                case WPA_PROTO_WPA:
                    auth_mode = WIFI_AUTH_WPA_PSK;
                    break;
                case WPA_PROTO_RSN:
                    auth_mode = WIFI_AUTH_WPA2_PSK;
                    break;
                default:
                    break;
            }
        }
        else {
            auth_mode = 0;
        }
        msg_print_uart1("+CWLAP:%d,%s,%d,%02x:%02x:%02x:%02x:%02x:%02x,%d\r\n",
                         auth_mode,
                         scan_repo->pScanInfo[i].ssid,
                         scan_repo->pScanInfo[i].rssi,
                         scan_repo->pScanInfo[i].bssid[0],
                         scan_repo->pScanInfo[i].bssid[1],
                         scan_repo->pScanInfo[i].bssid[2],
                         scan_repo->pScanInfo[i].bssid[3],
                         scan_repo->pScanInfo[i].bssid[4],
                         scan_repo->pScanInfo[i].bssid[5],
                         scan_repo->pScanInfo[i].ap_channel);

    }
 
    for(i = 0; i < g_wifi_argc; i++)
    {
        if(g_wifi_argv[i])
        {
            free(g_wifi_argv[i]);
            g_wifi_argv[i] = NULL;
        }
    }
}

void _at_msg_ext_wifi_show_all_patch(int argc, char *argv[])
{
    wifi_scan_info_t *scan_list = NULL;
    u16 apCount = 0;
    int i;
    
    
    wifi_scan_get_ap_num(&apCount);
    
    scan_list = (wifi_scan_info_t *)malloc(sizeof(wifi_scan_info_t) * apCount);
    if (!scan_list) {
        printf("malloc fail, scan_list is NULL\r\n");
        return;
    }
    
    wifi_scan_get_ap_records(&apCount, scan_list);
    
    if (apCount > 0)
    {
        for (i=0; i<apCount; i++) {
            msg_print_uart1("+CWLAP:%d,%s,%d,%02x:%02x:%02x:%02x:%02x:%02x,%d\r\n",
                                                     scan_list[i].auth_mode,
                                                     scan_list[i].ssid,
                                                     scan_list[i].rssi,
                                                     scan_list[i].bssid[0],
                                                     scan_list[i].bssid[1],
                                                     scan_list[i].bssid[2],
                                                     scan_list[i].bssid[3],
                                                     scan_list[i].bssid[4],
                                                     scan_list[i].bssid[5],
                                                     scan_list[i].channel);
        }

        //msg_print_uart1("\r\nOK\r\n");
    }
    
    free(scan_list);
}

void _at_msg_ext_wifi_show_ap_by_filter_patch(void)
{
    if (g_wifi_argc == 1) {
        _at_msg_ext_wifi_show_all_patch(NULL, NULL);
        return;
    }
    else {
        _at_msg_ext_wifi_show_one_ap_patch(g_wifi_argc, g_wifi_argv);
    }
    
}

/*
 * @brief An external Function at_msg_ext_wifi_err prototype declaration retention attribute segment
 *
 */
RET_DATA at_msg_ext_wifi_err_fp_t at_msg_ext_wifi_err;

/*
 * @brief An external Function _at_msg_ext_wifi_connect prototype declaration retention attribute segment
 *
 */
RET_DATA _at_msg_ext_wifi_connect_fp_t _at_msg_ext_wifi_connect;

/*
 * @brief An external Function scan_report_sorting prototype declaration retention attribute segment
 *
 */
RET_DATA at_msg_ext_wifi_sorting_fp_t at_msg_ext_wifi_sorting;

void _at_msg_ext_init(void)
{
    at_msg_ext_wifi_scan = at_msg_ext_wifi_scan_patch;
    at_msg_ext_wifi_connect = at_msg_ext_wifi_connect_patch;
    at_msg_ext_wifi_disconnect = at_msg_ext_wifi_disconnect_patch;
    at_msg_ext_wifi_get_rssi = at_msg_ext_wifi_get_rssi_patch;
    at_msg_ext_wifi_err = at_msg_ext_wifi_err_patch;
    _at_msg_ext_wifi_connect = _at_msg_ext_wifi_connect_patch;
    at_msg_ext_wifi_sorting = at_msg_ext_wifi_sorting_patch;
}

