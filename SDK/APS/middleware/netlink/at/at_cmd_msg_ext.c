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
 * @file at_cmd_msg_ext.c
 * @author Michael Liao
 * @date 14 Dec 2017
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
#include <ctype.h>
#include "nl1000.h"
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_common.h"
#include "at_cmd_msg_ext.h"
#include "controller_wifi_com.h"

/*
 * @brief Display the extend message of "Wi-Fi Scan" for different customer's AT CMD
 *
 * @param [in] cusType specify which customer
 *
 * @param [in] result scan results
 *
 */
void at_msg_ext_wifi_scan_impl(int cusType, scan_report_t *result)
{
    unsigned char bssid[6] = {0};
    char ssid[IEEE80211_MAX_SSID_LEN + 1] = {0};
    int apNum = 0;
    int i, j;
    int rate = 0;

    switch (cusType)
    {
    	case AT_MSG_EXT_AMPEC:
            /**
                      Msg Format-
                        <CR><LF>Response message<NUL><CR><LF>
                        Response message- # Type BSSID RSSI Rate Chan Security SSID
                          Ex, "0 Infra 00:0F:66:88:88:02 -55 54.0 11 WPA TKIP Cloud
                                1 Infra 10:6F:3F:6D:6E:04 -73 144.4 10 WPA2 AES WL6
                                2 Infra BC:EE:7B:E7:00:30 -46 72.2 1 WPA2 Mixed ASUS_K"
                    */
            os_memset(bssid, 0, sizeof(bssid));
            os_memset(ssid, 0, (IEEE80211_MAX_SSID_LEN + 1));

            apNum = (unsigned int) result->uScanApNum;
            if (apNum == 0) {
                msg_print_uart1("\r\n\r\n");
                break;
            }

            for (i = 0; i < apNum; i++) {
                os_memcpy(bssid, result->pScanInfo[i].bssid, sizeof(bssid));
                os_memcpy(ssid, result->pScanInfo[i].ssid, sizeof(ssid));
                rate = (result->pScanInfo[i].supported_rates[0] << 24 |
					    result->pScanInfo[i].supported_rates[1] << 16 |
						result->pScanInfo[i].supported_rates[2] <<  8 |
						result->pScanInfo[i].supported_rates[3]);
                msg_print_uart1("\r\n%d ", i);
                msg_print_uart1(" N/A "); /** Not Ready on MAC layer */
                msg_print_uart1("%02x:%02x:%02x:%02x:%02x:%02x ", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
                msg_print_uart1("%d ", result->pScanInfo[i].rssi);
                msg_print_uart1("%d ", rate);
                msg_print_uart1("%d ", result->pScanInfo[i].ap_channel);
                msg_print_uart1(" N/A "); /** Not Ready on MAC layer */
                for (j = 0; j <= IEEE80211_MAX_SSID_LEN; j++)
                {
                    msg_print_uart1("%c", ssid[j]);
                }
            }

            msg_print_uart1("\r\n");

    		break;
        case AT_MSG_EXT_ESPRESSIF:
    		break;
    	default:
    		break;
	}
}

/*
 * @brief Display the extend message of "Wi-Fi Connect" for different customer's AT CMD
 *
 */
void at_msg_ext_wifi_connect_impl(int cusType)
{
    switch (cusType)
    {
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
        case AT_MSG_EXT_ESPRESSIF:
    		break;
    	default:
    		break;
	}
}

/*
 * @brief Display the extend message of "Wi-Fi Disconnect" for different customer's AT CMD
 *
 */
void at_msg_ext_wifi_disconnect_impl(int cusType)
{
    switch (cusType)
    {
    	case AT_MSG_EXT_AMPEC:
            /**
                      Msg Format-
                        <CR><LF>OK<CR><LF>
                    */
            msg_print_uart1("\r\nOK\r\n");
    		break;
        case AT_MSG_EXT_ESPRESSIF:
    		break;
    	default:
    		break;
	}
}

/*
 * @brief Display the extend message of "Wi-Fi Get RSSI" for different customer's AT CMD
 *
 */
void at_msg_ext_wifi_get_rssi_impl(int cusType, int rssi)
{
    switch (cusType)
    {
    	case AT_MSG_EXT_AMPEC:
            /**
                      Msg Format-
                        <CR><LF>Response message<NUL><CR><LF>
                        Response message- RSSI is -40
                    */
            msg_print_uart1("\r\nRSSI is %d\r\n", rssi);
    		break;
        case AT_MSG_EXT_ESPRESSIF:
    		break;
    	default:
    		break;
	}
}

/*
 * @brief An external Function at_msg_ext_wifi_scan prototype declaration retention attribute segment
 *
 */
RET_DATA at_msg_ext_wifi_scan_fp_t at_msg_ext_wifi_scan;

/*
 * @brief An external Function at_msg_ext_wifi_connect prototype declaration retention attribute segment
 *
 */
RET_DATA at_msg_ext_wifi_connect_fp_t at_msg_ext_wifi_connect;

/*
 * @brief An external Function at_msg_ext_wifi_disconnect prototype declaration retention attribute segment
 *
 */
RET_DATA at_msg_ext_wifi_disconnect_fp_t at_msg_ext_wifi_disconnect;

/*
 * @brief An external Function at_msg_ext_wifi_get_rssi prototype declaration retention attribute segment
 *
 */
RET_DATA at_msg_ext_wifi_get_rssi_fp_t at_msg_ext_wifi_get_rssi;

/*
 * @brief AT Command extending message interface Initialization
 *
 */
void at_msg_ext_init(void)
{
    /** Functions */
    at_msg_ext_wifi_scan = at_msg_ext_wifi_scan_impl;
    at_msg_ext_wifi_connect = at_msg_ext_wifi_connect_impl;
    at_msg_ext_wifi_disconnect = at_msg_ext_wifi_disconnect_impl;
    at_msg_ext_wifi_get_rssi = at_msg_ext_wifi_get_rssi_impl;
}

