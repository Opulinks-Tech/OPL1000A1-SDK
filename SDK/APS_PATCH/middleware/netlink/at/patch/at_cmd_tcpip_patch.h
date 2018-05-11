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
 * @file at_cmd_tcpip_patch.h
 * @author Michael Liao
 * @date 20 Mar 2018
 * @brief File containing declaration of at_cmd_tcpip api & definition of structure for reference.
 *
 */

#ifndef __AT_CMD_TCPIP_PATCH_H__
#define __AT_CMD_TCPIP_PATCH_H__

#include <stdint.h>
#include "lwip/sockets.h"

#define AT_DATA_TASK_QUEUE_SIZE 10
#define AT_LINK_MAX_NUM         5

#define AT_DATA_LEN_MAX     2048
#define AT_DATA_RX_BUFSIZE  1024

#define AT_DATA_TX_EVENT        0x0001
#define AT_DATA_TIMER_EVENT     0x0002

#define AT_SERVER_DEFAULT_PORT      (333)

#define IP2STR(ipaddr) ip4_addr1_16(ip_2_ip4(ipaddr)), \
    ip4_addr2_16(ip_2_ip4(ipaddr)), \
    ip4_addr3_16(ip_2_ip4(ipaddr)), \
    ip4_addr4_16(ip_2_ip4(ipaddr))

#define IPSTR "%d.%d.%d.%d"


#define IPV42STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)




typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} net_adapter_ip_info_t;

typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< station interface */
    TCPIP_ADAPTER_IF_AP,          /**< soft-AP interface */
    TCPIP_ADAPTER_IF_MAX
} net_adapter_if_t;

typedef struct {
    uint32_t event;
	uint32_t length;
	uint8_t *param;
} at_event_msg_t;


/** @brief
 * This enum defines the status of the station interface
 */
typedef enum {
    AT_STA_INACTIVE = 0,
    AT_STA_IDLE,
    AT_STA_GOT_IP,             /**<station got IP from connected AP.*/
    AT_STA_LINKED,             /**<station has created a TCP or UDP transmission.*/
    AT_STA_UNLINKED,           /**<The TCP or UDP transmission of station is disconnected */
    AT_STA_DISCONNECT          /**<station does not connect to an AP.*/
} at_state_type_t;


/** @brief
 * This enum defines the types of device running as a client or server
 */
typedef enum {
    AT_LOCAL_CLIENT = 0,
    AT_REMOTE_CLIENT
} at_terminal_type;


typedef enum {
    AT_LINK_DISCONNECTED = 0,
    AT_LINK_CONNECTING,
    AT_LINK_CONNECTED,
    AT_LINK_WAIT_SENDING,
    AT_LINK_SENDING,
    AT_LINK_WAIT_SENDING_CB,
    AT_LINK_TRANSMIT_CONNECTED,
    AT_LINK_TRANSMIT_SEND,
    AT_LINK_TRANSMIT_NON_CONN,
    AT_LINK_DISCONNECTING,
    AT_LINK_CHANGED
} at_link_state;


enum {
    AT_SEND_MODE_IDLE = 0,
    AT_SEND_MODE_SEND,
    AT_SEND_MODE_SENDEX,
    AT_SEND_MODE_TRANSMIT,
};


/** @brief
 * This enum defines the TCPIP link transmission type.
 */
typedef enum {
    AT_LINK_TYPE_TCP = 0,
    AT_LINK_TYPE_UDP,
    AT_LINK_TYPE_SSL,
    AT_LINK_TYPE_INVALID
} at_link_type_t;


typedef struct {
    at_link_state link_state;
    at_terminal_type terminal_type;
    int sock;
    int link_en;
    uint8_t send_mode;
    uint8_t link_id;
    uint8_t link_type;
    uint8_t timeout;
    uint8_t repeat_time;
    uint8_t change_mode;
    ip_addr_t remote_ip;
    uint16_t remote_port;
    uint16_t local_port;

    int32_t keep_alive;
    int32_t server_timeout;
    void* task_handle;
    char* recv_buf;
} at_socket_t;


void _at_cmd_tcpip_func_init(void);

void at_link_init(uint32_t conn_max);
int at_cmd_is_tcpip_ready(void);
int at_cmd_is_valid_ip(char *sIpStr);

void at_create_tcpip_data_task(void);
int at_data_task_send(at_event_msg_t *msg);
int at_socket_client_create_task(at_socket_t *plink);
int at_socket_client_cleanup_task(at_socket_t *plink);
int at_socket_server_create_task(int sock);
int at_socket_server_cleanup_task(int sock);


#endif //__AT_CMD_TCPIP_H__

