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
#include <string.h>
#include "iperf_task.h"
#include "ctype.h"
#include "msg.h"
#include "cmsis_os.h"
#include "lwip/api.h" //netconn API
#include "lwip/sockets.h" //socket API


enum {
    kConv_Unit,
    kConv_Kilo,
    kConv_Mega,
    kConv_Giga
};

/* factor to multiply the number by */
const double kConversion[] =
{
    1.0,                       /* unit */
    1.0 / 1024,                /* kilo */
    1.0 / 1024 / 1024,         /* mega */
    1.0 / 1024 / 1024 / 1024   /* giga */
};

/* factor to multiply the number by for bits*/
const double kConversionForBits[] =
{
    1.0,                       /* unit */
    1.0 / 1000,                /* kilo */
    1.0 / 1000 / 1000,         /* mega */
    1.0 / 1000 / 1000 / 1000   /* giga */
};

/* labels for Byte formats [KMG] */
const char* kLabel_Byte[] =
{
    "Byte",
    "KByte",
    "MByte",
    "GByte"
};

/* labels for bit formats [kmg] */
const char* kLabel_bit[]  =
{
    "bit",
    "Kbit",
    "Mbit",
    "Gbit"
};

typedef struct _iperf_context
{
    uint32_t server_addr;
    uint32_t port;
    uint32_t buffer_len;
    uint32_t win_band;
    uint32_t amount;
    iperf_result_t result_t;
    iperf_callback_t callback;
}iperf_context_t;

// Private variables -----------------------------------------------------------
uint32_t g_iperf_debug_feature = 0;
int g_iperf_is_tradeoff_test_client = 0;
int g_iperf_is_tradeoff_test_server = 0;
iperf_context_t g_iperf_context = {0};

//static uint32_t start_count = 0;
//static uint32_t end_count = 0;

#if 1
RET_DATA T_IperfCalculateResultFp iperf_calculate_result;
RET_DATA T_IperfDisplayReportFp iperf_display_report;
RET_DATA T_IperfResetCountFp iperf_reset_count;
RET_DATA T_IperfCopyCountFp iperf_copy_count;
RET_DATA T_IperfDiffCountFp iperf_diff_count;
RET_DATA T_IperfFtoaFp iperf_ftoa;
RET_DATA T_IperfByteSnprintfFp iperf_byte_snprintf;
RET_DATA T_IperfGetCurrentTimeFp iperf_get_current_time;
RET_DATA T_IperfPatternFp iperf_pattern;

RET_DATA T_IperfCommomFp iperf_udp_run_server;
RET_DATA T_IperfCommomFp iperf_tcp_run_server;
RET_DATA T_IperfCommomFp iperf_udp_run_client;
RET_DATA T_IperfCommomFp iperf_tcp_run_client;
RET_DATA T_IperfSetDebugModeFp iperf_set_debug_mode;
RET_DATA T_IperfRegisterCallbackFp iperf_register_callback;
RET_DATA T_IperfFormatTransformFp iperf_format_transform;
#else
// Private function prototypes -------------------------------------------------
static void iperf_calculate_result(int pkt_size, count_t *pkt_count);
static void iperf_display_report(char *report_title, unsigned time, unsigned h_ms_time, count_t *pkt_count);
static void iperf_reset_count(count_t *pkt_count);
static void iperf_copy_count(count_t *pkt_count_src, count_t *pkt_count_dest);
static void iperf_diff_count(count_t *result_count, count_t *pkt_count, count_t *tmp_count);
static char* iperf_ftoa(double f, char * buf, int precision);
static int byte_snprintf(char* outString, double inNum, char inFormat);
static void iperf_get_current_time(uint32_t *s, uint32_t *ms);
static void iperf_pattern(char *outBuf, int inBytes);
// Private functions -----------------------------------------------------------
#endif

void iperf_udp_run_server_impl(char *parameters[])
{

    int sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    int cli_len;
#if LWIP_IGMP
    struct ip_mreq group;
#endif
    int server_port;
    int i;
    count_t pkt_count;
    count_t tmp_count;
    int nbytes = 0; /* the number of read */
    int send_bytes = 0; /* the number of send */
#if LWIP_IGMP
    char *mcast;
    int mcast_tag = 0; /* the tag of parameter "-B"  */
#endif
    int interval_tag = 0; /* the tag of parameter "-i"  */
#if defined(IPERF_DEBUG_ENABLE)
    int tmp = 0;
#endif
    char *buffer = NULL;
    uint32_t t1, t2 , curr_t, curr_h_ms, t2_h_ms, t1_h_ms, tmp_t, tmp_h_ms, offset_t1, offset_t2, offset_time;
    UDP_datagram *udp_h;
    client_hdr *client_h;
    client_hdr client_h_trans;
    struct timeval timeout;
    timeout.tv_sec = 20; //set recvive timeout = 20(sec)
    timeout.tv_usec = 0;
    int is_test_started = 0;
    int udp_h_id = 0;
    t2_h_ms = 0;
    t1_h_ms = 0;
    offset_time = 0;

    //Statistics init
    iperf_reset_count(&pkt_count);
    iperf_reset_count(&tmp_count);
    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_count);
    server_port = 0;
    t1 = 0;
    t2 = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    int data_size = IPERF_TEST_BUFFER_SIZE;

    //Handle input parameters
    if (g_iperf_is_tradeoff_test_client == 0) {
        for (i = 0; i < 13; i++) {
            if (strcmp((char *)&parameters[i * offset], "-p") == 0) {
                i++;
                server_port = atoi((char *)&parameters[i * offset]);
            } else if (strcmp((char *)&parameters[i * offset], "-B") == 0) {
                i++;
#if LWIP_IGMP
                mcast = (char *)&parameters[i * offset];
                mcast_tag = 1;
                IPERF_LOGI("Join Multicast %s \n", mcast);
#endif
            } else if (strcmp((char *)&parameters[i * offset], "-i") == 0) {
                interval_tag = 1;
                IPERF_LOGI("Set 10 seconds between periodic bandwidth reports\n");
            } else if (strcmp((char *)&parameters[i * offset], "-l") == 0) {
                i++;
                data_size = iperf_format_transform((char *)&parameters[i * offset]);
                IPERF_LOGI("Set buffer size = %d Bytes\n", data_size);
                if (data_size > IPERF_TEST_BUFFER_SIZE ) {
                    data_size = IPERF_TEST_BUFFER_SIZE;
                    IPERF_LOGI("Upper limit of buffer size = %d Bytes\n", IPERF_TEST_BUFFER_SIZE);
                } else if (data_size < (sizeof(UDP_datagram) + sizeof(client_hdr))) {
                    data_size = sizeof(UDP_datagram) + sizeof(client_hdr);
                    IPERF_LOGI("Lower limit of buffer size = %d Bytes\n", data_size);
                }
            }
        }
    }

    // Create a new UDP connection handle
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        IPERF_LOGI("[%s:%d] sockfd = %d\n", __FUNCTION__, __LINE__, sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        vTaskDelete(NULL);
    }

    socklen_t len = sizeof(timeout);
    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, len) < 0) {
        IPERF_LOGI("Setsockopt failed - cancel receive timeout\n");
    }

    // Bind to port and any IP address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (server_port == 0) {
        servaddr.sin_port = htons(IPERF_DEFAULT_PORT);
        IPERF_LOGI("Default server port = %d \n", IPERF_DEFAULT_PORT);
    } else {
        servaddr.sin_port = htons(server_port);
        IPERF_LOGI("Set server port = %d \n", server_port);
    }
