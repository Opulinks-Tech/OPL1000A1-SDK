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
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "cmsis_os.h"
#include "hal_wdt.h"
#include "at_cmd.h"
#include "at_cmd_tcpip.h"
#include "at_cmd_common.h"
#include "at_cmd_tcpip.h"
#include "at_cmd.h"
#include "at_cmd_data_process.h"
#include "at_cmd_app.h"

#include "sys_os_config.h"

#include "wpa_cli.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/netif.h"
#include "lwip/apps/sntp.h"
#include "wlannetif.h"
#include "ping_cmd.h"
#include "wifi_api.h"
#include "network_config.h"
#include "controller_wifi_com.h"
#include "at_cmd_tcpip_patch.h"
#include "wpa_cli_patch.h"
#include "sys_common_api.h"
#include "sys_os_config_patch.h"
#include "at_cmd_common_patch.h"
#include "at_cmd_data_process_patch.h"
#include "at_cmd_nvm.h"

extern void data_process_lock_patch(int module, int data_len);

/******************************************************
 *                      Macros
 ******************************************************/
//#define AT_CMD_TCPIP_DBG

#ifdef AT_CMD_TCPIP_DBG
    #define AT_LOG                      printf(("[AT]: "fmt"\r\n"), ##arg)
#else
    #define AT_LOG(...)
#endif

#define AT_LOGI(fmt,arg...)             printf(("[AT]: "fmt"\r\n"), ##arg)

/******************************************************
 *                    Constants
 ******************************************************/


/******************************************************
 *                   Enumerations
 ******************************************************/


/******************************************************
 *                 Type Definitions
 ******************************************************/
#define AT_TRANS_TERM_STR "+++"

/******************************************************
 *                    Structures
 ******************************************************/


/******************************************************
 *               Static Function Declarations
 ******************************************************/


/******************************************************
 *               Variable Definitions
 ******************************************************/

extern _at_command_t *_g_AtCmdTbl_Tcpip_Ptr;
#if defined(__AT_CMD_SUPPORT__)
extern volatile at_state_type_t mdState;
extern volatile bool at_ip_mode; // 0: normal transmission mode. 1:transparent transmission
extern volatile bool at_ipMux;
extern volatile bool ipd_info_enable;

extern uint8_t sending_id;
extern uint32_t at_send_len;

extern at_socket_t atcmd_socket[AT_LINK_MAX_NUM];
extern int at_netconn_max_num;

extern volatile uint16_t server_timeover;
extern int tcp_server_socket;
extern TimerHandle_t server_timeout_timer;

extern char *at_sntp_server[SNTP_MAX_SERVERS];

extern osThreadId   at_tx_task_id;
extern osThreadId   at_tcp_server_task_handle;
extern osMessageQId at_tx_task_queue_id;
extern osPoolId     at_tx_task_pool_id;

extern const osPoolDef_t os_pool_def_at_tx_task_pool;
extern uint8_t *pDataLine;
extern uint8_t  at_data_line[];

RET_DATA uint8_t g_server_mode;
RET_DATA uint32_t g_server_port;

RET_DATA TimerHandle_t at_trans_timer;

/******************************************************
 *               Function Definitions
 ******************************************************/
void at_update_link_count_patch(int8_t count)
{
    static int cnt = 0;
    int tmp = 0;
    tmp = cnt + count;

    if ((cnt > 0) && (tmp <= 0)) {
        cnt = 0;
        if (mdState == AT_STA_LINKED) {
            mdState = AT_STA_UNLINKED;
        }
    } else if ((cnt >= 0) && (tmp > 0)) {
        cnt = tmp;
        mdState = AT_STA_LINKED;
    }
}

int at_close_client_patch(at_socket_t *link)
{
    int ret = -1;
    if (link->sock >= 0) {
        link->link_state = AT_LINK_DISCONNECTING;
        ret = lwip_close(link->sock);
        link->sock = -1;
    }

    link->link_en = 0;
    link->link_state = AT_LINK_DISCONNECTED;
    link->link_type = AT_LINK_TYPE_INVALID;
    return ret;
}

