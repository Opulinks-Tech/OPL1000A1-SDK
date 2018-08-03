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
#include <stdlib.h>
#include <string.h>

#include "iperf_cli.h"
#include "iperf_task.h"
#include "task.h"
#include "msg.h"
#include "sys_os_config.h"


/* for iperf task */
#define IPERF_TASK_NAME                OS_TASK_NAME_IPERF
#define IPERF_TASK_STACKSIZE           OS_TASK_STACK_SIZE_IPERF
#define IPERF_TASK_PRIO                (OS_TASK_PRIORITY_IPERF - OS_TASK_PRIORITY_IDLE)     // if use FreeRTOS API directly,
                                                                                            // translate from cmsis to FreeRTOS

RET_DATA cli_command_t *iperf_cli;
RET_DATA T_IperfCmdProc iperf_cmd_proc;

static int _cli_iperf_server(int len, char *param[])
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    g_iperf_param = pvPortMalloc(IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);
    if (g_iperf_param == NULL) {
        IPERF_LOGI("Warning: No enough memory to running iperf.");
        return 0;
    }

    memset(g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);

#if defined(IPERF_DEBUG_INTERNAL)
    IPERF_LOGI("_cli_iperf_server, g_iperf_param = 0x%x, param = 0x%x", g_iperf_param, param);
#endif

    for (i = 0; i < 13 && i < len; i++) {
        strcpy((char *)&g_iperf_param[i * offset], param[i]);
#if defined(IPERF_DEBUG_INTERNAL)
        IPERF_LOGI("_cli_iperf_client, g_iperf_param[%d] is \"%s\"", i, (char *)&g_iperf_param[i * offset]);
#endif
        if (param[i][0] == 0 &&  param[i][1] == 0) {
            break;
        }
    }

    for (i = 0; i < 13 && i < len; i++) {
        if (strcmp(param[i], "-u") == 0) {
            IPERF_LOGI("Iperf UDP Server: Start!");
            IPERF_LOGI("Iperf UDP Server Receive Timeout = 20 (secs)");
            xTaskCreate((TaskFunction_t)iperf_udp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE, g_iperf_param, IPERF_TASK_PRIO , NULL);
            is_create_task = 1;
            break;
        }

    }

    if (0 == is_create_task) {
        IPERF_LOGI("Iperf TCP Server: Start!");
        IPERF_LOGI("Iperf TCP Server Receive Timeout = 20 (secs)");
        xTaskCreate((TaskFunction_t)iperf_tcp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE, g_iperf_param, IPERF_TASK_PRIO , NULL);
        is_create_task = 1;
    }

    return 0;
}

static int _cli_iperf_client(int len, char *param[])
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    g_iperf_param = pvPortMalloc(IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);
    if (g_iperf_param == NULL) {
        IPERF_LOGI("Warning: No enough memory to running iperf.");
        return 0;
    }

    memset(g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);

    for (i = 0; i < 18 && i < len; i++) {
        strcpy((char *)&g_iperf_param[i * offset], param[i]);

#if defined(IPERF_DEBUG_INTERNAL)
        IPERF_LOGI("_cli_iperf_client, g_iperf_param[%d] is \"%s\"", i, (char *)&g_iperf_param[i * offset]);
#endif

        if (param[i][0] == 0 &&  param[i][1] == 0) {
            break;
        }
    }

    for (i = 0; i < 18 && i < len; i++) {
        if (strcmp(param[i], "-u") == 0) {
            IPERF_LOGI("Iperf UDP Client: Start!");
            xTaskCreate((TaskFunction_t)iperf_udp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE, g_iperf_param, IPERF_TASK_PRIO , NULL);
            is_create_task = 1;
            break;
        }
    }

    if (0 == is_create_task) {
        IPERF_LOGI("Iperf TCP Client: Start!");
        xTaskCreate((TaskFunction_t)iperf_tcp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE, g_iperf_param, IPERF_TASK_PRIO , NULL);
        is_create_task = 1;
    }

    return 0;
}