#if LWIP_IGMP
    //Multicast settings
    if (mcast_tag == 1) {
        group.imr_multiaddr.s_addr = inet_addr(mcast);
        group.imr_interface.s_addr = htonl(INADDR_ANY);

        if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(struct ip_mreq)) < 0) {
            IPERF_LOGI("Setsockopt failed - multicast settings\n");
        }
    }
#endif
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
        IPERF_LOGI("[%s:%d]\n", __FUNCTION__, __LINE__);
        close(sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        vTaskDelete(NULL);
    }

    cli_len = sizeof(cliaddr);
    buffer = pvPortMalloc(IPERF_TEST_BUFFER_SIZE);
    if (buffer == NULL) {
        IPERF_LOGI("not enough buffer to send data!\n");
        close(sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        vTaskDelete(NULL);
    }
    memset(buffer, 0, IPERF_TEST_BUFFER_SIZE);
    // Wait and check the request
    do {
        // Handles request
        do {
            iperf_get_current_time(&offset_t1, 0);
            nbytes = recvfrom(sockfd, buffer, data_size, /*MSG_TRUNC*/0x20, (struct sockaddr *)&cliaddr, (socklen_t *)&cli_len);
            iperf_get_current_time(&offset_t2, 0);

            //if connected to iperf v2.0.1, there is no end package sent from client side
            if ((offset_t2 > (offset_t1 + 2)) && (nbytes <= 0) && (pkt_count.times >= 1)) {
                offset_time = offset_t2 - offset_t1;
            } else if (offset_time != 0) {
                offset_time = 0;
            }

            udp_h = (UDP_datagram *)buffer;
            udp_h_id = (int)ntohl(udp_h->id);

#if defined(IPERF_DEBUG_INTERNAL)
            client_h = (client_hdr *)&buffer[12];
            client_h_trans.flags = (int32_t)(ntohl(client_h->flags));
            client_h_trans.num_threads = (int32_t)(ntohl(client_h->num_threads));
            client_h_trans.port = (int32_t)(ntohl(client_h->port));
            client_h_trans.buffer_len = (int32_t)(ntohl(client_h->buffer_len));
            client_h_trans.win_band = (int32_t)(ntohl(client_h->win_band));
            client_h_trans.amount = (int32_t)(ntohl(client_h->amount));

            DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("UDP server, receive from sockfd \"%d\", id is \"%d\", tv_sec is \"%d\", tv_usec is \"%d\", nbytes is \"%d\"\n",
                                                 sockfd, udp_h_id, ntohl(udp_h->tv_sec), ntohl(udp_h->tv_usec), nbytes));
            DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("UDP server, receive from sin_len = %d, sin_family = %d , port = %d, s_addr = 0x%x\n", cliaddr.sin_len, cliaddr.sin_family,
                                                 cliaddr.sin_port, cliaddr.sin_addr.s_addr));
            DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("[%s:%d] t1 = %d, t2 = %d\n", __FUNCTION__, __LINE__, t1, t2));

            DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("[%s:%d], client_h_trans.flag = %d, num_threads = %d, port = %d, buffer_len = %d, win_band = %d, amount = %d\n"
                                                 , __FUNCTION__, __LINE__, client_h_trans.flags, client_h_trans.num_threads, client_h_trans.port, client_h_trans.buffer_len, client_h_trans.win_band, client_h_trans.amount));
#endif

#if defined(IPERF_DEBUG_ENABLE)
            if (tmp != nbytes) {
                DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("\n[%s:%d] nbytes=%d \n", __FUNCTION__, __LINE__, nbytes));
            } else {
                DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("."));
            }
            tmp = nbytes;
