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
 * @file at_cmd_tcpip.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File supports the TCP/IP module AT Commands.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_tcpip.h"
#include "lwip_helper.h"
#include "at_cmd_common.h"

/*
 * @brief Command at+at_cmd_tcpip_cipstatus
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipstatus(int argc, char *argv[])
{
    lwip_get_ip_info("st1");
    printf("\r\nOK\r\n");
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cipdomain
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipdomain(int argc, char *argv[])
{
    lwip_get_ip_info("st1");
    printf("\r\nOK\r\n");
    msg_print_uart1("\r\nOK\r\n");
    return true;
}

/*
 * @brief Command at+cipstart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipstart(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipsend
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipsend(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipsendEX
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipsendex(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipclose
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipclose(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cifsr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cifsr(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipmux
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipmux(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipserver
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipserver(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipmode
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipmode(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+savetranslink
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_savetranslink(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipsto
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipsto(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ciupdate
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_ciupdate(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipdinfo
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipdinfo(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipsntpcfg
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipsntpcfg(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+cipsntptime
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_cipsntptime(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ping
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_ping(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcpipstart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+at_cmd_tcpip_end
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_end(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcpiprsv
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_rsv(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+at_cmd_tcpip_httpdevconf_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_httpdevconf_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+at_cmd_tcpip_httpdevconf_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_httpdevconf_stop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcp
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcp(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcpm
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpm(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcplist
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcplist(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcpleave
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpleave(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcps(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcpslist
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpslist(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcpsdis
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpsdis(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcpsstop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpsstop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+tcpsm
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpsm(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+udps
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_udps(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+udpsstop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_udpsstop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+udp
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_udp(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mqttsubscribe_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_mqttsubscribe_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mqttsubscribe_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_mqttsubscribe_stop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mqttpublish
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_mqttpublish(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mqttshowlist
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_mqttshowlist(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_svr_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_svr_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_svr_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_svr_stop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_svr_add_service
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_svr_add_service(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_svr_delete_service
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_svr_delete_service(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_svr_show_service
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_svr_show_service(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_client_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_client_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_client_get
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_client_stop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_client_post
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_client_get(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_client_put
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_client_put(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_client_delete
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_client_delete(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_client_observe
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_client_observe(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_client_observe_cancel
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_client_observe_cancel(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+coap_client_observe_show
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_coap_client_observe_show(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+sendemail
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_sendemail(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+http_sendreq
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_http_sendreq(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+https_sendreq
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_https_sendreq(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+iperf
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_iperf(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mdns_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_mdns_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mdns_add_service
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_mdns_add_service(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mdns_del_service
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_mdns_del_service(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+mdns_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_mdns_stop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ssdp_start
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_ssdp_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ssdp_stop
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_ssdp_stop(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ssdp_show_notice
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_ssdp_show_notice(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+ssdp_send_msearch
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_ssdp_send_msearch(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+show_dns_info
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_show_dns_info(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+set_dns_svr
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_set_dns_svr(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command Sample code to do TCP/IP test
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_tcpip_sample(void)
{
    return true;
}

/**
  * @brief AT Command Table for TCP/IP Module
  *
  */