static int _cli_iperf_help(int len, char *param[])
{

    IPERF_LOGI("Usage: iperf [-s|-c] [options]");
    IPERF_LOGI("       iperf [-h]\n");
    IPERF_LOGI("Client/Server:");
    IPERF_LOGI("  -u,               use UDP rather than TCP");
    IPERF_LOGI("  -p,    #          server port to listen on/connect to (default 5001)");
    IPERF_LOGI("  -n,    #[kmKM]    number of bytes to transmit ");
    IPERF_LOGI("  -b,    #[kmKM]    for UDP, bandwidth to send at in bits/sec");
    IPERF_LOGI("  -i,               10 seconds between periodic bandwidth reports \n");
    IPERF_LOGI("Server specific:");
    IPERF_LOGI("  -s,               run in server mode");
    IPERF_LOGI("  -B,    <ip>       bind to <ip>, and join to a multicast group (only Support UDP)");
    IPERF_LOGI("  -r,               for UDP, run iperf in tradeoff testing mode, connecting back to client\n");
    IPERF_LOGI("Client specific:");
    IPERF_LOGI("  -c,    <ip>       run in client mode, connecting to <ip>");
    IPERF_LOGI("  -w,    #[kmKM]    TCP window size");
    IPERF_LOGI("  -l,    #[kmKM]    UDP datagram size");
    IPERF_LOGI("  -t,    #          time in seconds to transmit for (default 10 secs)");
    IPERF_LOGI("  -S,    #          the type-of-service of outgoing packets\n");
    IPERF_LOGI("Miscellaneous:");
    IPERF_LOGI("  -h,               print this message and quit\n");
    IPERF_LOGI("[kmKM] Indicates options that support a k/K or m/M suffix for kilo- or mega-\n");
    IPERF_LOGI("TOS options for -S parameter:");
    IPERF_LOGI("BE: -S 0");
    IPERF_LOGI("BK: -S 32");
    IPERF_LOGI("VI: -S 160");
    IPERF_LOGI("VO: -S 224\n");
    IPERF_LOGI("Tradeoff Testing Mode:");
    IPERF_LOGI("Command: iperf -s -u -n <bits/bytes> -r \n");
    IPERF_LOGI("Example:");
    IPERF_LOGI("Iperf TCP Server: iperf -s");
    IPERF_LOGI("Iperf UDP Server: iperf -s -u");
    IPERF_LOGI("Iperf TCP Client: iperf -c <ip> -w <window size> -t <duration> -p <port>");
    IPERF_LOGI("Iperf UDP Client: iperf -c <ip> -u -l <datagram size> -t <duration> -p <port>");
    return 0;
}

#if defined(IPERF_DEBUG_ENABLE)
static uint8_t _cli_iperf_debug(uint8_t len, char *param[])
{
    int debug;
    debug = atoi(param[0]);
    IPERF_LOGI("Set iperf debug to %d(0x%x)\n", debug, debug);
    iperf_set_debug_mode(debug);
    return 0;
}
#endif

cli_command_t iperf_cli_body[] = {
    { "-s",        "iperf server",               _cli_iperf_server   , NULL},
    { "-c",        "iperf client",               _cli_iperf_client   , NULL},
    { "-h",                "help",               _cli_iperf_help     , NULL},
#if defined(IPERF_DEBUG_ENABLE)
    { "-d",               "debug",               _cli_iperf_debug    , NULL},
#endif
    { NULL }

};

void iperf_cmd_proc_impl(int len, char *param[])
{
    cli_command_t *ptCmd = iperf_cli;

    // Example:
    // param[0]: -c
    // param[1]: ip
    // ...

    while(ptCmd->cmd != NULL)
    {
        if(!strcmp(ptCmd->cmd, param[0]))
        {
            ptCmd->cmd_handle(len - 1, &(param[1]));
            goto done;
        }

        ptCmd += 1;
    }

    if(ptCmd == NULL)
    {
        IPERF_LOGI("command not found\n");
    }

done:
    return;
}

void Iperf_PreInit(void)
{
    iperf_cli = iperf_cli_body;
    iperf_cmd_proc = iperf_cmd_proc_impl;

    Iperf_TaskPreInit();

    return;
}

