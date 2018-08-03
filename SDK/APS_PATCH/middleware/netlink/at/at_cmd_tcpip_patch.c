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

extern volatile at_state_type_t mdState;
extern volatile bool at_ip_mode;
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

void at_process_recv_socket_patch(at_socket_t *plink)
{
    struct sockaddr_in sa;
    socklen_t socklen = sizeof(struct sockaddr_in);
    ip_addr_t remote_ip;
    uint16_t remote_port;
    char *rcv_buf = NULL;
    int len = 0;
    int32_t sock = -1;
    char temp[40];

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

        } else {
            char* data = (char*)malloc(40);
            uint32_t header_len = 0;

            if (data) {
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
                        header_len = sprintf(data, "\r\n+IPD,%d,%d,"IPSTR",%d:",plink->link_id, len,
                                IP2STR(&remote_ip), remote_port);
                    }
                    else {
                        header_len = sprintf(data, "\r\n+IPD,%d,"IPSTR",%d:", len,
                                IP2STR(&remote_ip), remote_port);

                    }
                } else {
                    if (at_ipMux) {
                        header_len = sprintf(data,"\r\n+IPD,%d,%d:",plink->link_id, len);
                    } else {
                        header_len = sprintf(data,"\r\n+IPD,%d:",len);
                    }
                }

                // Send +IPD info
                at_uart1_write_buffer(data, header_len);
                // Send data
                at_uart1_write_buffer(rcv_buf, len);
                free(data);
                data = NULL;

                if (plink->link_state != AT_LINK_WAIT_SENDING) {
                    plink->link_state = AT_LINK_CONNECTED;
                }
                if ((plink->sock >= 0) && (plink->terminal_type == AT_REMOTE_CLIENT)) {
                    plink->server_timeout = 0;
                }
            } else {
                printf("alloc fail\r\n");
            }
        }
    }
    else
    {
        //len = 0, Connection close
        //len < 0, error
        if(plink->sock >= 0) {
            if ((at_ip_mode != TRUE) || (plink->link_state != AT_LINK_TRANSMIT_SEND)) {
                if (at_ipMux == TRUE) {
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
            }
        }
    }

    free(rcv_buf);
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
    int state;

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

            state = wpas_get_state();
            if(state == WPA_COMPLETED || state == WPA_ASSOCIATED) {
                AT_LOGI("In connected, set mac address failed\r\n");
                ret = AT_RESULT_CODE_FAIL;
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


/*-------------------------------------------------------------------------------------
 * Definitions of interface function pointer
 *------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------
 * Interface assignment
 *------------------------------------------------------------------------------------*/


void _at_cmd_tcpip_func_init_patch(void)
{
    at_update_link_count                = at_update_link_count_patch;
    at_close_client                     = at_close_client_patch;
    at_process_recv_socket              = at_process_recv_socket_patch;
    at_socket_client_cleanup_task       = at_socket_client_cleanup_task_patch;
    at_socket_server_listen_task        = at_socket_server_listen_task_patch;
    at_socket_server_create_task        = at_socket_server_create_task_patch;
    at_socket_server_cleanup_task       = at_socket_server_cleanup_task_patch;

    /** Command Table (TCP/IP) */
    _g_AtCmdTbl_Tcpip_Ptr[0].cmd_handle  = _at_cmd_tcpip_cipstatus_patch;
    _g_AtCmdTbl_Tcpip_Ptr[2].cmd_handle  = _at_cmd_tcpip_cipstart_patch;
    _g_AtCmdTbl_Tcpip_Ptr[5].cmd_handle  = _at_cmd_tcpip_cipclose_patch;
    _g_AtCmdTbl_Tcpip_Ptr[16].cmd_handle = _at_cmd_tcpip_cipstamac_patch;
}

