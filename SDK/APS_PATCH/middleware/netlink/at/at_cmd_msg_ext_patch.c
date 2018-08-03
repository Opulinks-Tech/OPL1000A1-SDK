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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include "opl1000.h"
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_common.h"
#include "at_cmd_msg_ext.h"
#include "controller_wifi_com.h"
#include "at_cmd_wifi.h"
#include "wpa_at_if.h"
#include "wifi_api.h"
#include "wifi_types.h"
#include "at_cmd_wifi.h"
#include "common.h"
#include "driver_netlink.h"
#include "cmsis_os.h"
#include "wpa_cli.h"
#include "at_cmd_wifi_patch.h"
#include "at_cmd_msg_ext_patch.h"

extern Boolean isMAC(char *s);
extern char *g_wifi_argv[AT_MAX_CMD_ARGS];
extern int g_wifi_argc;

int at_compare_rssi_patch(const void *a, const void *b)
{
    return ((wifi_scan_info_t *)a)->rssi < ((wifi_scan_info_t *)b)->rssi ? 1 : -1;
}

void at_msg_ext_wifi_scan_sorting_patch(u8 num, wifi_scan_info_t *data)
{
    qsort(data, num, sizeof(data[0]), at_compare_rssi_patch);
}

void at_msg_ext_wifi_scan_by_option(u8 num, wifi_scan_info_t *scan_list)
{
    int mask, i;
    mask = get_sorting_mask();
    
    msg_print_uart1("\r\n");
    
    for (i = 0; i < num; i++) {
        msg_print_uart1("+CWLAP:");
        
        if (mask & AT_WIFI_SHOW_ECN_BIT)
        {
            msg_print_uart1("%d", scan_list[i].auth_mode);
            if ((mask&(~AT_WIFI_SHOW_ECN_BIT))>AT_WIFI_SHOW_ECN_BIT)
                msg_print_uart1(",");
        }

        if (mask & AT_WIFI_SHOW_SSID_BIT)
        {
            msg_print_uart1("%s", scan_list[i].ssid);
            if ((mask&(~AT_WIFI_SHOW_SSID_BIT))>AT_WIFI_SHOW_SSID_BIT)
                msg_print_uart1(",");
        }

        if (mask & AT_WIFI_SHOW_RSSI_BIT)
        {
            msg_print_uart1("%d", scan_list[i].rssi);
            if ((mask&(~AT_WIFI_SHOW_RSSI_BIT))>AT_WIFI_SHOW_RSSI_BIT)
                msg_print_uart1(",");
        }

        if (mask & AT_WIFI_SHOW_MAC_BIT)
        {
            msg_print_uart1("%02x:%02x:%02x:%02x:%02x:%02x",    scan_list[i].bssid[0],
                                                                scan_list[i].bssid[1],
                                                                scan_list[i].bssid[2],
                                                                scan_list[i].bssid[3],
                                                                scan_list[i].bssid[4],
                                                                scan_list[i].bssid[5]);
            if ((mask&(~AT_WIFI_SHOW_MAC_BIT))>AT_WIFI_SHOW_MAC_BIT)
                msg_print_uart1(",");
        }

        if (mask & AT_WIFI_SHOW_CHANNEL_BIT)
        {
            msg_print_uart1("%d", scan_list[i].channel);
        }
        
        msg_print_uart1("\r\n");
    }
}

void at_msg_ext_wifi_show_one_ap_patch(int argc, char *argv[])
{
    wifi_scan_info_t *scan_list = NULL;
    wifi_scan_info_t *scan_sort_list = NULL;
    u8 bssid[6] = {0};
    u8 ssid[MAX_LEN_OF_SSID + 1] = {0};
    u16 hitCount = 0;
    u16 apCount = 0;
    int ch;
    int len_ssid = 0;
    int len_bssid = 0;
    int i,j = 0;
    
    if (argc <= 1) return;

    memset(bssid, 0, 6);
    memset(ssid, 0, MAX_LEN_OF_SSID + 1);

    wifi_scan_get_ap_num(&apCount);
    
    if (apCount == 0) {
        return;
    }
    
    scan_list = (wifi_scan_info_t *)malloc(sizeof(wifi_scan_info_t) * apCount);
    if (!scan_list) {
        printf("malloc fail, scan_list is NULL\r\n");
        goto Done;
    }
    
    wifi_scan_get_ap_records(&apCount, scan_list);
    
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
            len_bssid = MAC_ADDR_LEN;
        }
    }

    if (argc >= 4)
    {
        ch = atoi(argv[3]);
    }

    for(i=0; i<apCount; i++) {
        if (argc >= 2) { //ssid
            if (os_memcmp(ssid, scan_list[i].ssid, len_ssid) != 0) {
                continue;
            }
        }

        if (argc >= 3) { //mac address
            if (os_memcmp(bssid, scan_list[i].bssid, len_bssid) != 0 ) {
                continue;
            }
        }

        if (argc >= 4) { //channel
            if (scan_list[i].channel != ch) {
                continue;
            }
        }
        
        hitCount++;
    }
 
    if (hitCount == 0) goto Done; //prevent all of parameter is null
    scan_sort_list = (wifi_scan_info_t *)malloc(sizeof(wifi_scan_info_t) * hitCount);
    if (!scan_sort_list) {
        printf("malloc fail, scan_list is NULL\r\n");
        goto Done;
    }
    
    for(i=0; i<apCount; i++) {
        if (argc >= 2) { //ssid
            if (os_memcmp(ssid, scan_list[i].ssid, len_ssid) != 0) {
                continue;
            }
        }

        if (argc >= 3) { //mac address
            if (os_memcmp(bssid, scan_list[i].bssid, len_bssid) != 0 ) {
                continue;
            }
        }

        if (argc >= 4) { //channel
            if (scan_list[i].channel != ch) {
                continue;
            }
        }
        memcpy(&scan_sort_list[j], &scan_list[i], sizeof(wifi_scan_info_t));
        j++;
    }
    
    if (is_sorting() == true) {
        at_msg_ext_wifi_scan_sorting_patch(hitCount, scan_sort_list);
    }
    
    at_msg_ext_wifi_scan_by_option(hitCount, scan_sort_list);
    
