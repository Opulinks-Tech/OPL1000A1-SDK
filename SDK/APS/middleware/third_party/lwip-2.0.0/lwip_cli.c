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
/* standard library */
#include <stdio.h>
#include <string.h>

/* LwIP headers */
#include "lwip/opt.h"
#include "lwip/dhcp.h"
#include "lwip/stats.h"
#include "lwip/netif.h"
#include "cli.h"

#include "lwip_cli.h"
#include "lwip_helper.h"
/**
 * Consumes one parameter. If there is no more parameter, exits as fail.
 */
#define CONSUME_OR_FAIL                                                     \
    do {                                                                    \
        len--;                                                              \
        if (len == 0) {                                                     \
            printf("missing parameter\n");                                  \
            return 1;                                                       \
        }                                                                   \
        param++;                                                            \
    } while (0)


/**
 * Consumes one parameter. If there is no more parameter, exits as fail.
 */
#define CONSUME_AND_RECURSIVE(ifname, iface, len, param)                    \
    do {                                                                    \
        len--;                                                              \
        param++;                                                            \
        if (len > 0 && _cli_ip_cmds(ifname, iface, len, param)) {           \
            return 1;                                                       \
        }                                                                   \
    } while (0)


/**
 * Macro to perform string comparison to make the code looks better.
 */
#define PARAM_IS(str)   (!strcmp(str, param[0]))


/****************************************************************************
 *
 * Types.
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Static variables.
 *
 ****************************************************************************/


unsigned int lwip_debug_log = 0;

#if defined(LWIP_DYNAMIC_DEBUG_ENABLE)
struct lwip_debug_flags lwip_debug_flags[] =
{
	[LWIP_DEBUG_IDX(ETHARP_DEBUG)]     = { "ETHARP_DEBUG",     LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(NETIF_DEBUG)]      = { "NETIF_DEBUG",      LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(PBUF_DEBUG)]       = { "PBUF_DEBUG",       LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(API_LIB_DEBUG)]    = { "API_LIB_DEBUG",    LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(API_MSG_DEBUG)]    = { "API_MSG_DEBUG",    LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(SOCKETS_DEBUG)]    = { "SOCKETS_DEBUG",    LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(ICMP_DEBUG)]       = { "ICMP_DEBUG",       LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(IGMP_DEBUG)]       = { "IGMP_DEBUG",       LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(INET_DEBUG)]       = { "INET_DEBUG",       LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(IP_DEBUG)]         = { "IP_DEBUG",         LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(IP_REASS_DEBUG)]   = { "IP_REASS_DEBUG",   LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(RAW_DEBUG)]        = { "RAW_DEBUG",        LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(MEM_DEBUG)]        = { "MEM_DEBUG",        LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(MEMP_DEBUG)]       = { "MEMP_DEBUG",       LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(SYS_DEBUG)]        = { "SYS_DEBUG",        LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TIMERS_DEBUG)]     = { "TIMERS_DEBUG",     LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_DEBUG)]        = { "TCP_DEBUG",        LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_INPUT_DEBUG)]  = { "TCP_INPUT_DEBUG",  LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_FR_DEBUG)]     = { "TCP_FR_DEBUG",     LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_RTO_DEBUG)]    = { "TCP_RTO_DEBUG",    LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_CWND_DEBUG)]   = { "TCP_CWND_DEBUG",   LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_WND_DEBUG)]    = { "TCP_WND_DEBUG",    LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_OUTPUT_DEBUG)] = { "TCP_OUTPUT_DEBUG", LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_RST_DEBUG)]    = { "TCP_RST_DEBUG",    LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCP_QLEN_DEBUG)]   = { "TCP_QLEN_DEBUG",   LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(UDP_DEBUG)]        = { "UDP_DEBUG",        LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(TCPIP_DEBUG)]      = { "TCPIP_DEBUG",      LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(PPP_DEBUG)]        = { "PPP_DEBUG",        LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(SLIP_DEBUG)]       = { "SLIP_DEBUG",       LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(DHCP_DEBUG)]       = { "DHCP_DEBUG",       LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(AUTOIP_DEBUG)]     = { "AUTOIP_DEBUG",     LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(SNMP_MSG_DEBUG)]   = { "SNMP_MSG_DEBUG",   LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(SNMP_MIB_DEBUG)]   = { "SNMP_MIB_DEBUG",   LWIP_DBG_OFF },
	[LWIP_DEBUG_IDX(DNS_DEBUG)]        = { "DNS_DEBUG",        LWIP_DBG_OFF },
	{ NULL, 0 }
};
#endif