#endif

            iperf_calculate_result(nbytes, &pkt_count);

            if (pkt_count.times == 1) {
                iperf_get_current_time(&t1, &t1_h_ms);
                t1_h_ms = (t1_h_ms / 100) % 10;
            }

            // Report by second
            if ((pkt_count.times >= 1 && interval_tag > 0)) {
                iperf_get_current_time(&curr_t, &curr_h_ms);
                curr_h_ms = (curr_h_ms / 100) % 10;

                if (offset_time > 0) {
                    curr_t -= offset_time;
                }

                if (curr_h_ms >= t1_h_ms) {
                    tmp_h_ms = curr_h_ms - t1_h_ms;
                    tmp_t = curr_t - t1;
                } else {
                    tmp_h_ms = curr_h_ms + 10 - t1_h_ms;
                    tmp_t = curr_t - t1 - 1;
                }

                if ((((curr_t - t1) / 10) == interval_tag) && ((curr_h_ms >= t1_h_ms) || ((curr_t - t1) % 10) >= 1)) {
                    count_t result_count;
                    IPERF_LOGI("\nInterval: %d.0 - %d.0 sec   ", (int)(curr_t - t1) / 10 * 10 - 10, (int)(curr_t - t1) / 10 * 10);
                    iperf_diff_count(&result_count, &pkt_count, &tmp_count);
                    iperf_display_report("UDP Server", 10, 0, &result_count);
                    iperf_copy_count(&pkt_count, &tmp_count);
                    interval_tag++;
                } else if (((udp_h_id < 0) || (nbytes <= 0)) &&
                           (((tmp_t) % 10) != 0) &&
                           (is_test_started == 1)) {
                    count_t result_count;
                    IPERF_LOGI("\nInterval: %d.0 - %d.%d sec   ", (int)(curr_t - t1 + 1) / 10 * 10 - 10, (int)tmp_t, (int)tmp_h_ms);
                    iperf_diff_count(&result_count, &pkt_count, &tmp_count);
                    iperf_display_report("UDP Server", (tmp_t - ((curr_t - t1 + 1) / 10 * 10 - 10)), tmp_h_ms, &result_count);
                    iperf_copy_count(&pkt_count, &tmp_count);
                    interval_tag++;
                }
            }

            if ((is_test_started == 0) && (udp_h_id > 0) && (nbytes > 0)) {
                is_test_started = 1;
            } else if (((udp_h_id < 0) || (nbytes <= 0)) && (is_test_started == 1)) { // the last package
                int32_t old_flag = 0;

                // test end, save the current time to "t2"
                if (pkt_count.times >= 1) {
                    /* sync the time if report by second */
                    if (interval_tag > 0) {
                        t2 = curr_t;
                        t2_h_ms = curr_h_ms;
                    } else {
                        iperf_get_current_time(&t2, &t2_h_ms);
                        t2_h_ms = (t2_h_ms / 100) % 10;
                        if (offset_time > 0) {
                            t2 -= offset_time;
                        }
                    }
                }

                // Calculate time: second
                if (t2_h_ms >= t1_h_ms) {
                    t2_h_ms = t2_h_ms - t1_h_ms;
                    t2 = t2 - t1;
                } else {
                    t2_h_ms = t2_h_ms + 10 - t1_h_ms;
                    t2 = t2 - t1 - 1;
                }
                // print out result
                iperf_display_report("[Total]UDP Server", t2, t2_h_ms, &pkt_count);


                //TODO: need to send the correct report to client-side, flag = 0 means the report is ignored.
                if (udp_h_id < 0) {
                    old_flag = client_h_trans.flags;
                    client_h_trans.flags = (int32_t)0;

                    // send the server report to client-side
                    send_bytes = sendto(sockfd, buffer, nbytes, 0, (struct sockaddr *)&cliaddr, cli_len);
                    (void)send_bytes;
                    client_h_trans.flags = old_flag;
                }

#if defined(IPERF_DEBUG_ENABLE)
                DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("[%s:%d]send_bytes = %d, nbytes = %d,\n", __FUNCTION__, __LINE__, send_bytes, nbytes));
#endif

                client_h = (client_hdr *)&buffer[12];
                client_h_trans.flags = (int32_t)(ntohl(client_h->flags));

                // Tradeoff mode
                if (IPERF_HEADER_VERSION1 & client_h_trans.flags) {
                    IPERF_LOGI("Tradeoff mode, client-side start.\n");

                    g_iperf_is_tradeoff_test_server = 1;
                    memset(&g_iperf_context, 0, sizeof(iperf_context_t));
                    g_iperf_context.server_addr = cliaddr.sin_addr.s_addr;
                    g_iperf_context.port = ntohl(client_h->port);
                    g_iperf_context.buffer_len = ntohl(client_h->buffer_len);
                    g_iperf_context.win_band = ntohl(client_h->win_band);
                    g_iperf_context.amount = ntohl(client_h->amount);
                    iperf_udp_run_client(NULL);
                    g_iperf_is_tradeoff_test_server = 0;

                }

                IPERF_LOGI("Data transfer is finished.\n");
                //TODO: send report to other side
                break;
            }
        } while (nbytes > 0);

#if defined(IPERF_DEBUG_ENABLE)
        DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("[%s:%d] Interval = %d.%d (secs)\n", __FUNCTION__, __LINE__, t2, t2_h_ms)); //sec.
#endif

    } while (0);
    if (buffer) {
        vPortFree(buffer);
    }
    IPERF_LOGI("\n UDP server close socket!\n");
    close(sockfd);

    IPERF_LOGI("If you want to execute iperf server again, please enter \"iperf -s -u\".\n");

    if (parameters) {
        vPortFree(parameters);
    }

    g_iperf_context.result_t.data_size = 0;
    g_iperf_context.result_t.send_time = 0;
    if (g_iperf_context.callback) {
        g_iperf_context.callback(&g_iperf_context.result_t);
    }
    // For tradeoff mode, task will be deleted in iperf_udp_run_client
    if (g_iperf_is_tradeoff_test_client == 0) {
        vTaskDelete(NULL);
    }
}


void iperf_tcp_run_server_impl(char *parameters[])
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    int server_port;
    int i;
    count_t pkt_count;
    count_t tmp_count;
    int nbytes = 0; /* the number of read */
    int total_rcv = 0; /* the total number of receive  */
    int num_tag = 0; /* the tag of parameter "-n"  */
    int interval_tag = 0; /* the tag of parameter "-i"  */
#if defined(IPERF_DEBUG_ENABLE)
    int tmp = 0;