int at_create_tcp_client_trans(at_socket_t *link)
{
    struct sockaddr_in remote_addr;
    int optval = 0;
    int ret;
    //struct timeval tv;

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_len = sizeof(remote_addr);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = lwip_htons(link->remote_port);
    inet_addr_from_ip4addr(&remote_addr.sin_addr, ip_2_ip4(&link->remote_ip));

    /* Create the socket */
    link->sock = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (link->sock < 0) {
        at_show_socket_error_reason("TCP client create", link->sock);
        return -1;
    }

    setsockopt(link->sock,SOL_SOCKET, SO_REUSEADDR ,&optval, sizeof(optval));
    setsockopt(link->sock,SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    if (link->keep_alive > 0) {
        optval = 1;
        setsockopt(link->sock,SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
        optval = link->keep_alive;
        setsockopt(link->sock,IPPROTO_TCP, TCP_KEEPIDLE,&optval, sizeof(optval));
        optval = 1;
        setsockopt(link->sock,IPPROTO_TCP, TCP_KEEPINTVL,&optval, sizeof(optval));
        optval = 3;
        setsockopt(link->sock,IPPROTO_TCP, TCP_KEEPCNT,&optval, sizeof(optval));
    }

    AT_LOGI("sock=%d, connecting to server IP:%s, Port:%d...",
             link->sock, ipaddr_ntoa(&link->remote_ip), link->remote_port);

    /* Connect */
    ret = lwip_connect(link->sock, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
    if (ret < 0) {
        at_show_socket_error_reason("TCP client connect", link->sock);
        lwip_close(link->sock);
        link->sock = -1;
        return -1;
    }
    
    AT_LOGI("Transparent Mode : socket create successful\r\n");
    link->link_type = AT_LINK_TYPE_TCP;
    link->link_state = AT_LINK_TRANSMIT_SEND;
    
    return ret;
}

void at_process_recv_socket_patch(at_socket_t *plink)
{
    struct sockaddr_in sa;
    socklen_t socklen = sizeof(struct sockaddr_in);
    ip_addr_t remote_ip;
    uint16_t remote_port;
    char *rcv_buf = NULL;
    int len = 0;
    int32_t sock = -1;
    char temp[40] = {0};

    if (plink == NULL){
        return;
    }

    rcv_buf = plink->recv_buf;
    
    //Poll read
    if (at_socket_read_set_timeout(plink, 10000) <= 0) {
        AT_LOGI("read timeout\r\n");
        return;
    }

    //if ((rcv_buf = (char *)malloc(AT_DATA_RX_BUFSIZE * sizeof(char))) == NULL) {
    //    AT_LOGI("rx buffer alloc fail\r\n");
    //    return;
    //}

    memset(&sa, 0x0, socklen);

    if (plink->link_type == AT_LINK_TYPE_TCP) {
        len = lwip_recv(plink->sock, rcv_buf, AT_DATA_RX_BUFSIZE, 0);
    } else if (plink->link_type == AT_LINK_TYPE_UDP) {
        len = lwip_recvfrom(plink->sock, rcv_buf, AT_DATA_RX_BUFSIZE, 0, (struct sockaddr*)&sa, &socklen);
    } else {
        /* TODO:AT_LINK_TYPE_SSL*/
        len = 0;
    }

    if (len > 0)
    {
        if (at_ip_mode == true) {
            at_uart1_write_buffer(rcv_buf, len);
        } else {
            uint32_t header_len = 0;

            if (plink->link_type == AT_LINK_TYPE_UDP) {
                inet_addr_to_ip4addr(ip_2_ip4(&remote_ip), &sa.sin_addr);
                remote_port = htons(sa.sin_port);

                if (plink->change_mode == 2) {
                    plink->remote_ip = remote_ip;
                    plink->remote_port = remote_port;
                } else if (plink->change_mode == 1) {
                    plink->remote_ip = remote_ip;
                    plink->remote_port = remote_port;
                    plink->change_mode = 0;
                }
            } else { //TCP
                remote_ip = plink->remote_ip;
                remote_port = plink->remote_port;
            }

            if (ipd_info_enable == true) {
                if (at_ipMux) {
                    header_len = sprintf(temp, "\r\n+IPD,%d,%d,"IPSTR",%d:",plink->link_id, len,
                            IP2STR(&remote_ip), remote_port);
                }
                else {
                    header_len = sprintf(temp, "\r\n+IPD,%d,"IPSTR",%d:", len,
                            IP2STR(&remote_ip), remote_port);

                }
            } else {
                if (at_ipMux) {
                    header_len = sprintf(temp,"\r\n+IPD,%d,%d:",plink->link_id, len);
                } else {
                    header_len = sprintf(temp,"\r\n+IPD,%d:",len);
                }
            }

            // Send +IPD info
            at_uart1_write_buffer(temp, header_len);
            // Send data
            at_uart1_write_buffer(rcv_buf, len);

            if (plink->link_state != AT_LINK_WAIT_SENDING) {
                plink->link_state = AT_LINK_CONNECTED;
            }
            if ((plink->sock >= 0) && (plink->terminal_type == AT_REMOTE_CLIENT)) {
                plink->server_timeout = 0;
            }
        }
    }
    else
    {
        //len = 0, Connection close
        //len < 0, error
        if(plink->sock >= 0) {
            if ((at_ip_mode != true) || (plink->link_state != AT_LINK_TRANSMIT_SEND)) {
                if (at_ipMux == true) {
                    sprintf(temp,"%d,CLOSED\r\n", plink->link_id);
                } else {
                    sprintf(temp,"CLOSED\r\n");
                }
                msg_print_uart1(temp);

                if (plink->link_state != AT_LINK_DISCONNECTING) {
                    sock = plink->sock;
                    plink->sock = -1;
                    plink->link_type = AT_LINK_TYPE_INVALID;
                    plink->link_en = 0;
                    if (sock >= 0) {
                        close(sock);
                    }
                    plink->link_state = AT_LINK_DISCONNECTED;
                }
            } else {
                AT_LOGI("Transparent Mode : Socket closed\r\n");
                if (plink->sock >= 0) {
                    close(plink->sock);
                    plink->sock = -1;
                }
                plink->link_type = AT_LINK_TYPE_INVALID;
                plink->link_state = AT_LINK_DISCONNECTED;
                plink->link_en = 0;
            }
        }
    }
}

int at_socket_client_cleanup_task_patch(at_socket_t* plink)
{
    int ret = -1;

    if (plink == NULL) {
        return ret;
    }
    //AT_SOCKET_LOCK();
    ret = at_close_client(plink);
    if (plink->task_handle) {
        //vTaskDelete(plink->task_handle);
        plink->task_handle = NULL;
    }

    if (plink->recv_buf) {
        free(plink->recv_buf);
        plink->recv_buf = NULL;
    }
    //AT_SOCKET_UNLOCK();
    return ret;
}

void at_socket_server_listen_task_patch(void *arg)
{
    int loop = 0;
    int listen_sock = -1;
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    int client_sock = -1;
    char tmp[16];
    at_socket_t *plink;

    if (arg == NULL) {
        goto task_exit;
    }

    listen_sock= *(uint32_t*)arg;
    if (listen_sock < 0) {
        goto task_exit;
    }

    for(;;)
    {
        client_sock = accept(listen_sock, (struct sockaddr *)&sa, &len);
        if (client_sock < 0)
            break;

            //AT_SOCKET_LOCK();
        for (loop = 0; loop < AT_LINK_MAX_NUM; loop++) {
            plink = at_link_get_id(loop);
            if (plink->sock < 0) {
                break;
            }
        }

        if (loop < AT_LINK_MAX_NUM)
        {
            if ((plink->recv_buf = (char *)malloc(AT_DATA_RX_BUFSIZE * sizeof(char))) == NULL) {
                AT_LOGI("rx buffer alloc fail\r\n");
                goto task_exit;
            }
            plink->link_state = AT_LINK_CONNECTED;
            inet_addr_to_ip4addr(ip_2_ip4(&plink->remote_ip), &sa.sin_addr);
            plink->remote_port = ntohs(sa.sin_port);
            plink->repeat_time = 0;
            plink->sock = client_sock;
            plink->terminal_type = AT_REMOTE_CLIENT;
            plink->server_timeout = 0;
            plink->link_type = AT_LINK_TYPE_TCP;//SSL
            at_sprintf(tmp,"%d,CONNECT\r\n",loop);
            msg_print_uart1(tmp);

            at_socket_client_create_task(plink);
            //AT_SOCKET_UNLOCK();
        }
        else
        {
            //AT_SOCKET_UNLOCK();
            at_sprintf(tmp,"connect reach max\r\n");
            msg_print_uart1(tmp);
            close(client_sock);
        }
    }

task_exit:

	if (listen_sock >= 0) {
        close(listen_sock);
        listen_sock = -1;
        tcp_server_socket = -1;
    }

	at_tcp_server_task_handle = NULL;

    vTaskDelete(NULL);
}

int at_socket_server_create_task_patch(int sock)
{
    osThreadDef_t task_def;

    if ((sock < 0) || (at_tcp_server_task_handle != NULL)) {
        return false;
    }

    /* Create task */
    task_def.name = OS_TASK_NAME_AT_SOCKET_SERVER;
    task_def.stacksize = 512;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = at_socket_server_listen_task;
    at_tcp_server_task_handle = osThreadCreate(&task_def, (void*)&tcp_server_socket);
    if(at_tcp_server_task_handle == NULL)
    {
        AT_LOGI("at tcp server task create fail \r\n");
    }
    else
    {
        AT_LOGI("at tcp server task create successful \r\n");
    }

    return 1;
}

int at_socket_server_cleanup_task_patch(int sock)
{
    int i;
    char temp[64] ={0};
    at_socket_t *plink = NULL;

	AT_LOGI("at_socket_server_cleanup_task entry\r\n");
    if ((sock < 0) /*|| (sock != tcp_server_socket)*/) {
        return 0;
    }

	AT_LOGI("at_tcp_server_task_handle:%p\r\n",at_tcp_server_task_handle);

    // Destory server socket listener task
    if (at_tcp_server_task_handle) {
        vTaskDelete(at_tcp_server_task_handle);
        at_tcp_server_task_handle = NULL;
    }

	AT_LOGI("tcp_server_socket:%d\r\n",sock);
    if (sock >= 0) {
        close(sock);
        sock = -1;
        tcp_server_socket = -1;
    }

    //close all accept client connection socket
    for (i = 0; i < at_netconn_max_num; i++) {
        plink = at_link_get_id(i);
        if ((plink->sock >= 0) && (plink->terminal_type == AT_REMOTE_CLIENT)) {
           at_socket_client_cleanup_task(plink);
           at_sprintf(temp,"%d,CLOSED\r\n", i);
           msg_print_uart1(temp);
        }
    }
	AT_LOGI("at_socket_server_cleanup_task leave\r\n");
    return 1;
}

void at_trans_timeout_cb( TimerHandle_t xTimer )
{
    uint32_t data_len = 0;
    data_len = pDataLine - at_data_line;
    pDataLine = at_data_line;
    
    if (data_len > 0) {
        //When a single packet containing +++ is received,
        //returns to normal command mode.
        if (memcmp(&at_data_line[0], AT_TRANS_TERM_STR, 3) == 0) {
            at_ip_mode = false;
            atcmd_socket[0].send_mode = AT_SEND_MODE_IDLE;
            pDataLine = at_data_line; //Point to index 0
            data_process_unlock();
            
            if (at_trans_timer) {
                xTimerStop(at_trans_timer, portMAX_DELAY);
                xTimerDelete(at_trans_timer, portMAX_DELAY);
                at_trans_timer = NULL;
            }
            
            AT_LOGI("Transparent Mode : off\r\n");
            return;
        }
        
        at_event_msg_t msg = {0};

        msg.event = AT_DATA_TX_EVENT;
        msg.length = data_len;
        msg.param = at_data_line;
        at_data_task_send(&msg);
    }
}

int at_cmd_trans_lock(void)
{
    data_process_lock_patch(LOCK_TCPIP, AT_TCP_TRANS_LOCK_ID);
    
    if (at_trans_timer == NULL) {
        at_trans_timer = xTimerCreate("TransTimoutTmr",
                                      (20 / portTICK_PERIOD_MS),
                                      pdTRUE,
                                      NULL,
                                      at_trans_timeout_cb);
        xTimerStart(at_trans_timer, 0);
    }
    
    AT_LOGI("Transparent Mode : on\r\n");
    
    return 0;
}

void at_server_timeout_handler_patch(void)
{
    uint8_t loop = 0;
    at_socket_t *link = NULL;
    char resp_buf[32];
    
    for (loop = 0; loop < at_netconn_max_num; loop++) {
        link = at_link_get_id(loop);
        if ((link->sock >= 0) && (link->terminal_type == AT_REMOTE_CLIENT)) {
            link->server_timeout++;

            if (link->server_timeout >= server_timeover) {
                at_socket_client_cleanup_task(link);
                at_sprintf(resp_buf,"%d,CLOSED\r\n", loop);
                msg_print_uart1(resp_buf);
            }
        }
    }
    AT_LOGI("at_server_timeout_handler\r\n");
}

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
int _at_cmd_tcpip_cipstatus_patch(char *buf, int len, int mode)
{
    uint8_t* temp = NULL;
    uint8_t i = 0;
    struct sockaddr_in local_addr;
    socklen_t local_name_len;
    uint8_t pStr[4];
    int sc_type = 0;
    socklen_t optlen = sizeof(sc_type);
    uint8_t ret = AT_RESULT_CODE_ERROR;

    temp = (uint8_t*)at_malloc(64);

    at_sprintf(temp, "STATUS:%d\r\n", mdState);
    msg_print_uart1((char*)temp);

    for(i = 0; i < AT_LINK_MAX_NUM; i++)
    {
        at_socket_t *pLink = at_link_get_id(i);
        if(pLink->sock >= 0)
        {
            local_name_len = sizeof(local_addr);
            if(getsockopt(pLink->sock, SOL_SOCKET, SO_TYPE, &sc_type, &optlen) < 0)
            {
                goto exit;
            }

            if(getsockname(pLink->sock, (struct sockaddr*)&local_addr, &local_name_len) < 0)
            {
                goto exit;
            }

            at_memset(pStr, 0x0 ,sizeof(pStr));
            if(sc_type == SOCK_STREAM) // tcp
            {
                pStr[0] = 'T';
                pStr[1] = 'C';
                pStr[2] = 'P';
            }
            else if(sc_type == SOCK_DGRAM) // udp
            {
                pStr[0] = 'U';
                pStr[1] = 'D';
                pStr[2] = 'P';
            }
            else // raw
            {
                pStr[0] = 'R';
                pStr[1] = 'A';
                pStr[2] = 'W';
            }

            at_sprintf(temp, "%s:%d,\"%s\",\""IPSTR"\",%d,%d,%d\r\n",
                    "+CIPSTATUS",
                    pLink->link_id,
                    pStr,
                    IP2STR(&pLink->remote_ip),
                    pLink->remote_port,
                    ntohs(local_addr.sin_port),
                    pLink->terminal_type);
            msg_print_uart1((char*)temp);
        }
    }

    ret = AT_RESULT_CODE_OK;

exit:
    at_free(temp);
    at_response_result(ret);

    return ret;
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
int _at_cmd_tcpip_cipstart_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    char *pstr = NULL;
    char *remote_addr;
    char response[16];
    uint8_t linkID;
    uint8_t linkType = AT_LINK_TYPE_INVALID;
    uint8_t para = 1;
    int changType = 0;
    int remotePort = 0, localPort = 0;
    int keepAliveTime = 0;
    at_socket_t *link;
    uint8_t ret = AT_RESULT_CODE_ERROR;

    if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        AT_LOGI("at_cmd_buf_to_argc_argv fail\r\n");
        goto exit;
    }

    if (argc < 4)
    {
        AT_LOGI("Invalid param\r\n");
        goto exit;
    }

    if (at_ipMux)
    {
        linkID = atoi(argv[para++]);
    }
    else
    {
        linkID = 0;
    }

    if (linkID >= AT_LINK_MAX_NUM)
    {
        msg_print_uart1("ID ERROR\r\n");
        goto exit;
    }

    pstr = at_cmd_param_trim(argv[para++]);
    if(!pstr)
    {
        msg_print_uart1("Link type ERROR\r\n");
        goto exit;
    }

    if (at_strcmp(pstr, "TCP") == NULL)
    {
        linkType = AT_LINK_TYPE_TCP;
    }
    else if (at_strcmp(pstr, "UDP") == NULL)
    {
        linkType = AT_LINK_TYPE_UDP;
    }
    else if (at_strcmp(pstr, "SSL") == NULL)
    {
        linkType = AT_LINK_TYPE_SSL;
    }
    else
    {
        msg_print_uart1("Link type ERROR\r\n");
        goto exit;
    }
    remote_addr = at_cmd_param_trim(argv[para++]);
    if(!remote_addr)
    {
        msg_print_uart1("IP ERROR\r\n");
        goto exit;
    }
    AT_LOGI("%s\r\n", remote_addr);

    remotePort = atoi(argv[para++]);
    if((remotePort == 0))
    {
        msg_print_uart1("Miss param\r\n");
        goto exit;
    }

    if((remotePort > 65535) || (remotePort < 0))
    {
       goto exit;
    }

    AT_LOGI("remote port %d\r\n", remotePort);

    if (linkType == AT_LINK_TYPE_TCP)
    {
        if (para < argc)
        {
            keepAliveTime = atoi(argv[para++]);
            AT_LOGI("keepAliveTime %d\r\n", keepAliveTime);
            if ((keepAliveTime > 7200) || (keepAliveTime < 0)) {
                 goto exit;
            }
        }
        changType = 1;
    }
    else if (linkType == AT_LINK_TYPE_UDP)
    {
        if (para < argc)
        {
            localPort = atoi(argv[para++]);
            if (localPort == 0)
            {
                msg_print_uart1("Miss param2\r\n");
                goto exit;
            }

            if ((localPort > 65535) || (localPort <= 0))
            {
                goto exit;
            }
            AT_LOGI("localPort %d\r\n", localPort);

            if (para < argc)
            {
                changType = atoi(argv[para++]); //last param
                if((changType < 0) || (changType > 2))
                {
                    goto exit;
                }
                
                if ((at_ip_mode == true) && (changType != 0))
                {
                    goto exit;
                }
                AT_LOGI("changType %d\r\n", changType);
            }
        }
    }
    else if (linkType == AT_LINK_TYPE_SSL)
    {
        AT_LOGI("SSL not implement yet\r\n");
    }

    link = at_link_alloc_id(linkID);
    if (link == NULL)
    {
        msg_print_uart1("ALREAY CONNECT\r\n");
        goto exit;
    }

    link->link_type = linkType;
    link->link_state = AT_LINK_DISCONNECTED;
    link->change_mode = changType;
    link->remote_port = remotePort;
    link->local_port = localPort;
    link->repeat_time = 0;
    link->keep_alive = keepAliveTime;
    link->terminal_type = AT_LOCAL_CLIENT;

    if ((link->recv_buf = (char *)malloc(AT_DATA_RX_BUFSIZE * sizeof(char))) == NULL) {
        AT_LOGI("rx buffer alloc fail\r\n");
        goto exit;
    }

    switch (linkType)
    {
        case AT_LINK_TYPE_TCP:
            if (at_create_tcp_client(link, remote_addr, remotePort, 0) < 0)
            {
                msg_print_uart1("CONNECT FAIL\r\n");
                at_close_client(link);
                goto exit;
            }
            break;

        case AT_LINK_TYPE_UDP:
            if (at_create_udp_client(link, remote_addr, remotePort, localPort, 0) < 0)
            {
                msg_print_uart1("CONNECT FAIL\r\n");
                at_close_client(link);
                goto exit;
            }
            break;

        case AT_LINK_TYPE_SSL:
            msg_print_uart1("Not Support Yet\r\n");
        default:
            goto exit;
    }

    at_create_tcpip_data_task();
    
    if(at_ipMux)
    {
        at_sprintf(response,"%d,CONNECT\r\n", linkID);
        msg_print_uart1(response);
    }
    else
    {
        msg_print_uart1("CONNECT\r\n");
    }

    ret = AT_RESULT_CODE_OK;

    at_socket_client_create_task(link);

exit:
    at_response_result(ret);

    return ret;
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
int _at_cmd_tcpip_cipclose_patch(char *buf, int len, int mode)
{
    char *param = NULL;
    char resp_buf[32];
    uint8_t link_id, id;
    uint8_t ret = AT_RESULT_CODE_ERROR;
    at_socket_t *link;

    switch (mode) {
        case AT_CMD_MODE_EXECUTION:
            if (at_ipMux)
            {
                msg_print_uart1("MUX=1\r\n");
                goto exit;
            }

            link = at_link_get_id(0);
            if (link->sock >= 0)
            {
                at_socket_client_cleanup_task(link);
                at_sprintf(resp_buf,"%d,CLOSED\r\n", 0);
                msg_print_uart1(resp_buf);
                ret = AT_RESULT_CODE_OK;
            }

            break;

        case AT_CMD_MODE_SET:
            if(at_ipMux == 0)
            {
                msg_print_uart1("MUX=0\r\n");
                goto exit;
            }

            param = strtok(buf, "=");
            param = strtok(NULL, "\0");
            link_id = atoi(param);

            if (link_id > AT_LINK_MAX_NUM)
            {
                goto exit;
            }

            if (link_id == AT_LINK_MAX_NUM) {
                /* when ID=5, all connections will be closed.*/
                for(id = 0; id < AT_LINK_MAX_NUM; id++) {
                    link = at_link_get_id(id);

                    if (link->link_en == 1 || link->sock >=0) {
                        at_socket_client_cleanup_task(link);
                        at_sprintf(resp_buf,"%d,CLOSED\r\n", id);
                        msg_print_uart1(resp_buf);
                    }
                }
            } else {
                link = at_link_get_id(link_id);
                if (link->link_en == 1 || link->sock >=0) {
                    at_socket_client_cleanup_task(link);
                    at_sprintf(resp_buf,"%d,CLOSED\r\n", link_id);
                    msg_print_uart1(resp_buf);
                }
                else
                {
                    goto exit;
                }
            }
            ret = AT_RESULT_CODE_OK;
            break;

        default :
            ret = AT_RESULT_CODE_IGNORE;
            break;
    }

exit:
    at_response_result(ret);

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
int at_cmd_tcpip_cipmux_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    int  ipMux;
    uint8_t ret = AT_RESULT_CODE_ERROR;

    if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        AT_LOGI("at_cmd_buf_to_argc_argv fail\r\n");
        goto exit;
    }
    
    switch (mode) {
        case AT_CMD_MODE_READ:
            msg_print_uart1("+CIPMUX:%d\r\n", at_ipMux);
            break;

        case AT_CMD_MODE_SET:

            if (mdState == AT_STA_LINKED)
            {
                msg_print_uart1("link is builded\r\n");
                goto exit;
            }

            if (at_cmd_get_para_as_digital(argv[1], &ipMux) != 0) {
                goto exit;
            }
            
            if (ipMux > 1 || ipMux < 0)
            {
                goto exit;
            }
            
            if (ipMux == 1)
            {
                if (at_ip_mode == true)  // now serverEn is 0
                {
                    msg_print_uart1("IPMODE must be 0\r\n");
                    goto exit;
                }
                at_ipMux = true;
            }
            else
            {
                if (tcp_server_socket >= 0)
                {
                    msg_print_uart1("CIPSERVER must be 0\r\n");
                    goto exit;
                }
                if((at_ipMux == TRUE) && (mdState == AT_STA_LINKED))
                {
                    msg_print_uart1("Connection exists\r\n");
                    goto exit;
                }
                at_ipMux = false;
            }
            break;

        default :
            ret = AT_RESULT_CODE_IGNORE;
            goto exit;
    }

    ret = AT_RESULT_CODE_OK;
exit:
    at_response_result(ret);

    return ret;
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
int _at_cmd_tcpip_cipserver_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    //char *pstr = NULL;
    //uint8_t linkType = AT_LINK_TYPE_INVALID;
    uint8_t para = 1;
    int32_t server_enable = 0;
    int32_t port = 0;
    //int32_t ssl_ca_en = 0;
    //at_socket_t *link;
    uint8_t ret = AT_RESULT_CODE_ERROR;

    if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        AT_LOGI("at_cmd_buf_to_argc_argv fail\r\n");
        goto exit;
    }

    switch (mode)
    {
        case AT_CMD_MODE_READ:
            msg_print_uart1("+CIPSERVER:%d,%d\r\n", g_server_mode, g_server_port);
            ret = AT_RESULT_CODE_OK;
            break;
        case AT_CMD_MODE_SET:
            if (at_ipMux == FALSE)
            {
                goto exit;
            }
            
            server_enable = atoi(argv[para++]);
            if (server_enable < 0 || server_enable > 1) {
                goto exit;
            }
            
            g_server_mode = server_enable;
            
            if (g_server_mode == 1)
            {
                if (para != argc)
                {
                    port = atoi(argv[para++]);
                    if ((port > 65535) || (port <= 0))
                    {
                        goto exit;
                    }
                    
                    g_server_port = port;
                    AT_LOGI("port %d\r\n", g_server_port);
                }
                else
                {
                    g_server_port = AT_SERVER_DEFAULT_PORT;
                }

                if (para != argc)
                {
                    #if 0
                    pstr = at_cmd_param_trim(argv[para++]);
                    if (!pstr)
                    {
                        goto exit;
                    }
                    if (at_strcmp(pstr, "SSL") == NULL)
                    {
                        linkType = AT_LINK_TYPE_SSL;
                    }
                    else
                    {
                        linkType = AT_LINK_TYPE_TCP;
                    }

                    if (para != argc)
                    {
                        ssl_ca_en = atoi(argv[para++]);
                    }
                    #endif
                }
            }

            if (g_server_mode == 1)
            {
                at_create_tcpip_data_task();
                at_create_tcp_server(g_server_port, 0);
            }
            else
            {
                if (tcp_server_socket < 0)
                {
                    msg_print_uart1("no change\r\n");
                    ret = AT_RESULT_CODE_OK;
                    goto exit;
                }
                else
                {
                    at_socket_server_cleanup_task(tcp_server_socket);
                    g_server_port = 0;
                }
            }
            
            ret = AT_RESULT_CODE_OK;
            break;
        default:
            ret = AT_RESULT_CODE_IGNORE;
            break;
    }
    
exit:
    at_response_result(ret);
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
int at_cmd_tcpip_cipmode_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    uint8_t ret = AT_RESULT_CODE_ERROR;
    
    if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        AT_LOGI("at_cmd_buf_to_argc_argv fail\r\n");
        goto exit;
    }
    
    switch (mode) {
        case AT_CMD_MODE_READ:
            msg_print_uart1("+CIPMODE:%d\r\n", at_ip_mode);
            ret = AT_RESULT_CODE_OK;
            break;
        case AT_CMD_MODE_SET:
        {
            int ip_mode;
            
            if (at_ipMux == true) {
                goto exit;
            }
            
            if (at_cmd_get_para_as_digital(argv[1], &ip_mode) != 0) {
                goto exit;
            }
            
            if (ip_mode > 1 || ip_mode < 0) {
                goto exit;
            }
            
            if (atcmd_socket[0].sock >= 0 && ip_mode == 1) {
                if (atcmd_socket[0].link_type == AT_LINK_TYPE_UDP &&
                    atcmd_socket[0].change_mode != 0) {
                    goto exit;
                }
                
                atcmd_socket[0].send_mode = AT_SEND_MODE_TRANSMIT;
                atcmd_socket[0].link_state = AT_LINK_TRANSMIT_CONNECTED;
            }
            
            at_ip_mode = ip_mode;
            ret = AT_RESULT_CODE_OK;
        }
            break;
        default:
            ret = AT_RESULT_CODE_IGNORE;
            break;
    }
    
exit:
    at_response_result(ret);
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
int at_cmd_tcpip_cipsend_patch(char *buf, int len, int mode)
{
    char *param = NULL;
    at_socket_t *link;
    int send_id = 0;
    int send_len = 0;
    uint8_t ret = AT_RESULT_CODE_ERROR;

    switch (mode) {
        case AT_CMD_MODE_EXECUTION:
            //TODO:start sending data in transparent transmission mode.
            if (at_ipMux == true || at_ip_mode == false) {
                goto exit;
            }
            
            if (atcmd_socket[0].sock < 0) {
                goto exit;
            }
            
            atcmd_socket[0].link_state = AT_LINK_TRANSMIT_SEND;
            
            sending_id = send_id;
            at_send_len = 0;
            pDataLine = at_data_line;
            
            at_cmd_trans_lock();
            
            at_response_result(AT_RESULT_CODE_OK);
            msg_print_uart1("\r\n> ");
            ret = AT_RESULT_CODE_IGNORE;
            break;

        case AT_CMD_MODE_SET:  // AT+CIPSEND= link,<op>
            param = strtok(buf, "=");
            
            if (at_ip_mode == true)
            {
                goto exit;
            }
            
            if (at_ipMux) {
                /* Multiple connections */
                param = strtok(NULL, ",");
                send_id = (uint8_t)atoi(param);
                if (send_id >= AT_LINK_MAX_NUM) {
                    goto exit;
                }
            } else {
                send_id = 0;
            }

            link = at_link_get_id(send_id);
            if (link->sock < 0) {
                msg_print_uart1("link is not connected\r\n");
                goto exit;
            }

            param = strtok(NULL, "\0");

            send_len = atoi(param);
            if (send_len > AT_DATA_LEN_MAX) {
               msg_print_uart1("data length is too long\r\n");
               goto exit;
            }


            if (link->link_type == AT_LINK_TYPE_UDP)
            {
              //TODO: Remote IP and ports can be set in UDP transmission:
              //AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]
            }

            //switch port input to TCP/IP module
            sending_id = send_id;
            at_send_len = send_len;
            pDataLine = at_data_line;
            data_process_lock(LOCK_TCPIP, at_send_len);
            at_response_result(AT_RESULT_CODE_OK);
            msg_print_uart1("\r\n> ");
            ret = AT_RESULT_CODE_IGNORE;
            break;

        default :
            break;
    }

exit:
    at_response_result(ret);
    return ret;
}

void at_create_tcpip_tx_task_patch(void)
{
    osThreadDef_t task_def;
    osMessageQDef_t queue_def;
    
    if (at_tx_task_id != NULL)
        return;
    
    /* Create task */
    task_def.name = OS_TASK_NAME_AT_TX_DATA;
    task_def.stacksize = OS_TASK_STACK_SIZE_AT_TX_DATA_PATCH;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = at_data_tx_task;
    at_tx_task_id = osThreadCreate(&task_def, (void*)NULL);
    if(at_tx_task_id == NULL)
    {
        AT_LOGI("at_data Tx Task create fail \r\n");
    }
    else
    {
        AT_LOGI("at_data Tx Task create successful \r\n");
    }

    /* Create memory pool */
    at_tx_task_pool_id = osPoolCreate (osPool(at_tx_task_pool));
    if (!at_tx_task_pool_id)
    {
        printf("at_data TX Task Pool create Fail \r\n");
    }

    /* Create message queue*/
    queue_def.item_sz = sizeof(at_event_msg_t);
    queue_def.queue_sz = AT_DATA_TASK_QUEUE_SIZE;
    at_tx_task_queue_id = osMessageCreate(&queue_def, at_tx_task_id);
    if(at_tx_task_queue_id == NULL)
    {
        printf("at_data Tx create queue fail \r\n");
    }
}

int at_ip_send_data_patch(uint8_t *pdata, int send_len)
{
    at_socket_t *link = at_link_get_id(sending_id);
    int actual_send = 0;
    int i;
    char buf[64];

    if (at_ip_mode == false)
    {
        at_sprintf(buf, "\r\nRecv %d bytes\r\n", send_len);
        msg_print_uart1(buf);
    }
    
    for (i=0; i<send_len; i++) {
        printf("%c", pdata[i]);
    }
    printf("\r\n");
    
    if(link->sock < 0)
    {
        link->link_state = AT_LINK_DISCONNECTED;
        AT_LOGI("link_state is AT_LINK_DISCONNECTED\r\n");
        return -1;
    }

    if (link->link_type == AT_LINK_TYPE_TCP) {
        actual_send = lwip_write(link->sock, pdata, send_len);
        if (actual_send <= 0) {
            at_show_socket_error_reason("client send", link->sock);
            return -1;
        } else {
            AT_LOGI("id:%d,Len:%d,dp:%p\r\n", sending_id, send_len, pdata);
        }
    } else if (link->link_type == AT_LINK_TYPE_UDP) {

        struct sockaddr_in addr;
        memset(&addr, 0x0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(link->remote_port);
        inet_addr_from_ip4addr(&addr.sin_addr, ip_2_ip4(&link->remote_ip));

        AT_LOGI("sock=%d, udp send data to server IP:%s, Port:%d...",
             link->sock, ipaddr_ntoa(&link->remote_ip), link->remote_port);


        actual_send = sendto(link->sock, pdata, send_len, 0, (struct sockaddr *)&addr, sizeof(addr));
        if (actual_send <= 0) {
            at_show_socket_error_reason("client send", link->sock);
            return -1;
        } else {
            AT_LOGI("id:%d,Len:%d,dp:%p\r\n", sending_id, send_len, pdata);
        }
    }

    return 0;
}

void at_socket_process_task_patch(void *arg)
{
    at_socket_t* plink = (at_socket_t*)arg;
    bool link_count_flag = false;

    if (plink == NULL) {
        plink->task_handle = NULL;
        vTaskDelete(NULL);
    }

    //xSemaphoreTake(plink->sema,0);
    if ((plink->link_type == AT_LINK_TYPE_TCP) || (plink->link_type == AT_LINK_TYPE_SSL)) {
        link_count_flag = true;
        at_update_link_count(1);
    }

    for(;;)
    {
        if ((plink->link_state == AT_LINK_DISCONNECTED) || (plink->link_state == AT_LINK_DISCONNECTING)) {
            if (at_ip_mode == true) {
                if (at_create_tcp_client_trans(plink) < 0) {
                    vTaskDelay(300);
                    continue;
                }
            }
            else {
                break;
            }
        }
        at_process_recv_socket(plink);
        Hal_Wdt_Clear();
    }

    if (link_count_flag) {
        at_update_link_count(-1);
    }

    if (plink->recv_buf) {
        free(plink->recv_buf);
        plink->recv_buf = NULL;
        AT_LOGI("plink->recv_buf=%p\r\n", plink->recv_buf);
    }
    
    AT_LOGI("socket recv delete\r\n");

    if(plink->link_state == AT_LINK_DISCONNECTING) {
    	//xSemaphoreGive(plink->sema);
        plink->link_state = AT_LINK_DISCONNECTED;
    }

    plink->task_handle = NULL;
    vTaskDelete(NULL);
}

void at_data_tx_task_patch(void *arg)
{
    osEvent event;
    at_event_msg_t *pMsg;

    while(1)
    {
        event = osMessageGet(at_tx_task_queue_id, osWaitForever);
        if (event.status == osEventMessage)
        {
            pMsg = (at_event_msg_t*) event.value.p;
            switch(pMsg->event)
            {
                case AT_DATA_TX_EVENT:
                    AT_LOGI("at data event: %02X\r\n", pMsg->event);
                    if (at_ip_mode != true)
                    {
                        if (at_ip_send_data(pMsg->param, pMsg->length) < 0)
                            msg_print_uart1("\r\nSEND FAIL\r\n");
                        else
                            msg_print_uart1("\r\nSEND OK\r\n");
                        data_process_unlock();
                    }
                    else
                    {
                        at_ip_send_data(pMsg->param, pMsg->length);
                    }
                    break;
                case AT_DATA_TIMER_EVENT:
                    at_server_timeout_handler();
                    break;
                default:
                    AT_LOGI("FATAL: unknow at event: %02X\r\n", pMsg->event);
                    break;
            }
            if(pMsg->param != NULL)
                free(pMsg->param);
            osPoolFree(at_tx_task_pool_id, pMsg);
        }
    }
}

int at_trans_client_init(at_nvm_trans_config_t *trans_cfg)
{
    int ret = AT_RESULT_CODE_ERROR;
    at_socket_t *plink = NULL;
    
    if (trans_cfg == NULL) {
        goto exit;
    }
    
    if ((trans_cfg->enable != true) || (at_ip_mode != true)) {
        ret = AT_RESULT_CODE_OK;
        AT_LOGI("No saved links.\r\n");
        goto exit;
    }
    
    if ((trans_cfg->link_id != 0) || 
        (trans_cfg->link_type != AT_LINK_TYPE_TCP && trans_cfg->link_type != AT_LINK_TYPE_UDP) ||
        (at_strlen(trans_cfg->remote_ip) == 0)) {
        AT_LOGI("Saved links type error.\r\n");
        goto exit;
    }
    
    plink = at_link_get_id(trans_cfg->link_id);
    plink->link_id       = trans_cfg->link_id;
    plink->link_type     = trans_cfg->link_type;
    plink->remote_port   = trans_cfg->remote_port;
    plink->local_port    = trans_cfg->local_port;
    plink->change_mode   = trans_cfg->change_mode;
    plink->keep_alive    = trans_cfg->keep_alive;
    plink->send_mode     = AT_SEND_MODE_TRANSMIT;
    plink->terminal_type = AT_LOCAL_CLIENT;
    plink->repeat_time   = 0;
    plink->link_state    = AT_LINK_DISCONNECTED;
    
    switch (plink->link_type) {
        case AT_LINK_TYPE_TCP:
            if (at_create_tcp_client(plink, (char *)trans_cfg->remote_ip, plink->remote_port, 0) < 0)
            {
                AT_LOGI("Transparent create client failed.\r\n");
                at_close_client(plink);
                goto exit;
            }
            break;
        case AT_LINK_TYPE_UDP:
            if (at_create_udp_client(plink, (char *)trans_cfg->remote_ip, plink->remote_port, plink->local_port, 0) < 0)
            {
                AT_LOGI("Transparent create client failed.\r\n");
                at_close_client(plink);
                goto exit;
            }
            break;
        default:
            break;
    }
    
    if ((plink->recv_buf = (char *)malloc(AT_DATA_RX_BUFSIZE * sizeof(char))) == NULL) {
        AT_LOGI("rx buffer alloc fail\r\n");
        goto exit;
    }
    
    ret = AT_RESULT_CODE_OK;
    
    plink->link_state    = AT_LINK_TRANSMIT_SEND;
    
    at_socket_client_create_task(plink);
    
    at_create_tcpip_data_task();
    
exit:
    return ret;
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
int at_cmd_tcpip_savetranslink_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    uint8_t ret = AT_RESULT_CODE_ERROR;
    
    at_nvm_trans_config_t cfg;
    int enable = 0;
    int remote_port;
    uint8_t link_type = AT_LINK_TYPE_TCP;
    uint32_t ipaddr = 0;
    int keep_alive = 0;
    int local_port = 0;
    char *pStr = NULL;
    char *remote_ip = NULL;
    
    switch (mode) {
        case AT_CMD_MODE_READ:
            break;
        
        case AT_CMD_MODE_SET:
            if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS)) {
                AT_LOGI("at_cmd_buf_to_argc_argv fail\r\n");
                goto exit;
            }
            
            if (at_cmd_get_para_as_digital(argv[1], &enable) != 0) {
                goto exit;
            }
            
            if (enable > 1 || enable < 0) {
                goto exit;
            }
            
            if (enable == 1) {
                if (argc < 4) {
                    goto exit;
                }
                
                pStr = at_cmd_param_trim(argv[2]);
                if(!pStr) {
                    goto exit;
                }
                AT_LOGI("%s\r\n", pStr);
                
                remote_ip = pStr;
                ipaddr = inet_addr((char*)pStr);
                if (ipaddr == IPADDR_ANY)
                {
                    AT_LOGI("ipaddr is 0x%x\r\n", ipaddr);
                    goto exit;
                }
                
                if (ipaddr == IPADDR_NONE) {
                    if (at_strcmp(pStr, "255.255.255.255") == 0) {
                        AT_LOGI("ipaddr is 0x%x\r\n", ipaddr);
                        goto exit;
                    }
                }
                
                if (at_cmd_get_para_as_digital(argv[3], &remote_port) != 0) {
                    goto exit;
                }
                
                if((remote_port <=0) || (remote_port > 65535))
                {
                    AT_LOGI("remote_port is %d\r\n", remote_port);
                    goto exit;
                }
                
                if (argc >= 5) {
                    pStr = at_cmd_param_trim(argv[4]);
                    if(!pStr) {
                        goto exit;
                    }
                    
                    if (at_strcmp(pStr, "TCP") == 0) {
                        link_type = AT_LINK_TYPE_TCP;
                    }
                    else if (at_strcmp(pStr, "UDP") == 0) {
                        link_type = AT_LINK_TYPE_UDP;
                    }
                    else {
                        AT_LOGI("Link type ERROR\r\n");
                        goto exit;
                    }
                }
                
                if (argc >= 6) {
                    if (link_type == AT_LINK_TYPE_TCP) {
                        if (at_cmd_get_para_as_digital(argv[5], &keep_alive) != 0) {
                            goto exit;
                        }
                        
                        if (keep_alive < 0 || keep_alive > 7200) {
                            goto exit;
                        }
                    } else if (link_type == AT_LINK_TYPE_UDP) {
                        if (at_cmd_get_para_as_digital(argv[5], &local_port) != 0) {
                            goto exit;
                        }
                        
                        if (local_port <= 0 || local_port > 65535) {
                            goto exit;
                        }
                    }
                }
            }
            
            memset(&cfg, 0, sizeof(at_nvm_trans_config_t));
            cfg.enable = enable;
            cfg.link_id = 0;
            cfg.link_type = link_type;
            at_strncpy(cfg.remote_ip, remote_ip, sizeof(cfg.remote_ip));
            cfg.remote_port = remote_port;
            cfg.local_port  = local_port;
            cfg.keep_alive = keep_alive;
            
            if (at_cmd_nvm_trans_config_set(&cfg)) {
                AT_LOGI("Save transparent config failed. \r\n");
                goto exit;
            }
            
            ret = AT_RESULT_CODE_OK;
            break;
        
        default :
            ret = AT_RESULT_CODE_IGNORE;
            break;
    }
    
exit:
    at_response_result(ret);
    
    return true;
}
#endif /* #if defined(__AT_CMD_SUPPORT__) */

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
int _at_cmd_tcpip_cipstamac_patch(char *buf, int len, int mode)
{
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;
    uint8_t ret = AT_RESULT_CODE_ERROR;
    uint8_t mac[6] = {0};
    char temp[64]={0};
    char *pstr;

    switch (mode) {
        case AT_CMD_MODE_READ:
            wpa_cli_getmac(mac);
            at_sprintf(temp, "%s:\""MACSTR"\"\r\n", "+CIPSTAMAC",MAC2STR(mac));
            msg_print_uart1(temp);
            ret = AT_RESULT_CODE_OK;

            break;
        case AT_CMD_MODE_SET:
            if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS)) {
                AT_LOGI("at_cmd_buf_to_argc_argv fail\r\n");
                goto exit;
            }

            if (argc < 2) {
                goto exit;
            }

            pstr = at_cmd_param_trim(argv[1]);
            if (!pstr)
            {
                AT_LOGI("Invalid param\r\n");
                goto exit;
            }

            if (check_mac_addr_len(pstr) == -1) {
                AT_LOGI("Invalid mac address, wrong length of mac address \r\n");
                goto exit;
            }

            if (hwaddr_aton2(pstr, mac) == -1) {
                AT_LOGI("Invalid mac address \r\n");
                goto exit;
            }

            if (is_broadcast_ether_addr(mac)) {
                AT_LOGI("Invalid mac address, all of mac if 0xFF \r\n");
                goto exit;
            }

            if (is_multicast_ether_addr(mac)) {
                AT_LOGI("Invalid mac address, not allow multicast mac address \r\n");
                goto exit;
            }

            if (is_zero_ether_addr(mac)) {
                AT_LOGI("Invalid mac address, all of mac is zero. \r\n");
                goto exit;
            }

            wpa_cli_setmac(mac);

            mac_addr_set_config_source(MAC_IFACE_WIFI_STA, MAC_SOURCE_FROM_FLASH);

            ret = AT_RESULT_CODE_OK;

            break;
        default :
            ret = AT_RESULT_CODE_IGNORE;
            break;
    }