/****************************************************************************
 *
 * Local functions.
 *
 ****************************************************************************/


static int _show_lwip_stat(int len, char *param[])
{
#if LWIP_STATS_DISPLAY
    uint32_t    stats_config = 0;

    if (atoi(param[0]) == 0) {
        printf("1:    TCP\n");
        printf("2:    UDP\n");
        printf("4:    ICMP\n");
        printf("8:    IGMP\n");
        printf("16:   IPFRAG\n");
        printf("32:   IP\n");
        printf("64:   ETHARP\n");
        printf("128:  LINK\n");
        printf("256:  MEM\n");
        printf("512:  MEMP\n");
        printf("1024: SYS\n");
        printf("2048: PBUF\n");
    }

    stats_config = atoi(param[0]);
#if TCP_STATS
    if (stats_config & (1 << 0))
        TCP_STATS_DISPLAY();
#else
    if (stats_config & (1 << 0))
        printf("Please open the TCP_STATS Macro before compile!!!\n");
#endif

#if UDP_STATS
    if (stats_config & (1 << 1))
        UDP_STATS_DISPLAY();
#else
    if (stats_config & (1 << 1))
        printf("Please open the UDP_STATS Macro before compile!!!\n");
#endif

#if ICMP_STATS
    if (stats_config & (1 << 2))
        ICMP_STATS_DISPLAY();
#else
    if (stats_config & (1 << 2))
        printf("Please open the ICMP_STATS Macro before compile!!!\n");
#endif

#if  IGMP_STATS
    if (stats_config & (1 << 3))
        IGMP_STATS_DISPLAY();
#else
    if (stats_config & (1 << 3))
        printf("Please open the IGMP_STATS Macro before compile!!!\n");
#endif

#if IPFRAG_STATS
    if (stats_config & (1 << 4))
        IPFRAG_STATS_DISPLAY();
#else
    if (stats_config & (1 << 4))
        printf("Please open the IPFRAG_STATS Macro before compile!!!\n");
#endif

#if IP_STATS
    if (stats_config & (1 << 5))
        IP_STATS_DISPLAY();
#else
    if (stats_config & (1 << 5))
        printf("Please open the IP_STATS Macro before compile!!!\n");
#endif

#if ETHARP_STATS
    if (stats_config & (1 << 6))
        ETHARP_STATS_DISPLAY();
#else
    if (stats_config & (1 << 6))
        printf("Please open the ETHARP_STATS Macro before compile!!!\n");
#endif

#if LINK_STATS
    if (stats_config & (1 << 7))
        LINK_STATS_DISPLAY();
#else
    if (stats_config & (1 << 7))
        printf("Please open the LINK_STATS Macro before compile!!!\n");
#endif

#if MEM_STATS
    if (stats_config & (1 << 8))
        MEM_STATS_DISPLAY();
#else
    if (stats_config & (1 << 8))
        printf("Please open the MEM_STATS Macro before compile!!!\n");
#endif

#if MEMP_STATS
    uint32_t    i;
    if (stats_config & (1 << 9)) {
        for (i = 0; i < MEMP_MAX; i++) {
            MEMP_STATS_DISPLAY(i);
        }
    }
#else
    if (stats_config & (1 << 9))
        printf("Please open the MEMP_STATS Macro before compile!!!\n");
#endif

#if SYS_STATS
    if (stats_config & (1 << 10))
        SYS_STATS_DISPLAY();
#else
    if (stats_config & (1 << 10))
        printf("Please open the SYS_STATS Macro before compile!!!\n");
#endif


#else /*LWIP_STATS_DISPLAY*/
    printf("Please open the LWIP_STATS_DISPLAY Macro before compile!!!\n");
#endif
    return 0;
}