#endif
    char *buffer = NULL;
    uint32_t t1, t2, curr_t;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    struct timeval timeout;
    timeout.tv_sec = 20; //set recvive timeout = 20(sec)
    timeout.tv_usec = 0;

    //Statistics init
    iperf_reset_count(&pkt_count);
    iperf_reset_count(&tmp_count);
    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_count);
    server_port = 0;

    //Handle input parameters
    for (i = 0; i < 9; i++) {
        if (strcmp((char *)&parameters[i * offset], "-p") == 0) {
            i++;
            server_port = atoi((char *)&parameters[i * offset]);
        } else if (strcmp((char *)&parameters[i * offset], "-n") == 0) {
            i++;
            total_rcv = iperf_format_transform((char *)&parameters[i * offset]);
            num_tag = 1;
            IPERF_LOGI("Set number to receive = %d Bytes\n", total_rcv);
        } else if (strcmp((char *)&parameters[i * offset], "-i") == 0) {
            interval_tag = 1;
            IPERF_LOGI("Set 10 seconds between periodic bandwidth reports\n");
        }
    }

    // Create a new TCP connection handle
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        IPERF_LOGI("[%s:%d] listenfd = %d\n", __FUNCTION__, __LINE__, listenfd);
        if (parameters) {
            vPortFree(parameters);
        }
        vTaskDelete(NULL);
    }

    socklen_t len = sizeof(timeout);
    if (setsockopt (listenfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, len) < 0) {
        IPERF_LOGI("Setsockopt failed - cancel receive timeout\n");
    }

    do {
        // Bind to port and any IP address
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (server_port == 0) {
            servaddr.sin_port = htons(IPERF_DEFAULT_PORT);
            IPERF_LOGI("Default server port = %d \n", IPERF_DEFAULT_PORT);
        } else {
            servaddr.sin_port = htons(server_port);
            IPERF_LOGI("Set server port = %d \n", server_port);
        }

        if ((bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
            IPERF_LOGI("[%s:%d]\n", __FUNCTION__, __LINE__);
            break;
        }

        // Put the connection into LISTEN state
        if ((listen(listenfd, 1024)) < 0) {
            IPERF_LOGI("[%s:%d]\n", __FUNCTION__, __LINE__);
            break;
        }
        buffer = pvPortMalloc(IPERF_TEST_BUFFER_SIZE);
        if (buffer == NULL) {
            IPERF_LOGI("not enough buffer to send data!\n");
            close(listenfd);
            if (parameters) {
                vPortFree(parameters);
            }
            vTaskDelete(NULL);
        }
        memset(buffer, 0, IPERF_TEST_BUFFER_SIZE);
        do {
            if (server_port != 0) {
                IPERF_LOGI("Listen...(port = %d)\n", server_port);
            } else {
                IPERF_LOGI("Listen...(port = %d)\n", IPERF_DEFAULT_PORT);
            }
            // Block and wait for an incoming connection
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) != -1) {
                IPERF_LOGI("[%s:%d] Accept... (sockfd=%d)\n", __FUNCTION__, __LINE__, connfd);

                //Connection
                do {
                    nbytes = recv(connfd, buffer, IPERF_TEST_BUFFER_SIZE, 0);
                    iperf_calculate_result(nbytes, &pkt_count);
                    if (pkt_count.times == 1) {
                        iperf_get_current_time(&t1, 0);
                    }
#if defined(IPERF_DEBUG_ENABLE)
                    if (tmp != nbytes) {
                        DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("\n[%s:%d] nbytes=%d \n", __FUNCTION__, __LINE__, nbytes));
                    } else {
                        DBGPRINT_IPERF(IPERF_DEBUG_RECEIVE, ("."));
                    }
                    tmp = nbytes;
#endif
                    if (num_tag == 1) {
                        total_rcv -= nbytes;
                    }

                    //Reach total receive number "-n"
                    if (total_rcv < 0) {
                        IPERF_LOGI("Finish Receiving \n");
                        break;
                    }
                    if (pkt_count.times >= 1 && interval_tag > 0) {
                        iperf_get_current_time(&curr_t, 0);
                        if (((curr_t - t1) / 10) == interval_tag) {
                            count_t result_count;
                            IPERF_LOGI("\nInterval: %d - %d sec   ", (int)(curr_t - t1) / 10 * 10 - 10, (int)(curr_t - t1) / 10 * 10);
                            iperf_diff_count(&result_count, &pkt_count, &tmp_count);
                            iperf_display_report("TCP Server", 10, 0, &result_count);
                            iperf_copy_count(&pkt_count, &tmp_count);
                            interval_tag++;
                        }
                    }
                } while (nbytes > 0);

                if (pkt_count.times >= 1) {
                    iperf_get_current_time(&t2, 0);
                }


                IPERF_LOGI("\nClose socket!\n");
                //Get report
                iperf_display_report("[Total]TCP Server", t2 - t1, 0, &pkt_count);

                g_iperf_context.result_t.data_size = 0;
                g_iperf_context.result_t.send_time = 0;
                if (g_iperf_context.callback) {
                    g_iperf_context.callback(&g_iperf_context.result_t);
                }
                //Statistics init
                iperf_reset_count(&pkt_count);
                iperf_reset_count(&tmp_count);
                if (interval_tag > 0) {
                    interval_tag = 1;
                } else {
                    interval_tag = 0;
                }

                close(connfd);
            }
        } while (connfd != -1 && num_tag == 0);

        close(listenfd);
        if (num_tag == 0) {
            IPERF_LOGI("\nClose socket!\n");
            iperf_display_report("[Total]TCP Server ", t2 - t1, 0, &pkt_count);
        }
    } while (0); //Loop just once
    if (buffer) {
        vPortFree(buffer);
    }
    IPERF_LOGI("If you want to execute iperf server again, please enter \"iperf -s\".\n");

    if (parameters) {
        vPortFree(parameters);
    }

    vTaskDelete(NULL);
}