exit:
    at_response_result(ret);

    return true;
}

/*-------------------------------------------------------------------------------------
 * Definitions of interface function pointer
 *------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------
 * Interface assignment
 *------------------------------------------------------------------------------------*/


void _at_cmd_tcpip_func_init_patch(void)
{
    #if defined(__AT_CMD_SUPPORT__)
    g_server_mode = 0;
    g_server_port = 0;
    at_trans_timer = NULL;
    
    at_update_link_count                = at_update_link_count_patch;
    at_close_client                     = at_close_client_patch;
    at_process_recv_socket              = at_process_recv_socket_patch;
    at_socket_client_cleanup_task       = at_socket_client_cleanup_task_patch;
    at_socket_server_listen_task        = at_socket_server_listen_task_patch;
    at_socket_server_create_task        = at_socket_server_create_task_patch;
    at_socket_server_cleanup_task       = at_socket_server_cleanup_task_patch;
    at_create_tcpip_tx_task             = at_create_tcpip_tx_task_patch;
    at_socket_process_task              = at_socket_process_task_patch;
    at_data_tx_task                     = at_data_tx_task_patch;
    at_ip_send_data                     = at_ip_send_data_patch;
    at_server_timeout_handler           = at_server_timeout_handler_patch;
    
    /** Command Table (TCP/IP) */
    _g_AtCmdTbl_Tcpip_Ptr[0].cmd_handle  = _at_cmd_tcpip_cipstatus_patch;
    _g_AtCmdTbl_Tcpip_Ptr[2].cmd_handle  = _at_cmd_tcpip_cipstart_patch;
    _g_AtCmdTbl_Tcpip_Ptr[3].cmd_handle  = at_cmd_tcpip_cipsend_patch;
    _g_AtCmdTbl_Tcpip_Ptr[5].cmd_handle  = _at_cmd_tcpip_cipclose_patch;
    _g_AtCmdTbl_Tcpip_Ptr[7].cmd_handle  = at_cmd_tcpip_cipmux_patch;
    _g_AtCmdTbl_Tcpip_Ptr[8].cmd_handle  = _at_cmd_tcpip_cipserver_patch;
    _g_AtCmdTbl_Tcpip_Ptr[9].cmd_handle  = at_cmd_tcpip_cipmode_patch;
    _g_AtCmdTbl_Tcpip_Ptr[10].cmd_handle  = at_cmd_tcpip_savetranslink_patch;
    #endif
    _g_AtCmdTbl_Tcpip_Ptr[16].cmd_handle = _at_cmd_tcpip_cipstamac_patch;
}
