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
 * @file at_cmd_wifi.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File supports the Wi-Fi module AT Commands.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_wifi.h"
#include "wpa_cli.h"
#include "driver.h"
#include "driver_netlink.h"
#include "at_cmd_common.h"

/** Here for now until needed in other places in lwIP */
#ifndef isprint
#define in_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#endif

/*
 * @brief Check MAC string format
 *
 * @param [in] s MAC string
 *
 * @return 0 fail 1 success
 *
 */
int isMACValid(char *s) {
    int i;
    for(i = 0; i < 17; i++) {
        if(i % 3 != 2 && !isxdigit(s[i]))
            return 0;
        if(i % 3 == 2 && s[i] != ':')
            return 0;
    }
    if(s[17] != '\0')
        return 0;
    return true;
}

/*
 * @brief Command at+cwmode
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwmode(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwjap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwjap(int argc, char *argv[])
{
    wpa_cli_connect_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwlapopt
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwlapopt(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwlap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwlap(int argc, char *argv[])
{
    wpa_cli_showscanresults_handler(argc, argv);
    //msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwqap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwqap(int argc, char *argv[])
{
    wpa_cli_disconnect_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwsap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwsap(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwlif
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwlif(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwdhcp
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwdhcp(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwdhcps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwdhcps(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwautoconn
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwautoconn(int argc, char *argv[])
{
    /** TBD, we can implement it until the file system is ready */

    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cipstamac
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cipstamac(int argc, char *argv[])
{
    wpa_cli_mac_by_param(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cipsta
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cipsta(int argc, char *argv[])
{
    /** TBD, Need LWIP's api to get it */

    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cipap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cipap(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwstartsmart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwstartsmart(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwstopsmart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwstopsmart(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wps(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwscan
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwscan(int argc, char *argv[])
{
    wpa_cli_scan_handler(argc, argv);
    //msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwrec
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwrec(int argc, char *argv[])
{
    wpa_cli_reconnect_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cwssta
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_cwssta(int argc, char *argv[])
{
    /** TBD, we can implement it until the file system is ready */
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiinit
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_start(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiend
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_end(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifirsv
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_rsv(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiver
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifiver(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+tftp_client_put
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tftp_client_put(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+tftp_client_get
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tftp_client_get(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+tftp_client_buf_show
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tftp_client_buf_show(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+at_cmd_wifi_wifistatus
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifistatus(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+getrssi
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_getrssi(int argc, char *argv[])
{
    wpa_cli_getrssi();
    //msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+joinap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_joinap(int argc, char *argv[])
{
    //Step1. Connect to AP

    //Step2. Get IP from DHCP server

    //msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+leaveap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_leaveap(int argc, char *argv[])
{
    wpa_cli_disconnect_handler(argc, argv);
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiscan
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifiscan(int argc, char *argv[])
{
    wpa_cli_scan_handler(argc, argv);
    //msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+startap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_startap(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+stopap
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_stopap(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifigettxpwr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifigettxpwr(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifisettxpwr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifisettxpwr(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifipwrsave
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifipwrsave(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiinit
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifiinit(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifidown
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifidown(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+sntpstart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_sntpstart(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+sntpstop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_sntpstop(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_connect
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifi_p2p_connect(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_disconnect
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_wifi_p2p_disconnect(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_scan
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_p2p_scan(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_go_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_p2p_go_start(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_go_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_p2p_go_stop(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_wps_reg_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_p2p_wps_reg_start(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_p2p_wps_reg_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_p2p_wps_reg_stop(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_easy_setup_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_easy_setup_start(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_easy_setup_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_easy_setup_stop(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+joinap_wps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_joinap_wps(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifiscan_wps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifiscan_wps(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+wifi_ap_wps_reg_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_ap_wps_reg_stop(int argc, char *argv[])
{
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command Sample code to do Wi-Fi test
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_wifi_sample(void)
{
    /** Scan */
    at_cmd_wifi_cwscan(0, NULL);

    /** List APs */
    at_cmd_wifi_cwlap(0, NULL);

    return true;
}

/**
  * @brief AT Command Table for Wi-Fi Module
  *
  */
at_command_t gAtCmdTbl_Wifi[] =
{
    { "at+cwmode",                  at_cmd_wifi_cwmode,       "Wi-Fi mode" },
    { "at+cwjap",                   at_cmd_wifi_cwjap,        "Connect to AP" },
    { "at+cwlapopt",                at_cmd_wifi_cwlapopt,     "Configuration for at+cwlap" },
    { "at+cwlap",                   at_cmd_wifi_cwlap,        "List available APs" },
    { "at+cwqap",                   at_cmd_wifi_cwqap,        "Disconnect from AP" },
    { "at+cwsap",                   at_cmd_wifi_cwsap,        "Configuration for softAP" },
    { "at+cwlif",                   at_cmd_wifi_cwlif,        "Get station's IP which is connected to an AP" },
    { "at+cwdhcp",                  at_cmd_wifi_cwdhcp,       "Configuration for DHCP" },
    { "at+cwdhcps",                 at_cmd_wifi_cwdhcps,      "Set DHCP IP range of softAP to flash" },
    { "at+cwautoconn",              at_cmd_wifi_cwautoconn,   "Connect to AP automatically when power on" },
    { "at+cipstamac",               at_cmd_wifi_cipstamac,    "Set MAC address of station " },
    { "at+cipapmac",                at_cmd_wifi_cipstamac,    "Set MAC address of station " },
    { "at+cipsta",                  at_cmd_wifi_cipsta,       "Set Station IP" },
    { "at+cipap",                   at_cmd_wifi_cipap,        "Set softAP IP" },
    { "at+cwstartsmart",            at_cmd_wifi_cwstartsmart, "Start smart config" },
    { "at+cwstopsmart",             at_cmd_wifi_cwstopsmart,  "Stop smart config" },
    { "at+wps",                     at_cmd_wifi_wps,          "Set WPS" },
    { "at+cwscan",                  at_cmd_wifi_cwscan,       "Wi-Fi Scan" },
    { "at+cwrec",                   at_cmd_wifi_cwrec,        "Reconnect to AP" },
    { "at+cwssta",                  at_cmd_wifi_cwssta,       "Set configuration of station" },
    { "at+wifiinit",                at_cmd_wifi_start,        "Wi-Fi Start" },    //Back Door
    { "at+wifiend",                 at_cmd_wifi_end,          "Wi-Fi End" },      //Back Door
    { "at+wifirsv",                 at_cmd_wifi_rsv,          "Wi-Fi Reserved" }, //Back Door
    { "at+wifiver",                 at_cmd_wifi_wifiver,      "Show Wi-Fi FW version" },
    { "at+tftp_client_put",         at_cmd_tftp_client_put,   "Send data to tftp server" },
    { "at+tftp_client_get",         at_cmd_tftp_client_get,   "Get data from tftp server" },
    { "at+tftp_client_buf_show",    at_cmd_tftp_client_buf_show, "Show buffer data" },
    { "at+wifistatus",              at_cmd_wifi_wifistatus,   "Show AP/STA mode status" },
    { "at+getrssi",                 at_cmd_wifi_getrssi,      "Get RSSI" },
    { "at+joinap",                  at_cmd_wifi_joinap,       "Connect to AP" },
    { "at+leaveap",                 at_cmd_wifi_leaveap,      "Disconnect an AP" },
    { "at+wifiscan",                at_cmd_wifi_wifiscan,     "Do Wi-Fi Scan & show all APs" },
    { "at+startap",                 at_cmd_wifi_startap,      "Start Wi-Fi AP" },
    { "at+stopap",                  at_cmd_wifi_stopap,       "Stop Wi-Fi AP" },
    { "at+wifigettxpwr",            at_cmd_wifi_wifigettxpwr, "Show Wi-Fi TX Power" },
    { "at+wifisettxpwr",            at_cmd_wifi_wifisettxpwr, "Set Wi-Fi TX Power" },
    { "at+wifipwrsave",             at_cmd_wifi_wifipwrsave,  "Wi-Fi module enter Powersave mode" },
    { "at+wifiinit",                at_cmd_wifi_wifiinit,     "Turn on Wi-Fi" },
    { "at+wifidown",                at_cmd_wifi_wifidown,     "Turn off Wi-Fi" },
    { "at+sntpstart",               at_cmd_wifi_sntpstart,    "Start SNTP service" },
    { "at+sntpstop",                at_cmd_wifi_sntpstop,     "Stop SNTP service" },
    { "at+wifi_p2p_connect",        at_cmd_wifi_wifi_p2p_connect,   "connect to P2P Group Owner" },
    { "at+wifi_p2p_disconnect",     at_cmd_wifi_wifi_p2p_disconnect,"P2P disconnect" },
    { "at+wifi_p2p_scan",           at_cmd_wifi_p2p_scan,           "P2P scan" },
    { "at+wifi_p2p_go_start",       at_cmd_wifi_p2p_go_start,       "Start P2P Group Owner" },
    { "at+wifi_p2p_go_stop",        at_cmd_wifi_p2p_go_stop,        "Stop P2P Group Owner" },
    { "at+wifi_p2p_wps_reg_start",  at_cmd_wifi_p2p_wps_reg_start,  "Start P2P Group Owner WPS" },
    { "at+wifi_p2p_wps_reg_stop",   at_cmd_wifi_p2p_wps_reg_stop,   "Stop P2P Group Owner WPS" },
    { "at+wifi_easy_setup_start",   at_cmd_wifi_easy_setup_start,   "" },
    { "at+wifi_easy_setup_stop",    at_cmd_wifi_easy_setup_stop,    "" },
    { "at+joinap_wps",              at_cmd_wifi_joinap_wps,         "Use WPS to connect the remote AP" },
    { "at+wifiscan_wps",            at_cmd_wifiscan_wps,            "Show APs which can support WPS" },
    { "at+wifi_ap_wps_reg_stop",    at_cmd_wifi_ap_wps_reg_stop,    "Show APs which can support WPS" },
    { NULL,                         NULL,                           NULL},
};

/*
 * @brief Global variable g_AtCmdTbl_Wifi_Ptr retention attribute segment
 *
 */
RET_DATA at_command_t *g_AtCmdTbl_Wifi_Ptr;

/*
 * @brief AT Command Interface Initialization for Wi-Fi modules
 *
 */
void at_cmd_wifi_func_init(void)
{
    /** Command Table (Wi-Fi) */
    g_AtCmdTbl_Wifi_Ptr = gAtCmdTbl_Wifi;
}

