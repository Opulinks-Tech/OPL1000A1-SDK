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

#ifndef __IPERF_TASK_H__
#define __IPERF_TASK_H__

// Includes --------------------------------------------------------------------
#include <stdint.h>
#include "FreeRTOS.h"
//#include "timer.h"
//#include "lwip/api.h" //netconn API
//#include "lwip/sockets.h" //socket API

#if 1
#define IPERF_LOGE(fmt,arg...)   printf(("[iperf]: "fmt"\n"), ##arg)
#define IPERF_LOGW(fmt,arg...)   printf(("[iperf]: "fmt"\n"), ##arg)
#define IPERF_LOGI(fmt,arg...)   printf(("[iperf]: "fmt"\n"), ##arg)
#else
#define IPERF_LOGE(fmt,arg...)   printf(("\n[iperf]: "fmt), ##arg)
#define IPERF_LOGW(fmt,arg...)   printf(("\n[iperf]: "fmt), ##arg)
#define IPERF_LOGI(fmt,arg...)   printf(("\n[iperf]: "fmt), ##arg)
#endif


// Private typedef -------------------------------------------------------------
typedef struct count_s {
    uint64_t Bytes;
    unsigned KBytes;
    unsigned MBytes;
    unsigned GBytes;
    unsigned times;
} count_t;

// used to reference the 4 byte ID number we place in UDP datagrams
// use int32_t if possible, otherwise a 32 bit bitfield (e.g. on J90)
typedef struct UDP_datagram {
    int32_t id;
    unsigned int tv_sec;
    unsigned int tv_usec;
} UDP_datagram;

/*
 * The client_hdr structure is sent from clients
 * to servers to alert them of things that need
 * to happen. Order must be perserved in all
 * future releases for backward compatibility.
 * 1.7 has flags, num_threads, port, and buffer_len
 */
typedef struct client_hdr {
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     * The lowest order diferentiates between dualtest and
     * tradeoff modes, wheither the speaker needs to start
     * immediately or after the audience finishes.
     */
    int32_t flags;
    int32_t num_threads;
    int32_t port;
    int32_t buffer_len;
    int32_t win_band;
    int32_t amount;
} client_hdr;

/*
 * The server_hdr structure facilitates the server
 * report of jitter and loss on the client side.
 * It piggy_backs on the existing clear to close
 * packet.
 */
typedef struct server_hdr {
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     */
    int32_t flags;
    int32_t total_len1;
    int32_t total_len2;
    int32_t stop_sec;
    int32_t stop_usec;
    int32_t error_cnt;
    int32_t outorder_cnt;
    int32_t datagrams;
    int32_t jitter1;
    int32_t jitter2;
} server_hdr;

typedef struct iperf_result_struct {
    int32_t data_size;
    int32_t send_time;
    char total_len[20];
    char result[20];
    char report_title[20];
} iperf_result_t;


// Private macro ---------------------------------------------------------------
#define IPERF_DEFAULT_PORT  5001 //Port to listen
#define IPERF_USE_PBUF      1    //Set to nonzero to use pbuf (faster) instead of netbuf (safer)

#define IPERF_HEADER_VERSION1 0x80000000
#define IPERF_DEFAULT_UDP_RATE (1024 * 1024)
#define IPERF_TEST_BUFFER_SIZE (1460)
#define IPERF_COMMAND_BUFFER_NUM (18)
#define IPERF_COMMAND_BUFFER_SIZE (20) // 4 bytes align

#define DBGPRINT_IPERF(FEATURE, _Fmt)            \
        {                                        \
            if (g_iperf_debug_feature & FEATURE) \
            {                                    \
                printf _Fmt;                     \
            }                                    \
        }

#define IPERF_DEBUG_RECEIVE     (1<<0)
#define IPERF_DEBUG_SEND        (1<<1)
#define IPERF_DEBUG_REPORT      (1<<2)

typedef void (* iperf_callback_t)(iperf_result_t* iperf_result);

#if 1
typedef void (*T_IperfCommomFp)(char *parameters[]);
typedef void (*T_IperfSetDebugModeFp)(uint32_t debug);
typedef void (*T_IperfRegisterCallbackFp)(iperf_callback_t callback);
typedef int (*T_IperfFormatTransformFp)(char *param);
typedef void (*T_IperfCalculateResultFp)(int pkt_size, count_t *pkt_count);
typedef void (*T_IperfDisplayReportFp)(char *report_title, unsigned time, unsigned h_ms_time, count_t *pkt_count);
typedef void (*T_IperfResetCountFp)(count_t *pkt_count);
typedef void (*T_IperfCopyCountFp)(count_t *pkt_count_src, count_t *pkt_count_dest);
typedef void (*T_IperfDiffCountFp)(count_t *result_count, count_t *pkt_count, count_t *tmp_count);
typedef char* (*T_IperfFtoaFp)(double f, char * buf, int precision);
typedef int (*T_IperfByteSnprintfFp)(char* outString, double inNum, char inFormat);
typedef void (*T_IperfGetCurrentTimeFp)(uint32_t *s, uint32_t *ms);
typedef void (*T_IperfPatternFp)(char *outBuf, int inBytes);

extern T_IperfCommomFp iperf_udp_run_server;
extern T_IperfCommomFp iperf_tcp_run_server;
extern T_IperfCommomFp iperf_udp_run_client;
extern T_IperfCommomFp iperf_tcp_run_client;
extern T_IperfSetDebugModeFp iperf_set_debug_mode;
extern T_IperfRegisterCallbackFp iperf_register_callback;
extern T_IperfFormatTransformFp iperf_format_transform;

void Iperf_TaskPreInit(void);
#else
// Private function prototypes -------------------------------------------------
void iperf_udp_run_server(char *parameters[]);
void iperf_tcp_run_server(char *parameters[]);
void iperf_udp_run_client(char *parameters[]);
void iperf_tcp_run_client(char *parameters[]);
void iperf_set_debug_mode(uint32_t debug);
void iperf_register_callback(iperf_callback_t callback);
int iperf_format_transform(char *param);
#endif

#endif /* __IPERF_TASK_H__ */