void iperf_tcp_run_client_impl(char *parameters[])
{

    int sockfd;
    struct sockaddr_in servaddr;
    char *Server_IP;
    count_t pkt_count;
    count_t tmp_count;
    int nbytes = 0; /* the number of send */
    int total_send = 0; /* the total number of transmit  */
    int num_tag = 0; /* the tag of parameter "-n"  */
    int interval_tag = 0; /* the tag of parameter "-i"  */
    char *str = NULL;
    int i;
    int win_size, send_time, server_port, pkt_delay, tos;
    uint32_t t1, t2, curr_t;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    //Statistics init
    iperf_reset_count(&pkt_count);
    iperf_reset_count(&tmp_count);
    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_count);
    win_size = 0;
    send_time = 0;
    server_port = 0;
    pkt_delay = 0;
    tos = 0;

    //Handle input parameters
    Server_IP = (char *)&parameters[0];

    for (i = 1; i < 18; i++) {
        if (strcmp((char *)&parameters[i * offset], "-l") == 0) {
            i++;
            win_size = iperf_format_transform((char *)&parameters[i * offset]);
            IPERF_LOGI("Set window size = %d Bytes\n", win_size);
        }

        else if (strcmp((char *)&parameters[i * offset], "-t") == 0) {
            i++;
            send_time = atoi((char *)&parameters[i * offset]);
            IPERF_LOGI("Set send times = %d (secs)\n", atoi((char *)&parameters[i * offset]));

        }

        else if (strcmp((char *)&parameters[i * offset], "-p") == 0) {
            i++;
            server_port = atoi((char *)&parameters[i * offset]);

        }

        else if (strcmp((char *)&parameters[i * offset], "-d") == 0) {
            i++;
            pkt_delay = atoi((char *)&parameters[i * offset]);
            IPERF_LOGI("Set packet delay = %d (ms)\n", atoi((char *)&parameters[i * offset]));

        } else if (strcmp((char *)&parameters[i * offset], "-n") == 0) {
            i++;
            total_send = iperf_format_transform((char *)&parameters[i * offset]);
            num_tag = 1;
            IPERF_LOGI("Set number to transmit = %d Bytes\n", total_send);
        } else if (strcmp((char *)&parameters[i * offset], "-S") == 0) {
            i++;
            tos = atoi((char *)&parameters[i * offset]);
            IPERF_LOGI("Set TOS = %d \n", atoi((char *)&parameters[i * offset]));
        } else if (strcmp((char *)&parameters[i * offset], "-i") == 0) {
            interval_tag = 1;
            IPERF_LOGI("Set 10 seconds between periodic bandwidth reports\n");
        }
    }

    if (win_size == 0) {
        win_size = 1460;
        IPERF_LOGI("Default window size = %d Bytes\n", win_size);
    }
    if (send_time == 0) {
        if (num_tag == 1) {
            send_time = 999999;
        } else {
            send_time = 10;
            IPERF_LOGI("Default send times = %d (secs)\n", send_time);
        }
    }

    // Create a new TCP connection handle
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        IPERF_LOGI("[%s:%d] sockfd = %d\n", __FUNCTION__, __LINE__, sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        vTaskDelete(NULL);
    }

    if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0) {
        IPERF_LOGI("Set TOS: fail!\n");
    }

    // Bind to port and IP
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(Server_IP);
    if (server_port == 0) {
        servaddr.sin_port = htons(IPERF_DEFAULT_PORT);
        IPERF_LOGI("Default server port = %d \n", IPERF_DEFAULT_PORT);
    } else {
        servaddr.sin_port = htons(server_port);
        IPERF_LOGI("Set server port = %d \n", server_port);
    }

    if ((connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
        IPERF_LOGI("Connect failed, sockfd is %d, addr is \"%s\"\n", (int)sockfd, ((struct sockaddr *)&servaddr)->sa_data);
        close(sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        if (g_iperf_context.callback)
            g_iperf_context.callback(NULL);
        vTaskDelete(NULL);
    }

    iperf_get_current_time(&t1, 0);

    str = pvPortMalloc(1 * IPERF_TEST_BUFFER_SIZE);
    if (str == NULL) {
        IPERF_LOGI("not enough buffer to send data!\n");
        close(sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        if (g_iperf_context.callback)
            g_iperf_context.callback(NULL);
        vTaskDelete(NULL);
    }
    memset(str, 0, IPERF_TEST_BUFFER_SIZE);
    iperf_pattern(str, IPERF_TEST_BUFFER_SIZE);
    do {
        nbytes = send(sockfd, str, win_size, 0);
        iperf_calculate_result(nbytes, &pkt_count);
#if defined(IPERF_DEBUG_ENABLE)
        DBGPRINT_IPERF(IPERF_DEBUG_SEND, ("\n[%s:%d] nbytes=%d \n", __FUNCTION__, __LINE__, nbytes));
#endif
        vTaskDelay(pkt_delay);
        if (num_tag == 1) {
            total_send -= nbytes;
        }
        //Reach total receive number "-n"
        if (total_send < 0) {
            IPERF_LOGI("Finish Sending \n");
            break;
        }

        if (interval_tag > 0) {
            iperf_get_current_time(&curr_t, 0);

            if (((curr_t - t1) / 10) == interval_tag) {
                count_t result_count;
                IPERF_LOGI("\nInterval: %d - %d sec   ", (int)(curr_t - t1) / 10 * 10 - 10, (int)(curr_t - t1) / 10 * 10);
                iperf_diff_count(&result_count, &pkt_count, &tmp_count);
                iperf_display_report("TCP Client", 10, 0, &result_count);
                iperf_copy_count(&pkt_count, &tmp_count);
                interval_tag++;
            }
        }

        iperf_get_current_time(&curr_t, 0);
    } while ( (curr_t - t1) < send_time );

    iperf_get_current_time(&t2, 0);
    if (str) {
        vPortFree(str);
    }
    close(sockfd);
    IPERF_LOGI("\nClose socket!\n");

    iperf_display_report("[Total]TCP Client", t2 - t1, 0, &pkt_count);

    if (parameters) {
        vPortFree(parameters);
    }

    g_iperf_context.result_t.data_size = win_size;
    g_iperf_context.result_t.send_time = send_time;
    if (g_iperf_context.callback)
        g_iperf_context.callback(&g_iperf_context.result_t);

    vTaskDelete(NULL);

}


void iperf_udp_run_client_impl(char *parameters[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    char *Server_IP = 0;
    count_t pkt_count;
    count_t tmp_count;
    int nbytes = 0; /* the number of send */
    int total_send = 0; /* the total number of transmit  */
    int num_tag = 0; /* the tag of parameter "-n"  */
    int interval_tag = 0; /* the tag of parameter "-i"  */
    int tradeoff_tag = 0; /* the tag of parameter "-r"  */
    char *str = NULL;
    int i;
    int data_size, send_time, server_port, pkt_delay, pkt_delay_offset, tos, bw;
    uint32_t t1, t2, curr_t, t1_ms, last_tick, current_tick, last_sleep, current_sleep;
    UDP_datagram *udp_h;
    client_hdr *client_h;
    int udp_h_id = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    //Statistics init
    iperf_reset_count(&pkt_count);
    iperf_reset_count(&tmp_count);
    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_count);
    data_size = 0;
    send_time = 0;
    server_port = 0;
    pkt_delay = 0;
    pkt_delay_offset = 0;
    tos = 0;
    bw = 0;

    //Handle input parameters
    if (g_iperf_is_tradeoff_test_server == 0) {
        Server_IP = (char *)&parameters[0];
        for (i = 1; i < 18; i++) {
            if (strcmp((char *)&parameters[i * offset], "-l") == 0) {
                i++;
                data_size = iperf_format_transform((char *)&parameters[i * offset]);
                IPERF_LOGI("Set datagram size = %d Bytes\n", data_size);
            }

            else if (strcmp((char *)&parameters[i * offset], "-t") == 0) {
                i++;
                send_time = atoi((char *)&parameters[i * offset]);
                IPERF_LOGI("Set send times = %d (secs)\n", atoi((char *)&parameters[i * offset]));
            }

            else if (strcmp((char *)&parameters[i * offset], "-p") == 0) {
                i++;
                server_port = atoi((char *)&parameters[i * offset]);
            }

            else if (strcmp((char *)&parameters[i * offset], "-d") == 0) {
                i++;
                pkt_delay = atoi((char *)&parameters[i * offset]);
                IPERF_LOGI("Set packet delay = %d (ms)\n", atoi((char *)&parameters[i * offset]));
            } else if (strcmp((char *)&parameters[i * offset], "-n") == 0) {
                i++;
                total_send = iperf_format_transform((char *)&parameters[i * offset]);
                num_tag = 1;
                IPERF_LOGI("Set number to transmit = %d Bytes\n", total_send);
            } else if (strcmp((char *)&parameters[i * offset], "-S") == 0) {
                i++;
                tos = atoi((char *)&parameters[i * offset]);
                IPERF_LOGI("Set TOS = %d \n", atoi((char *)&parameters[i * offset]));
            } else if (strcmp((char *)&parameters[i * offset], "-b") == 0) {
                i++;
                IPERF_LOGI("Set bandwidth = %s\n", (char *)&parameters[i * offset]);
                bw = iperf_format_transform((char *)&parameters[i * offset]) / 8;
                if (bw > 2621440 || bw <= 0) {
                    bw = 2621440;
                    IPERF_LOGI("Upper limit of bandwith setting = 20Mbits/sec\n");
                }
                IPERF_LOGI("bandwidth = %d\n", bw);
            } else if (strcmp((char *)&parameters[i * offset], "-i") == 0) {
                interval_tag = 1;
                IPERF_LOGI("Set 10 seconds between periodic bandwidth reports\n");
            } else if (strcmp((char *)&parameters[i * offset], "-r") == 0) {
                tradeoff_tag = 1;
                IPERF_LOGI("Set to tradeoff mode\n");
            }
        }
    }

    // Bind to port and IP
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    if (g_iperf_is_tradeoff_test_server == 0) {
        servaddr.sin_addr.s_addr = inet_addr(Server_IP);
    } else {
        servaddr.sin_addr.s_addr = g_iperf_context.server_addr;
        server_port = g_iperf_context.port;
        bw = g_iperf_context.win_band / 8;
        total_send = g_iperf_context.amount;
        num_tag = 1;
    }
    IPERF_LOGI("Server address = %x \n", (unsigned int)servaddr.sin_addr.s_addr);

    if (data_size == 0) {
        data_size = 1460;
        IPERF_LOGI("Default datagram size = %d Bytes\n", data_size);
    }

    g_iperf_context.result_t.data_size = data_size;

    if(!bw)
    {
        bw = 1024;
    }

    if (bw > 0) {
        pkt_delay = (1000 * data_size) / bw;

        // pkt_dalay add 1ms regularly to reduce the offset
        pkt_delay_offset = (((1000 * data_size) % bw) * 10 / bw);
        if (pkt_delay_offset) {
            pkt_delay_offset = 10 / pkt_delay_offset;
        }
    }

    if (send_time == 0) {
        if (num_tag == 1) {
            send_time = 999999;
        } else {
            send_time = 10;
            IPERF_LOGI("Default send times = %d (secs)\n", send_time);
        }
    }

    g_iperf_context.result_t.send_time = send_time;

    // Create a new TCP connection handle
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        IPERF_LOGI("[%s:%d] sockfd = %d\n", __FUNCTION__, __LINE__, sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        if (g_iperf_context.callback)
            g_iperf_context.callback(NULL);
        vTaskDelete(NULL);
    }

    if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0) {
        IPERF_LOGI("Set TOS: fail!\n");
    }

    if (server_port == 0) {
        servaddr.sin_port = htons(IPERF_DEFAULT_PORT);
        IPERF_LOGI("\nDefault server port = %d \n", IPERF_DEFAULT_PORT);
    } else {
        servaddr.sin_port = htons(server_port);
        IPERF_LOGI("\nSet server port = %d \n", server_port);
    }

    if ((connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
        IPERF_LOGI("Connect failed\n");
        close(sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        if (g_iperf_context.callback)
            g_iperf_context.callback(NULL);
        vTaskDelete(NULL);
    }

    str = pvPortMalloc(1 * IPERF_TEST_BUFFER_SIZE);
    if (str == NULL) {
        IPERF_LOGI("not enough buffer to send data!\n");
        close(sockfd);
        if (parameters) {
            vPortFree(parameters);
        }
        if (g_iperf_context.callback)
            g_iperf_context.callback(NULL);
        vTaskDelete(NULL);
    }
    memset(str, 0, IPERF_TEST_BUFFER_SIZE);
    iperf_pattern(str, IPERF_TEST_BUFFER_SIZE);

    // Init UDP data header
    udp_h = (UDP_datagram *)&str[0];
    client_h = (client_hdr *)&str[12];
    if (tradeoff_tag == 1) {
        client_h->flags = htonl(IPERF_HEADER_VERSION1);
    } else {
        client_h->flags = 0;
    }
    client_h->num_threads = htonl(1);
    client_h->port = htonl(IPERF_DEFAULT_PORT);
    client_h->buffer_len = 0;
    client_h->win_band = htonl((bw * 8));
    if (num_tag != 1) { // time mode
        client_h->amount = htonl(~(long)(send_time * bw));
    } else {
        client_h->amount = htonl((long)(total_send));
        client_h->amount &= htonl(0x7FFFFFFF);
    }

    iperf_get_current_time(&t1, &t1_ms);
    last_tick = t1_ms;
    last_sleep = 0;

    do {
        udp_h->id = htonl(udp_h_id++);
        udp_h->tv_sec = htonl((last_tick + last_sleep) / 1000);
        udp_h->tv_usec = htonl(last_tick + last_sleep);

        nbytes = send(sockfd, str, data_size, 0);
        iperf_calculate_result(nbytes, &pkt_count);

        iperf_get_current_time(&curr_t, &current_tick);

        if ((udp_h_id % pkt_delay_offset) == 0) {
            current_sleep = pkt_delay - (current_tick - last_tick - last_sleep) + 1;
        } else {
            current_sleep = pkt_delay - (current_tick - last_tick - last_sleep);
        }

        if ((int)current_sleep > 0) {
            vTaskDelay(current_sleep);
        } else {
            current_sleep = 0;
        }

        last_tick = current_tick;
        last_sleep = current_sleep;

#if defined(IPERF_DEBUG_INTERNAL)
        // show the debug info per second
        if (((bw == 0) && ((udp_h_id % 5000 == 0))) || (udp_h_id % (bw / nbytes) == 0)) {
            DBGPRINT_IPERF(IPERF_DEBUG_SEND, ("\n[%s:%d] nbytes = %d, udp_h_id = %d, pkt_delay = %d, current_tick = %d, current_sleep = %d\n",
                                              __FUNCTION__, __LINE__, nbytes, udp_h_id, pkt_delay, current_tick, current_sleep));
        }
#endif

        if (num_tag == 1) {
            total_send -= nbytes;
        }

        //Reach total receive number "-n"
        if (total_send < 0) {
            IPERF_LOGI("Finish Sending ");
            break;
        }

        if (interval_tag > 0) {
            if (((current_tick - t1_ms) / 10000) == interval_tag) {
                count_t result_count;
                IPERF_LOGI("\nInterval: %d - %d sec   ", (int)(current_tick - t1_ms) / 10000 * 10 - 10, (int)(current_tick - t1_ms) / 10000 * 10);
                iperf_diff_count(&result_count, &pkt_count, &tmp_count);
                iperf_display_report("UDP Client", 10, 0, &result_count);
                iperf_copy_count(&pkt_count, &tmp_count);
                interval_tag++;
            }
            iperf_get_current_time(&curr_t, &current_tick);
        }
    } while ((current_tick + pkt_delay - t1_ms) < send_time * 1000);

    iperf_get_current_time(&t2, 0);
    iperf_display_report("[Total]UDP Client", t2 - t1, 0, &pkt_count);

    // send the last datagram
    udp_h_id = (-udp_h_id);
    udp_h->id = htonl(udp_h_id);
    iperf_get_current_time(&curr_t, 0);
    udp_h->tv_sec = htonl(curr_t);
    udp_h->tv_usec = htonl(curr_t * 1000);

    nbytes = send(sockfd, str, data_size, 0);

    //TODO: receive the report from server side and print out
    if (str) {
        vPortFree(str);
    }
    IPERF_LOGI("\nUDP Client close socket!");
    close(sockfd);

    // tradeoff testing
    if (tradeoff_tag == 1) {
        IPERF_LOGI("Tradoff test, start server-side.");
        g_iperf_is_tradeoff_test_client = 1;
        iperf_udp_run_server(NULL);
        g_iperf_is_tradeoff_test_client = 0;
    }

    if (parameters) {
        vPortFree(parameters);
    }
    if (g_iperf_context.callback)
        g_iperf_context.callback(&g_iperf_context.result_t);

    // For tradeoff mode, task will be deleted in iperf_udp_run_server
    if (g_iperf_is_tradeoff_test_server == 0) {
        vTaskDelete(NULL);
    }
}


void iperf_calculate_result_impl(int pkt_size, count_t *pkt_count)
{
    if (pkt_size > 0) {
        pkt_count->Bytes += pkt_size;
        pkt_count->times++;
    }
}


char * iperf_ftoa_impl(double f, char * buf, int precision)
{
    char * ptr = buf;
    char * p = ptr;
    char * p1;
    char c;
    long intPart;
    char* temp_str;

    // sign stuff
    if (f < 0) {
        f = -f;
        *ptr++ = '-';
    }

    f += (double)0.005;

    intPart = (long)f;
    f -= intPart;

    if (!intPart)
        *ptr++ = '0';
    else {
        // save start pointer
        p = ptr;

        // convert (reverse order)
        while (intPart) {
            *p++ = '0' + intPart % 10;
            intPart /= 10;
        }

        // save end pos
        p1 = p;

        // reverse result
        while (p > ptr) {
            c = *--p;
            *p = *ptr;
            *ptr++ = c;
    }

        // restore end pos
        ptr = p1;
}

    // decimal part
    if (precision) {
        // place decimal point
        *ptr++ = '.';

        // convert
        while (precision--) {
            f *= (double)10.0;
            c = (char)f;
            *ptr++ = '0' + c;
            f -= c;
        }
    }

    // terminating zero
    *ptr = 0;

    temp_str = --ptr;
    while(*temp_str != '.') {
        if(*temp_str == '0') {
            *temp_str = '\0';
        } else {
            break;
        }
        temp_str--;
    }

    if((*(temp_str+1) == '\0') && (*temp_str == '.')) {
        *(temp_str+1) = '0';
    }

    return buf;
}


int iperf_byte_snprintf_impl( char* outString, double inNum, char inFormat)
{
    int conv;

    if ( ! isupper( (int)inFormat ) ) {
        inNum *= 8;
    }

    double tmpNum = inNum;
    conv = kConv_Unit;

    if ( isupper((int)inFormat) ) {
        while ( tmpNum >= (double)1024.0  &&  conv <= kConv_Giga ) {
            tmpNum /= (double)1024.0;
            conv++;
    }
    } else {
        while ( tmpNum >= (double)1000.0  &&  conv <= kConv_Giga ) {
            tmpNum /= (double)1000.0;
            conv++;
        }
    }

    if(conv > 3) {
        conv = 3;
    }

    if ( ! isupper ((int)inFormat) ) {
        inNum *= kConversionForBits[ conv ];
    } else {
        inNum *= kConversion [conv];
    }

    iperf_ftoa(inNum, outString, 2);
    return conv;
} /* end byte_snprintf */


void iperf_display_report_impl(char *report_title, unsigned time, unsigned h_ms_time, count_t *pkt_count)
{
    double tmp_time = time + (double)h_ms_time/(double)10.0;
    char s[9] = {0};
    double tput = 0.0;
    int conv;
    memcpy(g_iperf_context.result_t.report_title, report_title, strlen(report_title));
#if defined(IPERF_DEBUG_ENABLE)
    DBGPRINT_IPERF(IPERF_DEBUG_REPORT, ("\nTransfer in %d.%d seconds: ", time, h_ms_time));
    if (pkt_count->GBytes != 0) {
        DBGPRINT_IPERF(IPERF_DEBUG_REPORT, ("%d GBytes ", pkt_count->GBytes));
    }

    if (pkt_count->MBytes != 0) {
        DBGPRINT_IPERF(IPERF_DEBUG_REPORT, ("%d MBytes ", pkt_count->MBytes));
    }

    if (pkt_count->KBytes != 0) {
        DBGPRINT_IPERF(IPERF_DEBUG_REPORT, ("%d KBytes ", pkt_count->KBytes));
}

    DBGPRINT_IPERF(IPERF_DEBUG_REPORT, ("[%s:%d], time = %d, h_ms_time = %d, GBytes = %d, MBytes = %d, KBytes= %d, Bytes= %d \n", __FUNCTION__, __LINE__,
                                        time, h_ms_time, pkt_count->GBytes, pkt_count->MBytes, pkt_count->KBytes, pkt_count->Bytes));
#endif
    tput = (double)(pkt_count->Bytes);

    conv = iperf_byte_snprintf(s, tput, 'K');
    IPERF_LOGI("The total len: %s %s", s, kLabel_Byte[conv]);

    sprintf(g_iperf_context.result_t.total_len, "%s %s", s, kLabel_Byte[conv]);
    tput = tput/(double)tmp_time;

    conv = iperf_byte_snprintf(s, tput, 'k');

    IPERF_LOGI("%s Bandwidth: %s %s/sec.", report_title, s, kLabel_bit[conv]);

    sprintf(g_iperf_context.result_t.result, "%s %s/sec.", s, kLabel_bit[conv]);

#if defined(IPERF_DEBUG_ENABLE)
    DBGPRINT_IPERF(IPERF_DEBUG_REPORT, ("Receive times: %d\n", pkt_count->times));
#endif

}


void iperf_reset_count_impl(count_t *pkt_count)
{
    pkt_count->Bytes = 0;
    pkt_count->times = 0;
}


void iperf_copy_count_impl(count_t *pkt_count_src, count_t *pkt_count_dest)
{
    pkt_count_dest->Bytes = pkt_count_src->Bytes;
    pkt_count_dest->times = pkt_count_src->times;
}


void iperf_diff_count_impl(count_t *result_count, count_t *pkt_count, count_t *tmp_count)
{
    /* pkt_count > tmp_count */
    result_count->times = pkt_count->times - tmp_count->times;

    if (pkt_count->Bytes >= tmp_count->Bytes) {
        result_count->Bytes = pkt_count->Bytes - tmp_count->Bytes;
    } else {
        IPERF_LOGI("Warning: Diff data is wrong.");
    }

#if defined(IPERF_DEBUG_INTERNAL)
    DBGPRINT_IPERF(IPERF_DEBUG_REPORT, ("\niperf_diff_count: ret.times = %d, ret.GBytes = %d, ret.MBytes = %d, ret.KBytes = %d, ret.Bytes = %d\n",
                                        result_count->times, result_count->GBytes, result_count->MBytes, result_count->KBytes, result_count->Bytes));
#endif
}


void iperf_get_current_time_impl(uint32_t *s, uint32_t *ms)
{
#if 1
    uint32_t dwTick = 0;
    int32_t dwOverflow = 0;

    osKernelSysTickEx(&dwTick, &dwOverflow);

    if(s)
    {
        uint32_t dwSecPerTickOverflow = ((0xFFFFFFFF / osKernelSysTickFrequency) + 1); // ((4294967295 / 1000) + 1) => 4294968
        uint32_t dwSec = dwSecPerTickOverflow * dwOverflow;

        *s = dwSec + (dwTick / osKernelSysTickFrequency);
        //IPERF_LOGI("[%s %d] sec[%u]\n", __func__, __LINE__, *s);
    }

    if(ms)
    {
        *ms = osKernelSysTickToMs(dwTick) % 1000;
        //IPERF_LOGI("[%s %d] ms[%u]\n", __func__, __LINE__, *ms);
    }
#else
    uint32_t count = 0;
    uint64_t count_temp = 0;
    //hal_gpt_status_t ret_status;

    //ret_status = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    //if (HAL_GPT_STATUS_OK != ret_status) {
    //    IPERF_LOGI("[%s:%d]get count error, ret_status = %d", __FUNCTION__, __LINE__, ret_status);
    //}

    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &end_count);
    //hal_gpt_get_duration_count(start_count, end_count, &count);
    if (s) {
        *s = count / 32768;
    }

    if (ms) {
        count_temp = (uint64_t)count * 1000;
        *ms = (uint32_t)(count_temp / 32768);
    }
#endif
}


void iperf_set_debug_mode_impl(uint32_t debug)
{
    g_iperf_debug_feature = debug;
}


int iperf_format_transform_impl(char *param)
{
    char *temp;
    int win_size = 0;
    int i;

    temp = param;

    for (i = 0; temp[i] != '\0'; i++) {
        if (temp[i] == 'k') {
            temp[i] = '\0';
            win_size = (int) (1000 * atof(temp));
        } else if (temp[i] == 'm') {
            temp[i] = '\0';
            win_size = (int) (1000 * 1000 * atof(temp));
        } else if (temp[i] == 'K') {
            temp[i] = '\0';
            win_size = (int) (1024 * atof(temp));
        } else if (temp[i] == 'M') {
            temp[i] = '\0';
            win_size = (int) (1024 * 1024 * atof(temp));
        } else {
            win_size = atoi(param);
        }
    }
    return win_size;
}

/*
 * Initialize the buffer with a pattern of (index mod 10).
 */
void iperf_pattern_impl(char *outBuf, int inBytes)
{
    while (inBytes -- > 0) {
        outBuf[inBytes] = (inBytes % 10) + '0';
    }
}

void iperf_register_callback_impl(iperf_callback_t callback)
{
    g_iperf_context.callback = callback;
}

void Iperf_TaskPreInit(void)
{
    iperf_calculate_result = iperf_calculate_result_impl;
    iperf_display_report = iperf_display_report_impl;
    iperf_reset_count = iperf_reset_count_impl;
    iperf_copy_count = iperf_copy_count_impl;
    iperf_diff_count = iperf_diff_count_impl;
    iperf_ftoa = iperf_ftoa_impl;
    iperf_byte_snprintf = iperf_byte_snprintf_impl;
    iperf_get_current_time = iperf_get_current_time_impl;
    iperf_pattern = iperf_pattern_impl;

    iperf_udp_run_server = iperf_udp_run_server_impl;
    iperf_tcp_run_server = iperf_tcp_run_server_impl;
    iperf_udp_run_client = iperf_udp_run_client_impl;
    iperf_tcp_run_client = iperf_tcp_run_client_impl;
    iperf_set_debug_mode = iperf_set_debug_mode_impl;
    iperf_register_callback = iperf_register_callback_impl;
    iperf_format_transform = iperf_format_transform_impl;

    return;
}