Done:
    for(i = 0; i < g_wifi_argc; i++)
    {
        if(g_wifi_argv[i])
        {
            free(g_wifi_argv[i]);
            g_wifi_argv[i] = NULL;
        }
    }
    
    free(scan_list);
    free(scan_sort_list);
    scan_list = NULL;
    scan_sort_list = NULL;
}

void at_msg_ext_wifi_show_all_patch(int argc, char *argv[])
{
    wifi_scan_info_t *scan_list = NULL;
    u16 apCount = 0;
    
    wifi_scan_get_ap_num(&apCount);
    
    if (apCount == 0) return;
    
    scan_list = (wifi_scan_info_t *)malloc(sizeof(wifi_scan_info_t) * apCount);
    if (!scan_list) {
        printf("malloc fail, scan_list is NULL\r\n");
        return;
    }
    
    wifi_scan_get_ap_records(&apCount, scan_list);
    
    if (is_sorting() == true) {
        at_msg_ext_wifi_scan_sorting_patch(apCount, scan_list);
    }
    
    at_msg_ext_wifi_scan_by_option(apCount, scan_list);
    
    free(scan_list);
}

void _at_msg_ext_wifi_connect_patch(int cusType, int msg_code)
{
    switch (cusType)
    {
        case AT_MSG_EXT_ESPRESSIF:
            switch(msg_code)
            {
                case ERR_WIFI_CWJAP_DONE:
                    msg_print_uart1("WIFI CONNECTED\r\n");
                    break;
                case ERR_WIFI_CWJAP_TO:
                    msg_print_uart1("\r\n+CWJAP:%d\r\n", ERR_WIFI_CWJAP_TO);
                    msg_print_uart1("\r\nERROR\r\n");
                    break;
                case ERR_WIFI_CWJAP_PWD_INVALID:
                    msg_print_uart1("\r\n+CWJAP:%d\r\n", ERR_WIFI_CWJAP_PWD_INVALID);
                    msg_print_uart1("\r\nERROR\r\n");
                    break;
                case ERR_WIFI_CWJAP_NO_AP:
                    msg_print_uart1("\r\n+CWJAP:%d\r\n", ERR_WIFI_CWJAP_NO_AP);
                    msg_print_uart1("\r\nERROR\r\n");
                    break;
                case ERR_WIFI_CWJAP_FAIL:
                    msg_print_uart1("\r\n+CWJAP:%d\r\n", ERR_WIFI_CWJAP_FAIL);
                    msg_print_uart1("\r\nERROR\r\n");
                    break;
                case ERR_WIFI_CWJAP_FAIL_OTHERS:
                    msg_print_uart1("\r\n+CWJAP:%d\r\n", ERR_WIFI_CWJAP_FAIL_OTHERS);
                    msg_print_uart1("\r\nERROR\r\n");
                    break;
                default:
                    msg_print_uart1("WIFI DISCONNECT\r\n");
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

void at_msg_ext_wifi_dispatch_connect_reason(bool connected, int reason)
{
    switch(reason) {
        case WIFI_REASON_CODE_MIC_FAILURE:
        case WIFI_REASON_CODE_DIFFERENT_INFO_ELEM:
            _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_PWD_INVALID);
            break;
        case WIFI_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT:
        case WIFI_REASON_CODE_GROUP_KEY_UPDATE_TIMEOUT:
            _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_FAIL);
            break;
        case WIFI_REASON_CODE_AUTO_CONNECT_FAILED:
            if (connected) {
                _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_DISCONNECT);
            }
            else {
                _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_FAIL);
            }
            break;
        case WIFI_REASON_CODE_CONNECT_NOT_FOUND:
            _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_NO_AP);
            break;
        case WIFI_REASON_CODE_CONNECT_TIMEOUT:
            _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_TO);
            break;            
        default:
            _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, ERR_WIFI_CWJAP_DISCONNECT);
            break;
    }
}

void at_msg_ext_init_patch(void)
{
    set_sorting(false, AT_WIFI_SHOW_ALL_BIT);
    
    /** Functions */
    at_msg_ext_wifi_show_one_ap = at_msg_ext_wifi_show_one_ap_patch;
    at_msg_ext_wifi_show_all = at_msg_ext_wifi_show_all_patch;
    _at_msg_ext_wifi_connect = _at_msg_ext_wifi_connect_patch;

}