at_command_t gAtCmdTbl_Tcpip[] =
{
    { "at+cipstatus",        at_cmd_tcpip_cipstatus,   "Get connection status" },
    { "at+cipdomain",        at_cmd_tcpip_cipdomain,   "DNS domain function" },
    { "at+cipstart",         at_cmd_tcpip_cipstart,    "Establish TCP connection, UDP transmission" },
    { "at+cipsend",          at_cmd_tcpip_cipsend,     "Send data" },
    { "at+cipsendEX",        at_cmd_tcpip_cipsendex,   "Send data till \0" },
    { "at+cipclose",         at_cmd_tcpip_cipclose,    "Close TCP/UDP" },
    { "at+cifsr",            at_cmd_tcpip_cifsr,       "Get local IP address" },
    { "at+cipmux",           at_cmd_tcpip_cipmux,      "Set multi-connection mode" },
    { "at+cipserver",        at_cmd_tcpip_cipserver,   "Set TCP server" },
    { "at+cipmode",          at_cmd_tcpip_cipmode,     "Set transmission mode" },
    { "at+savetranslink",    at_cmd_tcpip_savetranslink, "Save to flash" },
    { "at+cipsto",           at_cmd_tcpip_cipsto,      "Set timeout of TCP server" },
    { "at+ciupdate",         at_cmd_tcpip_ciupdate,    "Update Wi-Fi software" },
    { "at+cipdinfo",         at_cmd_tcpip_cipdinfo,    "When receive data, +IPD remote IP/port" },
    { "at+cipsntpcfg",       at_cmd_tcpip_cipsntpcfg,  "Configuration for SNTP server" },
    { "at+cipsntptime",      at_cmd_tcpip_cipsntptime, "Query SNTP time" },
    { "at+ping",             at_cmd_tcpip_ping,        "Function PING" },
    { "at+tcpipstart",       at_cmd_tcpip_start,       "TCP/IP Start" },    //Back Door
    { "at+tcpipend",         at_cmd_tcpip_end,         "TCP/IP End" },      //Back Door
    { "at+tcpiprsv",         at_cmd_tcpip_rsv,         "TCP/IP Reserved" }, //Back Door
    { "at+httpdevconf_start",at_cmd_tcpip_httpdevconf_start, "Start HTTP Server to do data configuration" },
    { "at+httpdevconf_stop", at_cmd_tcpip_httpdevconf_stop,  "Stop data configuration" },
    { "at+tcp",              at_cmd_tcp,               "Connect to TCP server" },
    { "at+tcpm",             at_cmd_tcpm,              "Send data to TCP server" },
    { "at+tcplist",          at_cmd_tcplist,           "Show all connections of the remote TCP server" },
    { "at+tcpleave",         at_cmd_tcpleave,          "Disconnect the TCP connection" },
    { "at+tcps",             at_cmd_tcps,              "Start TCP server" },
    { "at+tcpslist",         at_cmd_tcpslist,          "Show all connections of the local TCP server" },
    { "at+tcpsdis",          at_cmd_tcpsdis,           "Disconnect a specified TCP connection of the local TCP server" },
    { "at+tcpsstop",         at_cmd_tcpsstop,          "Stop local TCP server" },
    { "at+tcpsm",            at_cmd_tcpsm,             "Send data to remote TCP client" },
    { "at+udps",             at_cmd_udps,              "Start local UDP server" },
    { "at+udpsstop",         at_cmd_udpsstop,          "Stop local UDP server" },
    { "at+udp",              at_cmd_udp,               "Send data to remote UDP client" },
    { "at+mqttsubscribe_start",         at_cmd_tcpip_mqttsubscribe_start,       "Set topic by MQTT subscribe" },
    { "at+mqttsubscribe_stop",          at_cmd_tcpip_mqttsubscribe_stop,        "Stop MQTT subscribe" },
    { "at+mqttpublish",                 at_cmd_tcpip_mqttpublish,               "Send message by MQTT publish on specified topic" },
    { "at+mqttshowlist",                at_cmd_tcpip_mqttshowlist,              "Show all topics of local MQTT subscribe" },
    { "at+coap_svr_start",              at_cmd_tcpip_coap_svr_start,            "Start CoAP server" },
    { "at+coap_svr_stop",               at_cmd_tcpip_coap_svr_stop,             "Stop CoAP server" },
    { "at+coap_svr_add_service",        at_cmd_tcpip_coap_svr_add_service,      "Add service on CoAP server" },
    { "at+coap_svr_delete_service",     at_cmd_tcpip_coap_svr_delete_service,   "Delete service on CoAP server" },
    { "at+coap_svr_show_service",       at_cmd_tcpip_coap_svr_show_service,     "Show service on CoAP server" },
    { "at+coap_client_start",           at_cmd_tcpip_coap_client_start,         "Connect a CoAP server" },
    { "at+coap_client_stop",            at_cmd_tcpip_coap_client_start,         "Stop a CoAP server" },
    { "at+coap_client_get",             at_cmd_tcpip_coap_client_stop,          "Do GET function to CoAP server" },
    { "at+coap_client_post",            at_cmd_tcpip_coap_client_get,           "Do POST function to CoAP server" },
    { "at+coap_client_put",             at_cmd_tcpip_coap_client_put,           "Do PUT function to CoAP server" },
    { "at+coap_client_delete",          at_cmd_tcpip_coap_client_delete,        "Do DELETE function to CoAP server" },
    { "at+coap_client_observe",         at_cmd_tcpip_coap_client_observe,       "Do OBSERVE function to CoAP server" },
    { "at+coap_client_observe_cancel",  at_cmd_tcpip_coap_client_observe_cancel,"Do OBSERVE Cancel function to CoAP server" },
    { "at+coap_client_observe_show",    at_cmd_tcpip_coap_client_observe_show,  "Show all URI services" },
    { "at+sendemail",                   at_cmd_tcpip_sendemail,                 "Send Email" },
    { "at+http_sendreq",                at_cmd_tcpip_http_sendreq,              "Send HTTP request to web server" },
    { "at+https_sendreq",               at_cmd_tcpip_https_sendreq,             "Send HTTPS request to web server" },
    { "at+iperf",                       at_cmd_tcpip_iperf,                     "Run IPERF" },
    { "at+mdns_start",                  at_cmd_tcpip_mdns_start,                "Enable mDNS" },
    { "at+mdns_add_service",            at_cmd_tcpip_mdns_add_service,          "Add mDNS service" },
    { "at+mdns_del_service",            at_cmd_tcpip_mdns_del_service,          "Delete mDNS service" },
    { "at+mdns_stop",                   at_cmd_tcpip_mdns_stop,                 "Disable mDNS" },
    { "at+ssdp_start",                  at_cmd_tcpip_ssdp_start,                "Open SSDP" },
    { "at+ssdp_stop",                   at_cmd_tcpip_ssdp_stop,                 "Close SSDP" },
    { "at+ssdp_show_notice",            at_cmd_tcpip_ssdp_show_notice,          "Specify if show notification message" },
    { "at+ssdp_send_msearch",           at_cmd_tcpip_ssdp_send_msearch,         "Send M-Search request" },
    { "at+show_dns_info",               at_cmd_tcpip_show_dns_info,             "Show DNS server information" },
    { "at+set_dns_svr",                 at_cmd_tcpip_set_dns_svr,               "Set DNS server" },
    { NULL,                             NULL,                                   NULL},
};

/*
 * @brief Global variable g_AtCmdTbl_Tcpip_Ptr retention attribute segment
 *
 */
RET_DATA at_command_t *g_AtCmdTbl_Tcpip_Ptr;

/*
 * @brief AT Command Interface Initialization for TCP/IP modules
 *
 */
void at_cmd_tcpip_func_init(void)
{
    /** Command Table (TCP/IP) */
    g_AtCmdTbl_Tcpip_Ptr = gAtCmdTbl_Tcpip;
}