/**
 * Process interface commands from user.
 *
 * This function is recursive such that user can input commands consecutively.
 *
 * @retval 1    if fail when processing following subcommands.
 * @retval 0    if succeeded when processing following subcommands.
 */
static uint8_t _cli_ip_cmds(
    char            *ifname,
    struct netif    *iface,
    uint8_t         len,
    char            *param[])
{

    return 0;
}

static int _cli_ip_show(char *ifname)
{
    lwip_get_ip_info(ifname);
    return 0;
}

#if defined(LWIP_DYNAMIC_DEBUG_ENABLE)
static int _set_debug_flag(int len, char *param[])
{
	uint8_t i = 0;

	for (i = 0; i < len; i++)
	{
		lwip_debug_flags[atoi(param[i])].debug_flag |= LWIP_DBG_ON;
	}

	return 0;
}

static int _clear_debug_flag(int len, char *param[])
{
	uint8_t i = 0;

	printf("clear debug flags:\n");

	for (i = 0; i < len; i++)
	{
		lwip_debug_flags[atoi(param[i])].debug_flag &= ~LWIP_DBG_ON;
		printf("%d: %s\n", atoi(param[i]), lwip_debug_flags[atoi(param[i])].debug_flag_name);
	}

	return 0;
}

static int _show_all_debug_flag(int len, char *param[])
{
	uint8_t i = 0;

	printf("show all debug flag:\n");

	for (i = 0; lwip_debug_flags[i].debug_flag_name != NULL; i++) {
		printf("%d: %s %s\n", i,
                              lwip_debug_flags[i].debug_flag_name,
                              (lwip_debug_flags[i].debug_flag & LWIP_DBG_ON) ?
                                    "on" : "off");
	}

	return 0;
}
#else

static int _show_all_debug_flag(int len, char *param[]) {return 0;}
static int _set_debug_flag(int len, char *param[])  {return 0;}
static int _clear_debug_flag(int len, char *param[])  {return 0;}

#endif


/****************************************************************************
 *
 * API variable.
 *
 ****************************************************************************/

cli_command_t lwip_cli[] = {
    { "all",  "show all debug flag", _show_all_debug_flag },
    { "on",   "set all debug flag",  _set_debug_flag      },
    { "off",  "clr all debug flag",  _clear_debug_flag    },
    { "stat", "show statistics",     _show_lwip_stat      },
    { NULL }
};


/****************************************************************************
 *
 * API functions.
 *
 ****************************************************************************/


/**
 * IP configuration handler function.
 *
 */
int lwip_ip_cli(int len, char *param[])
{
   //ToDO:
    return 0;
}

int lwip_cli_handler(int token_cnt, char *param[])
{
    if (token_cnt < 1) return 0;

    if (strcmp(param[0], "init") == NULL )
    {
        lwip_network_init(WIFI_MODE_STA_ONLY);
    }
    else if (strcmp(param[0], "start") == NULL)
    {
        lwip_net_start(WIFI_MODE_STA_ONLY);
    }
    else if (strcmp(param[0], "stop") == NULL)
    {
        lwip_net_stop(WIFI_MODE_STA_ONLY);
    }
    else if (strcmp(param[0], "ip_info") == NULL )
    {
        _cli_ip_show("st1");
    }
    else if (strcmp(param[0], "sta") == NULL)
    {
        stats_display();
    }
    else if (strcmp(param[0], "client") == NULL)
    {
        tcp_client(param);
    }
    else if (strcmp(param[0], "demo") == NULL)
    {
        socket_server_demo(param);
    }
#if defined(LWIP_DYNAMIC_DEBUG_ENABLE)
    else if (strcmp(param[0], "debug_all") == NULL)
    {
       _show_all_debug_flag(token_cnt, param);
    }
    else if (strcmp(param[0], "debug_set") == NULL)
    {
       _set_debug_flag(token_cnt-1, (param+1));
    }
    else if (strcmp(param[0], "debug_clr") == NULL)
    {
       _clear_debug_flag(token_cnt-1, (param+1));
    }
#endif
    return 0;
}

